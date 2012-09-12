
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

#include "mathmacros.h"
#include "earth.h"
#include "timef.h"
#include "astronomy.h"
#include "himmelquad.h"
#include "atmospheregeode.h"
#include "moongeode.h"
#include "moonglaregeode.h"
#include "starsgeode.h"
#include "starmapgeode.h"
#include "highcloudlayergeode.h"
#include "dubecloudlayergeode.h"

#include <osg/ShapeDrawable>

#include <stdlib.h>
#include <assert.h>


namespace osgHimmel
{

Himmel *Himmel::createWithoutClouds()
{
    // cubeMapFilePaths should contain a questionmark '?' that is replaced
    // by cubemap extensions '_px', '_nx', '_py', etc. 
    // e.g. "resources/starmap?.png" points to "resources/milkyway_px.png" etc.

    return new Himmel(
        new StarMapGeode("resources/skymap?.png")
    ,   new MoonGeode("resources/moon?.png")
    ,   new StarsGeode("resources/brightstars")
    ,   new AtmosphereGeode()
    ,   NULL//new HighCloudLayerGeode()
    ,   NULL//new DubeCloudLayerGeode()
    ,   new Astronomy()
    );
}


Himmel *Himmel::createWithClouds()
{
    // cubeMapFilePaths should contain a questionmark '?' that is replaced
    // by cubemap extensions '_px', '_nx', '_py', etc. 
    // e.g. "resources/starmap?.png" points to "resources/milkyway_px.png" etc.

    return new Himmel(
        new StarMapGeode("resources/skymap?.png")
    ,   new MoonGeode("resources/moon?.png")
    ,   new StarsGeode("resources/brightstars")
    ,   new AtmosphereGeode()
    ,   new HighCloudLayerGeode()
    ,   new DubeCloudLayerGeode()
    ,   new Astronomy()
    );
}


Himmel::Himmel(
    StarMapGeode *milkyWay
,   MoonGeode *moon
,   StarsGeode *stars
,   AtmosphereGeode *atmosphere
,   HighCloudLayerGeode *highLayer
,   DubeCloudLayerGeode *dubeLayer
,   AbstractAstronomy *astronomy)
:   AbstractHimmel()
,   m_starmap(milkyWay)
,   m_moon(moon)
,   m_moonGlare(NULL)
,   m_stars(stars)
,   m_atmosphere(atmosphere)
,   m_highLayer(highLayer)
,   m_dubeLayer(dubeLayer)
,   m_astronomy(astronomy)

,   u_sun(NULL)
,   u_sunr(NULL)
,   u_time(NULL)
,   u_common(NULL)
{
    assert(m_astronomy);

    u_sun = new osg::Uniform("sun", osg::Vec3(0.0, 0.0, 0.0));
    getOrCreateStateSet()->addUniform(u_sun);
    u_sunr = new osg::Uniform("sunr", osg::Vec3(0.0, 0.0, 0.0));
    getOrCreateStateSet()->addUniform(u_sunr);


    u_time = new osg::Uniform("time", 0.f);
    getOrCreateStateSet()->addUniform(u_time);

    // 0: altitude in km
    // 1: apparent angular radius (not diameter!)
    // 2: radius up to "end of atm"
    // 3: seed (for randomness of stuff)
    u_common = cmnUniform();
    getOrCreateStateSet()->addUniform(u_common);


    int bin = 0;
    static const std::string binName("RenderBin");

    addAntiCull()->getOrCreateStateSet()->setRenderBinDetails(bin++, binName);

    if(m_starmap)
    {
        addChild(m_starmap);
        m_starmap->getOrCreateStateSet()->setRenderBinDetails(bin++, binName);
    }
    if(m_stars)
    {
        addChild(m_stars);
        m_stars->getOrCreateStateSet()->setRenderBinDetails(bin++, binName);
    }
    if(m_moon)
    {
        addChild(m_moon);
        m_moon->getOrCreateStateSet()->setRenderBinDetails(bin++, binName);

        //m_moonGlare = new MoonGlareGeode;
        //addChild(m_moonGlare);
        //m_moonGlare->getOrCreateStateSet()->setRenderBinDetails(bin++, binName);

        //m_moon->addUniformsToVariousStateSate(m_moonGlare->getOrCreateStateSet());
    }
    if(m_atmosphere)
    {
        addChild(m_atmosphere);
        m_atmosphere->getOrCreateStateSet()->setRenderBinDetails(bin++, binName);
    }



    if(m_highLayer)
    {
        addChild(m_highLayer);
        m_highLayer->getOrCreateStateSet()->setRenderBinDetails(bin++, binName);
    }

    if(m_dubeLayer)
    {
        addChild(m_dubeLayer);
        m_dubeLayer->getOrCreateStateSet()->setRenderBinDetails(bin++, binName);
    }
};


osg::Geode *Himmel::addAntiCull()
{
    // Add a unit cube to this geode, to avoid culling of hquads, stars, 
    // moon, etc. caused by automatic near far retrieval of osg. This geode 
    // should be added first, since all other nodes are drawn with blending.

    osg::ref_ptr<osg::Geode> antiCull = new osg::Geode();
    addChild(antiCull);

    // 2 * 2 ^ 0.5 -> should fit an rotating cube with radius 1
    osg::Box *cube = new osg::Box(osg::Vec3(), 2.8284f); 

    osg::ShapeDrawable *cubeDrawable = new osg::ShapeDrawable(cube);
    cubeDrawable->setColor(osg::Vec4(0.f, 0.f, 0.f, 1.f));

    antiCull->addDrawable(cubeDrawable);

    return antiCull;
}


osg::Uniform *Himmel::cmnUniform()
{
    return new osg::Uniform("cmn", osg::Vec4(defaultAltitude()
        , Earth::meanRadius(), Earth::meanRadius() + Earth::atmosphereThicknessNonUniform(), 0));
}


Himmel::~Himmel()
{
    delete m_astronomy;
};


void Himmel::update()
{
    AbstractHimmel::update();

    updateSeed();

    if(isDirty())
    {
        const t_aTime atime = t_aTime::fromTimeF(*getTime());
        astro()->update(atime);

        osg::Vec3 sunv = astro()->getSunPosition(false);
        u_sun->set(sunv);

        osg::Vec3 sunrv = astro()->getSunPosition(true);
        u_sunr->set(sunrv);

        u_time->set(static_cast<float>(getTime()->getf()));

        if(m_starmap)
            m_starmap->update(*this);
        if(m_moon)
        {
            m_moon->update(*this);
            //m_moonGlare->update(*this);
        }
        if(m_stars)
            m_stars->update(*this);
        if(m_atmosphere)
            m_atmosphere->update(*this);
        if(m_highLayer)
            m_highLayer->update(*this);

        if(m_dubeLayer)
            m_dubeLayer->update(*this);

        dirty(false);
    }
}


void Himmel::updateSeed()
{
    osg::Vec4 temp; 
    u_common->get(temp);

    temp[3] = rand();
    u_common->set(temp);
}


const osg::Vec3 Himmel::getSunPosition() const
{
    osg::Vec3 sunv;
    u_sun->get(sunv);

    return sunv;
}


const osg::Vec3 Himmel::getSunPosition(const t_aTime &aTime) const
{
    return astro()->getSunPosition(aTime, m_astronomy->getLatitude(), m_astronomy->getLongitude(), false);
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


const float Himmel::setAltitude(const float altitude)
{
    osg::Vec4 temp; 
    u_common->get(temp);

    // Clamp altitude into non uniform atmosphere. (min alt is 1m)
    temp[0] = _clamp(0.001f, Earth::atmosphereThicknessNonUniform(), altitude);
    u_common->set(temp);

    return getAltitude();
}

const float Himmel::getAltitude() const
{
    osg::Vec4 temp; 
    u_common->get(temp);

    return temp[0];
}

const float Himmel::defaultAltitude()
{
    return 0.2f;
}

} // namespace osgHimmel