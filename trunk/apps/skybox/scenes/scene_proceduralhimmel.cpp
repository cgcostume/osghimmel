
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

#include "include/himmel.h"
#include "include/atmospheregeode.h"
#include "include/moongeode.h"
#include "include/starsgeode.h"
#include "include/milkywaygeode.h"
#include "include/abstractastronomy.h"

#include "utils/qt2osg.h"


#include <osg/Light>
#include <osg/LightSource>


namespace
{
    // Properties

    static const QString GROUP_PROCEDURAL                  (TR("Procedrual"));

    static const QString GROUP_PROCEDURAL_ATMOSPHERE       (TR("Atmosphere"));
    static const QString PROPERTY_ATM_SUNSCALE             (TR("Sun Scale"));
    static const QString PROPERTY_ATM_EXPOSURE             (TR("Exposure"));
    static const QString PROPERTY_ATM_BLUEHOUR_COLOR       (TR("Blue Hour Color"));
    static const QString PROPERTY_ATM_BLUEHOUR_INTENSITY   (TR("Blue Hour Intensity"));
    static const QString PROPERTY_ATM_AVGGR                (TR("Avg Ground Reflectance"));
    static const QString PROPERTY_ATM_HR                   (TR("Rayleigh Height Scale"));
    static const QString PROPERTY_ATM_BETARR               (TR("Rayleigh Coefficient R"));
    static const QString PROPERTY_ATM_BETARG               (TR("Rayleigh Coefficient G"));
    static const QString PROPERTY_ATM_BETARB               (TR("Rayleigh Coefficient B"));
    static const QString PROPERTY_ATM_HM                   (TR("Mie Height Scale"));
    static const QString PROPERTY_ATM_BETAM                (TR("Mie Coefficient"));
    static const QString PROPERTY_ATM_MIEG                 (TR("Mie G"));

    static const QString GROUP_PROCEDURAL_MOON             (TR("Moon"));
    static const QString PROPERTY_MOON_SCALE               (TR("Moon Scale"));
    static const QString PROPERTY_MOON_SUNSHINE_COLOR      (TR("Sun Shine Color"));
    static const QString PROPERTY_MOON_SUNSHINE_INTENSITY  (TR("Sun Shine Intensity"));
    static const QString PROPERTY_MOON_EARTHSHINE_COLOR    (TR("Earth Shine Color"));
    static const QString PROPERTY_MOON_EARTHSHINE_INTENSITY(TR("Earth Shine Intensity"));
    static const QString PROPERTY_MOON_POSITION_OVERRIDE   (TR("Moon Position Override"));
    static const QString PROPERTY_MOON_POSITION_X          (TR("Moon Position X"));
    static const QString PROPERTY_MOON_POSITION_Y          (TR("Moon Position Y"));
    static const QString PROPERTY_MOON_POSITION_Z          (TR("Moon Position Z"));

    static const QString GROUP_PROCEDURAL_STARS            (TR("Stars"));

    static const QString PROPERTY_STARS_COLOR              (TR("Color (Stars)"));
    static const QString PROPERTY_STARS_COLOR_RATIO        (TR("Color Ratio (Stars)"));
    static const QString PROPERTY_STARS_GLARE_INTENSITY    (TR("Glare Intensity"));
    static const QString PROPERTY_STARS_GLARE_SCALE        (TR("Glare Scale"));
    static const QString PROPERTY_STARS_APPARENT_MAG       (TR("Apprant Magnitude (Stars)"));
    static const QString PROPERTY_STARS_SCATTERING         (TR("Scattering (Stars)"));
    static const QString PROPERTY_STARS_SCINTILLATION      (TR("Scintillation"));

    static const QString GROUP_PROCEDURAL_MILKYWAY         (TR("MilkyWay"));

    static const QString PROPERTY_MILKYWAY_COLOR           (TR("Color (MilkyWay)"));
    static const QString PROPERTY_MILKYWAY_COLOR_RATIO     (TR("Color Ratio (MilkyWay)"));
    static const QString PROPERTY_MILKYWAY_APPARENT_MAG    (TR("Apparent Magnitude (MilkyWay)"));
    static const QString PROPERTY_MILKYWAY_SCATTERING      (TR("Scattering (MilkyWay)"));
}

// TEMP

#include <osg/NodeCallback>
#include <osgUtil/CullVisitor>

#include <math.h>
float f;

class ns : public osg::NodeCallback
{
    public:
    
        ns(osg::Light *light, Himmel *himmel)
        :   m_ligth(light)
        ,   m_himmel(himmel)
        {
            f = 0.f;
        }
       
        virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
        {
            traverse(node, nv);
            
            osg::Vec3 p = m_himmel->getSunPosition();

            float t1 = 1.0 / sqrt(1.0 + pow(-p.z() + 1.1, 32)) + 0.05;
            float t2 = 1.0 / sqrt(1.0 + pow(-p.z() + 1.2, 16));

            p *= -10000;
            m_ligth->setPosition(osg::Vec4(p.x(), p.y(), p.z(), 1.0));


            m_ligth->setDiffuse(osg::Vec4(1.0 * t2, 0.95 * t2, 0.9 * t2, 1.0) + osg::Vec4(1.0 * t1, 0.00 * t1, 0.0 * t1, 1.0));


            f += 0.01f;
        }

protected:
    osg::Light *m_ligth;
    Himmel* m_himmel;
};


Scene_ProceduralHimmel::Scene_ProceduralHimmel(osg::Camera *camera)
:   AbstractHimmelScene(camera)
,   m_himmel(NULL)
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

    m_himmel->addUpdateCallback(new ns(light, m_himmel));
}


Scene_ProceduralHimmel::~Scene_ProceduralHimmel()
{
}


AbstractHimmel *Scene_ProceduralHimmel::himmel()
{
    return m_himmel;
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
    createProperty(*moonGroup, PROPERTY_MOON_POSITION_OVERRIDE, false); 
    createProperty(*moonGroup, PROPERTY_MOON_POSITION_X, 0.0, -1.0, 1.0, 0.01); 
    createProperty(*moonGroup, PROPERTY_MOON_POSITION_Y, 0.0, -1.0, 1.0, 0.01); 
    createProperty(*moonGroup, PROPERTY_MOON_POSITION_Z, 0.0, -1.0, 1.0, 0.01);   

    QtProperty *starsGroup = createGroup(GROUP_PROCEDURAL_STARS);

    createProperty(*starsGroup, PROPERTY_STARS_COLOR, toQColor(StarsGeode::defaultColor())); 
    createProperty(*starsGroup, PROPERTY_STARS_COLOR_RATIO, StarsGeode::defaultColorRatio(), 0.0, 1.0, 0.1); 
    createProperty(*starsGroup, PROPERTY_STARS_GLARE_INTENSITY, 1.0, 0.0, 100.0, 0.1); 
    createProperty(*starsGroup, PROPERTY_STARS_GLARE_SCALE, StarsGeode::defaultGlareScale(), 0.0, 100.0, 0.1); 
    createProperty(*starsGroup, PROPERTY_STARS_SCATTERING, StarsGeode::defaultScattering(), 0.0, 100.0, 0.1); 
    createProperty(*starsGroup, PROPERTY_STARS_SCINTILLATION, StarsGeode::defaultScintillation(), 0.0, 100.0, 0.1); 
    createProperty(*starsGroup, PROPERTY_STARS_APPARENT_MAG, StarsGeode::defaultApparentMagnitude(), -32.0, 32.0, 0.1);


    QtProperty *milkywayGroup = createGroup(GROUP_PROCEDURAL_MILKYWAY);

    createProperty(*milkywayGroup, PROPERTY_MILKYWAY_COLOR, toQColor(MilkyWayGeode::defaultColor())); 
    createProperty(*milkywayGroup, PROPERTY_MILKYWAY_COLOR_RATIO, MilkyWayGeode::defaultColorRatio(), 0.0, 10.0, 0.25); 
    createProperty(*milkywayGroup, PROPERTY_MILKYWAY_APPARENT_MAG, MilkyWayGeode::defaultApparentMagnitude(), -32.0, 32.0, 0.1);
    createProperty(*milkywayGroup, PROPERTY_MILKYWAY_SCATTERING, MilkyWayGeode::defaultScattering(), 0.0, 100.0, 0.1);
}


void Scene_ProceduralHimmel::propertyChanged(
    QtProperty *p
,   const QString &name)
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
        m_himmel->atmosphere()->setScatteringRayleigh(osg::Vec3(doubleValue(PROPERTY_ATM_BETARR), doubleValue(PROPERTY_ATM_BETARG), doubleValue(PROPERTY_ATM_BETARB)) * 1e-3); 
    else if(PROPERTY_ATM_HM == name)
        m_himmel->atmosphere()->setThicknessMie(doubleValue(PROPERTY_ATM_HM)); 
    else if(PROPERTY_ATM_BETAM == name)
        m_himmel->atmosphere()->setScatteringMie(doubleValue(PROPERTY_ATM_BETAM) * 1e-3); 
    else if(PROPERTY_ATM_MIEG == name)
        m_himmel->atmosphere()->setPhaseG(doubleValue(PROPERTY_ATM_MIEG)); 


    else if(PROPERTY_MOON_SCALE == name)
        m_himmel->moon()->setScale(doubleValue(PROPERTY_MOON_SCALE));
    else if(PROPERTY_MOON_SUNSHINE_COLOR == name)
        m_himmel->moon()->setSunShineColor(toVec3(colorValue(PROPERTY_MOON_SUNSHINE_COLOR)));
    else if(PROPERTY_MOON_SUNSHINE_INTENSITY == name)
        m_himmel->moon()->setSunShineIntensity(doubleValue(PROPERTY_MOON_SUNSHINE_INTENSITY));
    else if(PROPERTY_MOON_EARTHSHINE_COLOR == name)
        m_himmel->moon()->setEarthShineColor(toVec3(colorValue(PROPERTY_MOON_EARTHSHINE_COLOR)));
    else if(PROPERTY_MOON_EARTHSHINE_INTENSITY == name)
        m_himmel->moon()->setEarthShineIntensity(doubleValue(PROPERTY_MOON_EARTHSHINE_INTENSITY));
    else if(PROPERTY_MOON_POSITION_OVERRIDE == name)
        m_himmel->astro()->setOverrideMoonPosition(boolValue(PROPERTY_MOON_POSITION_OVERRIDE));
    else if(PROPERTY_MOON_POSITION_X == name || PROPERTY_MOON_POSITION_Y == name || PROPERTY_MOON_POSITION_Z == name)
    {
        osg::Vec3 vec;
        vec._v[0] =  doubleValue(PROPERTY_MOON_POSITION_X);
        vec._v[1] =  doubleValue(PROPERTY_MOON_POSITION_Y);
        vec._v[2] =  doubleValue(PROPERTY_MOON_POSITION_Z);
        vec.normalize();
        m_himmel->astro()->setMoonPosition(vec);
    }


    else if(PROPERTY_STARS_COLOR == name)
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


    else if(PROPERTY_MILKYWAY_COLOR == name)
        m_himmel->milkyway()->setColor(toVec3(colorValue(PROPERTY_MILKYWAY_COLOR)));
    else if(PROPERTY_MILKYWAY_COLOR_RATIO == name)
        m_himmel->milkyway()->setColorRatio(doubleValue(PROPERTY_MILKYWAY_COLOR_RATIO));
    else if(PROPERTY_MILKYWAY_APPARENT_MAG == name)
        m_himmel->milkyway()->setApparentMagnitude(doubleValue(PROPERTY_MILKYWAY_APPARENT_MAG));
    else if(PROPERTY_MILKYWAY_SCATTERING == name)
        m_himmel->milkyway()->setScattering(doubleValue(PROPERTY_MILKYWAY_SCATTERING));
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