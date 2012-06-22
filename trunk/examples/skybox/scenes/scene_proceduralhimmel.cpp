
// Copyright (c) 2011-2012, Daniel Müller <dm@g4t3.de>
// Computer Graphics Systems Group at the Hasso-Plattner-Institute, Germany
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright notice, 
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright 
//     notice, this list of conditions and the following disclaimer in the 
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of the Computer Graphics Systems Group at the 
//     Hasso-Plattner-Institute (HPI), Germany nor the names of its 
//     contributors may be used to endorse or promote products derived from 
//     this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.

#include "scene_proceduralhimmel.h"

#include "utils/tr.h"
#include "utils/qt2osg.h"
#include "utils/shadermodifier.h"

#include "osgHimmel/himmel.h"
#include "osgHimmel/atmospheregeode.h"
#include "osgHimmel/moongeode.h"
#include "osgHimmel/moonglaregeode.h"
#include "osgHimmel/starsgeode.h"
#include "osgHimmel/starmapgeode.h"
#include "osgHimmel/abstractastronomy.h"
#include "osgHimmel/dubecloudlayergeode.h"
#include "osgHimmel/highcloudlayergeode.h"

#include <osg/Light>
#include <osg/LightSource>


namespace
{
    // Properties

    const QString GROUP_PROCEDURAL                  (TR("Procedrual"));

    const QString GROUP_PROCEDURAL_ATMOSPHERE       (TR("Atmosphere"));
    const QString PROPERTY_ATM_SUNSCALE             (TR("Sun Scale"));
    const QString PROPERTY_ATM_EXPOSURE             (TR("Exposure"));
    const QString PROPERTY_ATM_BLUEHOUR_COLOR       (TR("Blue Hour Color"));
    const QString PROPERTY_ATM_BLUEHOUR_INTENSITY   (TR("Blue Hour Intensity"));
    const QString PROPERTY_ATM_AVGGR                (TR("Avg Ground Reflectance"));
    const QString PROPERTY_ATM_HR                   (TR("Rayleigh Height Scale"));
    const QString PROPERTY_ATM_BETARR               (TR("Rayleigh Coefficient R"));
    const QString PROPERTY_ATM_BETARG               (TR("Rayleigh Coefficient G"));
    const QString PROPERTY_ATM_BETARB               (TR("Rayleigh Coefficient B"));
    const QString PROPERTY_ATM_HM                   (TR("Mie Height Scale"));
    const QString PROPERTY_ATM_BETAM                (TR("Mie Coefficient"));
    const QString PROPERTY_ATM_MIEG                 (TR("Mie G"));

    const QString GROUP_PROCEDURAL_MOON             (TR("Moon"));
    const QString PROPERTY_MOON_SCALE               (TR("Moon Scale"));
    const QString PROPERTY_MOON_SUNSHINE_COLOR      (TR("Sun Shine Color"));
    const QString PROPERTY_MOON_SUNSHINE_INTENSITY  (TR("Sun Shine Intensity"));
    const QString PROPERTY_MOON_EARTHSHINE_COLOR    (TR("Earth Shine Color"));
    const QString PROPERTY_MOON_EARTHSHINE_INTENSITY(TR("Earth Shine Intensity"));

    const QString GROUP_PROCEDURAL_STARS            (TR("Stars"));

    const QString PROPERTY_STARS_COLOR              (TR("Color (Stars)"));
    const QString PROPERTY_STARS_COLOR_RATIO        (TR("Color Ratio (Stars)"));
    const QString PROPERTY_STARS_GLARE_INTENSITY    (TR("Glare Intensity"));
    const QString PROPERTY_STARS_GLARE_SCALE        (TR("Glare Scale"));
    const QString PROPERTY_STARS_APPARENT_MAG       (TR("Apprant Magnitude (Stars)"));
    const QString PROPERTY_STARS_SCATTERING         (TR("Scattering (Stars)"));
    const QString PROPERTY_STARS_SCINTILLATION      (TR("Scintillation"));

    const QString GROUP_PROCEDURAL_MILKYWAY         (TR("StarMap"));

    const QString PROPERTY_MILKYWAY_COLOR           (TR("Color (StarMap)"));
    const QString PROPERTY_MILKYWAY_COLOR_RATIO     (TR("Color Ratio (StarMap)"));
    const QString PROPERTY_MILKYWAY_APPARENT_MAG    (TR("Apparent Magnitude (StarMap)"));
    const QString PROPERTY_MILKYWAY_SCATTERING      (TR("Scattering (StarMap)"));

    const QString GROUP_PROCEDURAL_CLOUDS1          (TR("HighCloudLayer1"));

    const QString PROPERTY_CLOUDS1_ALTITUDE         (TR("Altitude (km)"));
    const QString PROPERTY_CLOUDS1_CHANGE           (TR("Change"));
    const QString PROPERTY_CLOUDS1_SCALE            (TR("Scale"));
    const QString PROPERTY_CLOUDS1_COVERAGE         (TR("Coverage"));
    const QString PROPERTY_CLOUDS1_SHARPNESS        (TR("Sharpness"));
    const QString PROPERTY_CLOUDS1_WINDX            (TR("Wind-X"));
    const QString PROPERTY_CLOUDS1_WINDY            (TR("Wind-Y")); 

    const QString GROUP_PROCEDURAL_CLOUDS2          (TR("HighCloudLayer1"));

    const QString PROPERTY_CLOUDS2_ALTITUDE         (TR("Altitude (km)"));
    const QString PROPERTY_CLOUDS2_CHANGE           (TR("Change"));
    const QString PROPERTY_CLOUDS2_SCALE            (TR("Scale"));
    const QString PROPERTY_CLOUDS2_COVERAGE         (TR("Coverage"));
    const QString PROPERTY_CLOUDS2_SHARPNESS        (TR("Sharpness"));
    const QString PROPERTY_CLOUDS2_WINDX            (TR("Wind-X"));
    const QString PROPERTY_CLOUDS2_WINDY            (TR("Wind-Y"));
}


using namespace osgHimmel;


CameraLock::CameraLock(Himmel *himmel, osg::Camera *camera)
:   m_camera(camera)
,   m_himmel(himmel)
,   m_target(T_None)
{
}

void CameraLock::operator()(
    osg::Node *node
,   osg::NodeVisitor *nv)
{
    traverse(node, nv);
    
    osg::Vec3 target(0.0, 0.0, 0.0);

    switch(m_target)
    {
    case T_Moon:
        target = m_himmel->astro()->getMoonPosition(true);
        break;
    case T_Sun:
        target = m_himmel->astro()->getSunPosition(true);
        break;
    default:
        return;
    }
    m_camera->setViewMatrix(osg::Matrix::lookAt(
        osg::Vec3(0.0, 0.0, 0.0), target, osg::Vec3(0.0, 0.0, 1.0))); 
}

void CameraLock::setTarget(const CameraLock::e_Target target)
{
    m_target = target;
}


// TEMP
//
//#include <osg/NodeCallback>
//#include <osgUtil/CullVisitor>
//
//#include <math.h>
//
//class ns : public osg::NodeCallback
//{
//    public:
//    
//        ns(osg::Light *light, Himmel *himmel)
//        :   m_ligth(light)
//        ,   m_himmel(himmel)
//        {
//        }
//       
//        virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
//        {
//            traverse(node, nv);
//            
//            osg::Vec3 p = m_himmel->getSunPosition();
//
//            float t1 = 1.0 / sqrt(1.0 + pow(-p.z() + 1.1, 32)) + 0.05;
//            float t2 = 1.0 / sqrt(1.0 + pow(-p.z() + 1.2, 16));
//
//            p *= 10000;
//            m_ligth->setPosition(osg::Vec4(p.x(), p.y(), p.z(), 1.0));
//            //m_ligth->setDiffuse(osg::Vec4(1.0, 1.0, 1.0, 1.0));
//            m_ligth->setDiffuse(osg::Vec4(1.0 * t2, 0.95 * t2, 0.9 * t2, 1.0));
//        }
//
//protected:
//    osg::Light *m_ligth;
//    Himmel* m_himmel;
//};


Scene_ProceduralHimmel::Scene_ProceduralHimmel(osg::Camera *camera)
:   AbstractHimmelScene(camera)
,   m_himmel(NULL)
,   m_cameraLock(NULL)
{
    initializeProperties();

    m_himmel = Himmel::create();
    addChild(m_himmel);
  
   
    // TEMP

    osg::Group *group = new osg::Group();

    osg::Light *light(new osg::Light);
    osg::LightSource *lsource(new osg::LightSource);

    lsource->setLight(light);
    addChild(group);
    group->addChild(lsource);

    lsource->setLocalStateSetModes(osg::StateAttribute::ON);

    lsource->setStateSetModes(*group->getOrCreateStateSet(), osg::StateAttribute::ON);

    m_cameraLock = new CameraLock(m_himmel, camera);
    m_himmel->addUpdateCallback(m_cameraLock);
//    m_himmel->addUpdateCallback(new ns(light, m_himmel));
}


Scene_ProceduralHimmel::~Scene_ProceduralHimmel()
{
}


void Scene_ProceduralHimmel::setCameraLockTarget(const CameraLock::e_Target target)
{
    m_cameraLock->setTarget(target);
}


AbstractHimmel *Scene_ProceduralHimmel::himmel()
{
    return m_himmel;
}


void Scene_ProceduralHimmel::postInitialize()
{
    if(m_himmel->starmap())
    {
        shaderModifier()->registerShader(m_himmel->starmap()->getName(), m_himmel->starmap()->getVertexShader());
        shaderModifier()->registerShader(m_himmel->starmap()->getName(), m_himmel->starmap()->getGeometryShader());
        shaderModifier()->registerShader(m_himmel->starmap()->getName(), m_himmel->starmap()->getFragmentShader());
    }

    if(m_himmel->stars())
    {
        shaderModifier()->registerShader(m_himmel->stars()->getName(), m_himmel->stars()->getVertexShader());
        shaderModifier()->registerShader(m_himmel->stars()->getName(), m_himmel->stars()->getGeometryShader());
        shaderModifier()->registerShader(m_himmel->stars()->getName(), m_himmel->stars()->getFragmentShader());
    }

    if(m_himmel->atmosphere())
    {
        shaderModifier()->registerShader(m_himmel->atmosphere()->getName(), m_himmel->atmosphere()->getVertexShader());
        shaderModifier()->registerShader(m_himmel->atmosphere()->getName(), m_himmel->atmosphere()->getGeometryShader());
        shaderModifier()->registerShader(m_himmel->atmosphere()->getName(), m_himmel->atmosphere()->getFragmentShader());
    }

    if(m_himmel->moon())
    {
        shaderModifier()->registerShader(m_himmel->moon()->getName(), m_himmel->moon()->getVertexShader());
        shaderModifier()->registerShader(m_himmel->moon()->getName(), m_himmel->moon()->getGeometryShader());
        shaderModifier()->registerShader(m_himmel->moon()->getName(), m_himmel->moon()->getFragmentShader());

        //shaderModifier()->registerShader(m_himmel->moonGlare()->getName(), m_himmel->moonGlare()->getVertexShader());
        //shaderModifier()->registerShader(m_himmel->moonGlare()->getName(), m_himmel->moonGlare()->getGeometryShader());
        //shaderModifier()->registerShader(m_himmel->moonGlare()->getName(), m_himmel->moonGlare()->getFragmentShader());
    }

        if(m_himmel->highLayer())
    {
        shaderModifier()->registerShader(m_himmel->highLayer()->getName(), m_himmel->highLayer()->getVertexShader());
        shaderModifier()->registerShader(m_himmel->highLayer()->getName(), m_himmel->highLayer()->getGeometryShader());
        shaderModifier()->registerShader(m_himmel->highLayer()->getName(), m_himmel->highLayer()->getFragmentShader());
    }

    if(m_himmel->dubeLayer())
    {
        shaderModifier()->registerShader(m_himmel->dubeLayer()->getName(), m_himmel->dubeLayer()->getVertexShader());
        shaderModifier()->registerShader(m_himmel->dubeLayer()->getName(), m_himmel->dubeLayer()->getGeometryShader());
        shaderModifier()->registerShader(m_himmel->dubeLayer()->getName(), m_himmel->dubeLayer()->getFragmentShader());
    }
}


void Scene_ProceduralHimmel::registerProperties()
{
    AbstractHimmelScene::registerProperties();

    QtProperty *proceduralGroup = createGroup(GROUP_PROCEDURAL);

    createProperty(*proceduralGroup, PROPERTY_ATM_SUNSCALE, AtmosphereGeode::defaultSunScale(), 0.0,   100.0, 0.25); 
    createProperty(*proceduralGroup, PROPERTY_ATM_EXPOSURE, AtmosphereGeode::defaultExposure(), 0.0, 2.0, 0.02); 
    createProperty(*proceduralGroup, PROPERTY_ATM_BLUEHOUR_COLOR, toQColor(AtmosphereGeode::defaultLHeureBleueColor())); 
    createProperty(*proceduralGroup, PROPERTY_ATM_BLUEHOUR_INTENSITY, AtmosphereGeode::defaultLHeureBleueIntensity(), 0.0, 2.0, 0.02); 
    createProperty(*proceduralGroup, PROPERTY_ATM_AVGGR, 0.1, 0.0, 4.0, 0.1); 
    createProperty(*proceduralGroup, PROPERTY_ATM_HR, 8.0, 0.0, 999.0, 0.1); 
    createProperty(*proceduralGroup, PROPERTY_ATM_BETARR,  5.8, 0.0, 999.0, 0.1); 
    createProperty(*proceduralGroup, PROPERTY_ATM_BETARG, 13.5, 0.0, 999.0, 0.1); 
    createProperty(*proceduralGroup, PROPERTY_ATM_BETARB, 33.1, 0.0, 999.0, 0.1); 
    createProperty(*proceduralGroup, PROPERTY_ATM_HM, 1.2, 0.0, 999.0, 0.1); 
    createProperty(*proceduralGroup, PROPERTY_ATM_BETAM, 8.0, 0.0, 999.0, 0.1); 
    createProperty(*proceduralGroup, PROPERTY_ATM_MIEG,  0.76, -1.0, 1.0, 0.02); 

    QtProperty *moonGroup = createGroup(GROUP_PROCEDURAL_MOON);

    createProperty(*moonGroup, PROPERTY_MOON_SCALE, MoonGeode::defaultScale(), 0.0, 100.0, 0.25); 
    createProperty(*moonGroup, PROPERTY_MOON_SUNSHINE_COLOR, toQColor(MoonGeode::defaultSunShineColor())); 
    createProperty(*moonGroup, PROPERTY_MOON_SUNSHINE_INTENSITY, MoonGeode::defaultSunShineIntensity(), 0.0, 100.0, 0.25); 
    createProperty(*moonGroup, PROPERTY_MOON_EARTHSHINE_COLOR, toQColor(MoonGeode::defaultEarthShineColor())); 
    createProperty(*moonGroup, PROPERTY_MOON_EARTHSHINE_INTENSITY, MoonGeode::defaultEarthShineIntensity(), 0.0, 100.0, 0.25); 

    QtProperty *starsGroup = createGroup(GROUP_PROCEDURAL_STARS);

    createProperty(*starsGroup, PROPERTY_STARS_COLOR, toQColor(StarsGeode::defaultColor())); 
    createProperty(*starsGroup, PROPERTY_STARS_COLOR_RATIO, StarsGeode::defaultColorRatio(), 0.0, 1.0, 0.1); 
    createProperty(*starsGroup, PROPERTY_STARS_GLARE_INTENSITY, 1.0, 0.0, 100.0, 0.1); 
    createProperty(*starsGroup, PROPERTY_STARS_GLARE_SCALE, StarsGeode::defaultGlareScale(), 0.0, 100.0, 0.1); 
    createProperty(*starsGroup, PROPERTY_STARS_SCATTERING, StarsGeode::defaultScattering(), 0.0, 100.0, 0.1); 
    createProperty(*starsGroup, PROPERTY_STARS_SCINTILLATION, StarsGeode::defaultScintillation(), 0.0, 100.0, 0.1); 
    createProperty(*starsGroup, PROPERTY_STARS_APPARENT_MAG, StarsGeode::defaultApparentMagnitude(), -32.0, 32.0, 0.1);


    QtProperty *starmapGroup = createGroup(GROUP_PROCEDURAL_MILKYWAY);

    createProperty(*starmapGroup, PROPERTY_MILKYWAY_COLOR, toQColor(StarMapGeode::defaultColor())); 
    createProperty(*starmapGroup, PROPERTY_MILKYWAY_COLOR_RATIO, StarMapGeode::defaultColorRatio(), 0.0, 10.0, 0.25); 
    createProperty(*starmapGroup, PROPERTY_MILKYWAY_APPARENT_MAG, StarMapGeode::defaultApparentMagnitude(), -32.0, 32.0, 0.1);
    createProperty(*starmapGroup, PROPERTY_MILKYWAY_SCATTERING, StarMapGeode::defaultScattering(), 0.0, 100.0, 0.1);


    QtProperty *clouds1Group = createGroup(GROUP_PROCEDURAL_CLOUDS1);

    createProperty(*clouds1Group, PROPERTY_CLOUDS1_ALTITUDE, HighCloudLayerGeode::defaultAltitude(), 1.f, 16.f, 0.1f); 
    createProperty(*clouds1Group, PROPERTY_CLOUDS1_CHANGE, HighCloudLayerGeode::defaultChange(), 0.f, 1.f, 0.02f); 
    createProperty(*clouds1Group, PROPERTY_CLOUDS1_SCALE, HighCloudLayerGeode::defaultScale(), 1.f, 1024.f, 16.f); 
    createProperty(*clouds1Group, PROPERTY_CLOUDS1_COVERAGE, 0.f, 0.f, 1.f, 0.02f); 
    createProperty(*clouds1Group, PROPERTY_CLOUDS1_SHARPNESS, 1.f, 0.f, 2.f, 0.01f); 
    createProperty(*clouds1Group, PROPERTY_CLOUDS1_WINDX, 0.f, -8.f, 8.f, 0.02f); 
    createProperty(*clouds1Group, PROPERTY_CLOUDS1_WINDY, 0.f, -8.f, 8.f, 0.02f); 
}


void Scene_ProceduralHimmel::propertyChanged(
    QtProperty *p
,   const QString &name)
{

    if(m_himmel->atmosphere())
    {
         if(PROPERTY_ATM_SUNSCALE == name)
        m_himmel->atmosphere()->setSunScale(doubleValue(PROPERTY_ATM_SUNSCALE));
    else if(PROPERTY_ATM_EXPOSURE == name)
        m_himmel->atmosphere()->setExposure(doubleValue(PROPERTY_ATM_EXPOSURE)); 
    else if(PROPERTY_ATM_BLUEHOUR_COLOR == name)
        m_himmel->atmosphere()->setLHeureBleueColor(toVec3(colorValue(PROPERTY_ATM_BLUEHOUR_COLOR)));
    else if(PROPERTY_ATM_BLUEHOUR_INTENSITY == name)
        m_himmel->atmosphere()->setLHeureBleueIntensity(doubleValue(PROPERTY_ATM_BLUEHOUR_INTENSITY)); 
    else if(PROPERTY_ATM_AVGGR == name)
        m_himmel->atmosphere()->setAverageGroundReflectance(doubleValue(PROPERTY_ATM_AVGGR)); 
    else if(PROPERTY_ATM_HR == name)
        m_himmel->atmosphere()->setThicknessRayleigh(doubleValue(PROPERTY_ATM_HR)); 
    else if(PROPERTY_ATM_BETARR == name
         || PROPERTY_ATM_BETARG == name
         || PROPERTY_ATM_BETARB == name)
        m_himmel->atmosphere()->setScatteringRayleigh(
            osg::Vec3(doubleValue(PROPERTY_ATM_BETARR), doubleValue(PROPERTY_ATM_BETARG), doubleValue(PROPERTY_ATM_BETARB)) * 1e-3f); 
    else if(PROPERTY_ATM_HM == name)
        m_himmel->atmosphere()->setThicknessMie(doubleValue(PROPERTY_ATM_HM)); 
    else if(PROPERTY_ATM_BETAM == name)
        m_himmel->atmosphere()->setScatteringMie(doubleValue(PROPERTY_ATM_BETAM) * 1e-3); 
    else if(PROPERTY_ATM_MIEG == name)
        m_himmel->atmosphere()->setPhaseG(doubleValue(PROPERTY_ATM_MIEG)); 
    }

    if(m_himmel->moon())
    {
         if(PROPERTY_MOON_SCALE == name)
        m_himmel->moon()->setScale(doubleValue(PROPERTY_MOON_SCALE));
    else if(PROPERTY_MOON_SUNSHINE_COLOR == name)
        m_himmel->moon()->setSunShineColor(toVec3(colorValue(PROPERTY_MOON_SUNSHINE_COLOR)));
    else if(PROPERTY_MOON_SUNSHINE_INTENSITY == name)
        m_himmel->moon()->setSunShineIntensity(doubleValue(PROPERTY_MOON_SUNSHINE_INTENSITY));
    else if(PROPERTY_MOON_EARTHSHINE_COLOR == name)
        m_himmel->moon()->setEarthShineColor(toVec3(colorValue(PROPERTY_MOON_EARTHSHINE_COLOR)));
    else if(PROPERTY_MOON_EARTHSHINE_INTENSITY == name)
        m_himmel->moon()->setEarthShineIntensity(doubleValue(PROPERTY_MOON_EARTHSHINE_INTENSITY));
    }


    if(m_himmel->stars())
    {
         if(PROPERTY_STARS_COLOR == name)
        m_himmel->stars()->setColor(toVec3(colorValue(PROPERTY_STARS_COLOR)));
    else if(PROPERTY_STARS_COLOR_RATIO == name)
        m_himmel->stars()->setColorRatio(doubleValue(PROPERTY_STARS_COLOR_RATIO));
    else if(PROPERTY_STARS_GLARE_INTENSITY == name)
        m_himmel->stars()->setGlareIntensity(doubleValue(PROPERTY_STARS_GLARE_INTENSITY));
    else if(PROPERTY_STARS_GLARE_SCALE == name)
        m_himmel->stars()->setGlareScale(doubleValue(PROPERTY_STARS_GLARE_SCALE));
    else if(PROPERTY_STARS_SCATTERING == name)
        m_himmel->stars()->setScattering(doubleValue(PROPERTY_STARS_SCATTERING));
    else if(PROPERTY_STARS_SCINTILLATION == name)
        m_himmel->stars()->setScintillation(doubleValue(PROPERTY_STARS_SCINTILLATION));
    else if(PROPERTY_STARS_APPARENT_MAG == name)
        m_himmel->stars()->setApparentMagnitude(doubleValue(PROPERTY_STARS_APPARENT_MAG));
    }


    if(m_himmel->starmap())
    {
         if(PROPERTY_MILKYWAY_COLOR == name)
        m_himmel->starmap()->setColor(toVec3(colorValue(PROPERTY_MILKYWAY_COLOR)));
    else if(PROPERTY_MILKYWAY_COLOR_RATIO == name)
        m_himmel->starmap()->setColorRatio(doubleValue(PROPERTY_MILKYWAY_COLOR_RATIO));
    else if(PROPERTY_MILKYWAY_APPARENT_MAG == name)
        m_himmel->starmap()->setApparentMagnitude(doubleValue(PROPERTY_MILKYWAY_APPARENT_MAG));
    else if(PROPERTY_MILKYWAY_SCATTERING == name)
        m_himmel->starmap()->setScattering(doubleValue(PROPERTY_MILKYWAY_SCATTERING));
    }


    if(m_himmel->highLayer())
    {
         if(PROPERTY_CLOUDS1_ALTITUDE == name)
        m_himmel->highLayer()->setAltitude(doubleValue(PROPERTY_CLOUDS1_ALTITUDE));
    else if(PROPERTY_CLOUDS1_CHANGE == name)
        m_himmel->highLayer()->setChange(doubleValue(PROPERTY_CLOUDS1_CHANGE));
    else if(PROPERTY_CLOUDS1_SCALE == name)
        m_himmel->highLayer()->setScale(doubleValue(PROPERTY_CLOUDS1_SCALE));
    else if(PROPERTY_CLOUDS1_COVERAGE == name)
        m_himmel->highLayer()->setCoverage(doubleValue(PROPERTY_CLOUDS1_COVERAGE));
    else if(PROPERTY_CLOUDS1_SHARPNESS == name)
        m_himmel->highLayer()->setSharpness(doubleValue(PROPERTY_CLOUDS1_SHARPNESS));

    else if(PROPERTY_CLOUDS1_WINDX == name
        ||  PROPERTY_CLOUDS1_WINDY == name)
        m_himmel->highLayer()->setWind(osg::Vec2(
            doubleValue(PROPERTY_CLOUDS1_WINDX), doubleValue(PROPERTY_CLOUDS1_WINDY)));
    }


    if(m_himmel->dubeLayer())
    {
         if(PROPERTY_CLOUDS2_ALTITUDE == name)
        m_himmel->dubeLayer()->setAltitude(doubleValue(PROPERTY_CLOUDS2_ALTITUDE));
    else if(PROPERTY_CLOUDS2_CHANGE == name)
        m_himmel->dubeLayer()->setChange(doubleValue(PROPERTY_CLOUDS2_CHANGE));
    else if(PROPERTY_CLOUDS2_SCALE == name)
        m_himmel->dubeLayer()->setScale(doubleValue(PROPERTY_CLOUDS2_SCALE));
    else if(PROPERTY_CLOUDS2_COVERAGE == name)
        m_himmel->dubeLayer()->setCoverage(doubleValue(PROPERTY_CLOUDS2_COVERAGE));
    else if(PROPERTY_CLOUDS2_SHARPNESS == name)
        m_himmel->dubeLayer()->setSharpness(doubleValue(PROPERTY_CLOUDS2_SHARPNESS));

    else if(PROPERTY_CLOUDS2_WINDX == name
        ||  PROPERTY_CLOUDS2_WINDY == name)
        m_himmel->dubeLayer()->setWind(osg::Vec2(
            doubleValue(PROPERTY_CLOUDS2_WINDX), doubleValue(PROPERTY_CLOUDS2_WINDY)));
    }
}


const double Scene_ProceduralHimmel::setLatitude(const double latitude)
{
    return m_himmel->setLatitude(latitude);
}


const double Scene_ProceduralHimmel::setLongitude(const double longitude)
{
    return m_himmel->setLongitude(longitude);
}


const double Scene_ProceduralHimmel::setAltitude(const double altitude)
{
    return m_himmel->setAltitude(altitude);
}