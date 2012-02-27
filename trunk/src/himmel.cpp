
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

#include "himmel.h"

#include "astronomy.h"
#include "astronomyla.h"

#include "himmelquad.h"
#include "atmospheregeode.h"
#include "moongeode.h"
#include "starsgeode.h"
#include "milkywaygeode.h"

#include <assert.h>


#include <osg/ShapeDrawable>


Himmel *Himmel::create()
{
    // cubeMapFilePaths should contain a questionmark '?' that is replaced
    // by cubemap extensions '_px', '_nx', '_py', etc. 
    // e.g. "resources/milkyway?.png" points to "resources/milkyway_px.png" etc.

    return new Himmel(
        new MilkyWayGeode("resources/milkyway?.png")
    ,   new MoonGeode("resources/moon?.png")
    ,   new StarsGeode("resources/brightstars")
    ,   new AtmosphereGeode()
    ,   new Astronomy());
}

// TODO: remove
osg::ref_ptr<osg::Uniform> useed = new osg::Uniform("seed", 0);

#include <osgUtil/RenderBin>

Himmel::Himmel(
    MilkyWayGeode *milkyWay
,   MoonGeode *moon
,   StarsGeode *stars
,   AtmosphereGeode *atmosphere
,   AbstractAstronomy *astronomy)
:   AbstractHimmel()
,   m_milkyway(milkyWay)
,   m_moon(moon)
,   m_stars(stars)
,   m_atmosphere(atmosphere)
,   m_astronomy(astronomy)

,   u_sun(NULL)
{
    assert(m_astronomy);

    setCullingActive(false);

    addAntiCull(); // Required to be added prior to milkyway.

    u_sun = new osg::Uniform("sun", osg::Vec3(0.0, 0.0, 0.0));
    getOrCreateStateSet()->addUniform(u_sun);

    getOrCreateStateSet()->addUniform(useed);

    int bin = 0;

    if(m_milkyway)
    {
        addChild(m_milkyway);
        m_milkyway->getOrCreateStateSet()->setRenderBinDetails(bin++, "RenderBin");
    }

    if(m_stars)
    {
        addChild(m_stars);
        m_stars->getOrCreateStateSet()->setRenderBinDetails(bin++, "RenderBin");
    }

    if(m_moon)
    {
        addChild(m_moon);
        m_moon->getOrCreateStateSet()->setRenderBinDetails(bin++, "RenderBin");
    }


    if(m_atmosphere)
    {
        addChild(m_atmosphere);
        m_atmosphere->getOrCreateStateSet()->setRenderBinDetails(bin++, "RenderBin");
    }

#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
    registerShader();
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER

};


Himmel::~Himmel()
{
    delete m_astronomy;
};


#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER

void Himmel::registerShader() const
{
    if(!shaderModifier())
        return;

    if(m_milkyway)
    {
        shaderModifier()->registerShader(m_milkyway->getName(), m_milkyway->vertexShader());
        shaderModifier()->registerShader(m_milkyway->getName(), m_milkyway->fragmentShader());
    }

    if(m_moon)
    {
        shaderModifier()->registerShader(m_moon->getName(), m_moon->vertexShader());
        shaderModifier()->registerShader(m_moon->getName(), m_moon->fragmentShader());
    }

    if(m_stars)
    {
        shaderModifier()->registerShader(m_stars->getName(), m_stars->vertexShader());
        shaderModifier()->registerShader(m_stars->getName(), m_stars->geometryShader());
        shaderModifier()->registerShader(m_stars->getName(), m_stars->fragmentShader());
    }

    if(m_atmosphere)
    {
        shaderModifier()->registerShader(m_atmosphere->getName(), m_atmosphere->vertexShader());
        shaderModifier()->registerShader(m_atmosphere->getName(), m_atmosphere->fragmentShader());
    }
}

#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER

void Himmel::update()
{
    AbstractHimmel::update();

    useed->set(rand());

    if(isDirty())
    {
        astro()->update(t_aTime::fromTimeF(*getTime()));

        osg::Vec3 sunv = astro()->getSunPosition();
        u_sun->set(sunv);

        if(m_milkyway)
            m_milkyway->update(*this);
        if(m_moon)
            m_moon->update(*this);
        if(m_stars)
            m_stars->update(*this);
        if(m_atmosphere)
            m_atmosphere->update(*this);

        dirty(false);
    }
    else
    {
        if(m_stars)
            m_stars->updateSeed();
    }
}


const osg::Vec3 Himmel::getSunPosition() const
{
    osg::Vec3 sunv;
    u_sun->get(sunv);

    return sunv;
}


const osg::Vec3 Himmel::getSunPosition(const t_aTime &aTime) const
{
    return astro()->getSunPosition(aTime, m_astronomy->getLatitude(), m_astronomy->getLongitude());
}


void Himmel::addAntiCull()
{
    // Add a unit cube to this geode, to avoid culling of stars, moon, etc. 
    // caused by automatic near far retrieval of osg. This geode should be 
    // added prior to the atmosphere node, since all other nodes are drawn
    // with blending enabled, and would make the cubes' points visible.

    osg::ref_ptr<osg::Geode> antiCull = new osg::Geode();
    addChild(antiCull);

    osg::Box *cube = new osg::Box(osg::Vec3(), 2.f);

    osg::ShapeDrawable *cubeDrawable = new osg::ShapeDrawable(cube);
    cubeDrawable->setColor(osg::Vec4(0.f, 0.f, 0.f, 1.f));

    antiCull->addDrawable(cubeDrawable);
}


const float Himmel::setLatitude(const float latitude)
{
    assert(m_astronomy);
    
    dirty();
    return m_astronomy->setLatitude(latitude);
}

const float Himmel::getLatitude() const
{
    assert(m_astronomy);
    return m_astronomy->getLatitude();
}


const float Himmel::setLongitude(const float longitude)
{
    assert(m_astronomy);

    dirty();
    return m_astronomy->setLongitude(longitude);
}

const float Himmel::getLongitude() const
{
    assert(m_astronomy);
    return m_astronomy->getLongitude();
}
