
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

#include "include/proceduralhimmel.h"
#include "include/atmospheregeode.h"
#include "include/moongeode.h"
#include "include/starsgeode.h"
#include "include/milkywaygeode.h"
#include "include/abstractastronomy.h"

#include "utils/qt2osg.h"


namespace
{
    // Properties

    static const QString GROUP_PROCEDURAL(TR("Procedrual"));

    static const QString GROUP_PROCEDURAL_ATMOSPHERE(TR("Atmosphere"));
    static const QString PROPERTY_DITHERING_MULTIPLIER (TR("Dithering Multiplier"));
    
    static const QString GROUP_PROCEDURAL_MOON(TR("Moon"));
    static const QString PROPERTY_MOON_SCALE               (TR("Scale"));
    static const QString PROPERTY_MOON_SUNSHINE_COLOR      (TR("Sun Shine Color"));
    static const QString PROPERTY_MOON_SUNSHINE_INTENSITY  (TR("Sun Shine Intensity"));
    static const QString PROPERTY_MOON_EARTHSHINE_COLOR    (TR("Earth Shine Color"));
    static const QString PROPERTY_MOON_EARTHSHINE_INTENSITY(TR("Earth Shine Intensity"));
    static const QString PROPERTY_MOON_POSITION_OVERRIDE   (TR("Moon Position Override"));
    static const QString PROPERTY_MOON_POSITION_X          (TR("Moon Position X"));
    static const QString PROPERTY_MOON_POSITION_Y          (TR("Moon Position Y"));
    static const QString PROPERTY_MOON_POSITION_Z          (TR("Moon Position Z"));

    static const QString GROUP_PROCEDURAL_STARS(TR("Stars"));

    static const QString PROPERTY_STARS_COLOR          (TR("Color"));
    static const QString PROPERTY_STARS_COLOR_RATIO    (TR("Color Ratio"));
    static const QString PROPERTY_STARS_GLARE_INTENSITY(TR("Glare Intensity"));
    static const QString PROPERTY_STARS_GLARE_SCALE    (TR("Glare Scale"));
    static const QString PROPERTY_STARS_MAX_VMAG       (TR("Max Visible Magnitude"));
    static const QString PROPERTY_STARS_SCATTERING     (TR("Scattering"));
    static const QString PROPERTY_STARS_SCINTILLATION  (TR("Scintillation"));

    static const QString GROUP_PROCEDURAL_MILKYWAY(TR("Milkyway"));

    static const QString PROPERTY_MILKYWAY_COLOR      (TR("Color Overlay"));
    static const QString PROPERTY_MILKYWAY_INTENSITY  (TR("Intensity"));

}

Scene_ProceduralHimmel::Scene_ProceduralHimmel(osg::Camera *camera)
:   AbstractHimmelScene(camera)
,   m_himmel(NULL)
{
    initializeProperties();

    m_himmel = new ProceduralHimmel();
    addChild(m_himmel);
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

    createProperty(*proceduralGroup, PROPERTY_DITHERING_MULTIPLIER, AtmosphereGeode::defaultDitheringMultiplier(), 0.0, 255.0, 1.00);

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

    createProperty(*starsGroup, PROPERTY_STARS_COLOR, QColor(Qt::white)); 
    createProperty(*starsGroup, PROPERTY_STARS_COLOR_RATIO, 0.0, 0.0, 1.0, 0.1); 
    createProperty(*starsGroup, PROPERTY_STARS_GLARE_INTENSITY, 1.0, 0.0, 100.0, 0.1); 
    createProperty(*starsGroup, PROPERTY_STARS_GLARE_SCALE, 1.0, 0.0, 100.0, 0.1); 
    createProperty(*starsGroup, PROPERTY_STARS_SCATTERING, 1.0, 0.0, 10.0, 0.1); 
    createProperty(*starsGroup, PROPERTY_STARS_SCINTILLATION, 1.0, 0.0, 10.0, 0.1); 
    createProperty(*starsGroup, PROPERTY_STARS_MAX_VMAG, StarsGeode::defaultMaxVMag(), -32.0, 32.0, 0.1); 


    QtProperty *milkywayGroup = createGroup(GROUP_PROCEDURAL_MILKYWAY);

    createProperty(*milkywayGroup, PROPERTY_MILKYWAY_COLOR, toQColor(MilkywayGeode::defaultColor())); 
    createProperty(*milkywayGroup, PROPERTY_MILKYWAY_INTENSITY, MilkywayGeode::defaultIntensity(), 0.0, 10.0, 0.25); 
}


void Scene_ProceduralHimmel::propertyChanged(
    QtProperty *p
,   const QString &name)
{
         if(PROPERTY_DITHERING_MULTIPLIER == name)
        m_himmel->atmosphere()->setDitheringMultiplier(doubleValue(PROPERTY_DITHERING_MULTIPLIER));


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
    else if(PROPERTY_STARS_MAX_VMAG == name)
        m_himmel->stars()->setMaxVMag(doubleValue(PROPERTY_STARS_MAX_VMAG));


    else if(PROPERTY_MILKYWAY_COLOR == name)
        m_himmel->milkyway()->setColor(toVec3(colorValue(PROPERTY_MILKYWAY_COLOR)));
    else if(PROPERTY_MILKYWAY_INTENSITY == name)
        m_himmel->milkyway()->setIntensity(doubleValue(PROPERTY_MILKYWAY_INTENSITY));

}


const double Scene_ProceduralHimmel::setLatitude(const double latitude)
{
    return m_himmel->setLatitude(latitude);
}


const double Scene_ProceduralHimmel::setLongitude(const double longitude)
{
    return m_himmel->setLongitude(longitude);
}