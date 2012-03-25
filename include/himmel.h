
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

#pragma once
#ifndef __PROCEDURALHIMMEL_H__
#define __PROCEDURALHIMMEL_H__

#include "declspec.h"
#include "abstracthimmel.h"
#include "atime.h"

#ifdef OSGHIMMEL_EXPORTS
#include "abstractastronomy.h"
#include "atmospheregeode.h"
#include "moongeode.h"
#include "starsgeode.h"
#include "milkywaygeode.h"
#include "cloudlayerhighgeode.h"
#else // #ifdef OSGHIMMEL_EXPORTS
class AbstractAstronomy;
class AtmosphereGeode;
class MoonGeode;
class StarsGeode;
class MilkyWayGeode;
class CloudLayerHighGeode;
#endif // #ifdef OSGHIMMEL_EXPORTS


class OSGH_API Himmel : public AbstractHimmel
{
public:
    static Himmel *create();

public:

    Himmel(
        MilkyWayGeode *milkyWay
    ,   MoonGeode *moon
    ,   StarsGeode *stars
    ,   AtmosphereGeode *atmosphere
    ,   CloudLayerHighGeode *highLayer
    ,   AbstractAstronomy *astronomy);

    virtual ~Himmel();

    static osg::Uniform *cmnUniform();


    const float setLatitude(const float latitude);
    const float getLatitude() const;

    const float setLongitude(const float longitude);
    const float getLongitude() const;

    const float setAltitude(const float altitude);
    const float getAltitude() const;
    static const float defaultAltitude();


    inline AtmosphereGeode *atmosphere() const 
    {
        return m_atmosphere;
    }

    inline MoonGeode *moon() const
    {
        return m_moon;
    }

    inline StarsGeode *stars() const
    {
        return m_stars;
    }

    inline MilkyWayGeode *milkyway() const
    {
        return m_milkyway;
    }


    inline AbstractAstronomy *astro() const
    {
        return m_astronomy;
    }


    const osg::Vec3 getSunPosition() const;
    const osg::Vec3 getSunPosition(const t_aTime &aTime) const;

protected:

    virtual void update();
    void updateSeed();

    // Workaround avoiding culling by osgs' automated near far 
    // retrieval. This fixes culling of stars and moon when no 
    // further scene geometry is available.
    // Six black quads will be drawn. This adds a black 
    // background that is used if no milkyway is given. This is
    // required since other geodes are rendered with blending 
    // enabled.
    void addAntiCull();


#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
    void registerShader() const;
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER

protected:

    osg::ref_ptr<osg::Uniform> u_sun;
    osg::ref_ptr<osg::Uniform> u_time;
    // height above mean sea level in km, mean earth radius in km
    osg::ref_ptr<osg::Uniform> u_common; 

    AbstractAstronomy *m_astronomy;

    osg::ref_ptr<AtmosphereGeode>     m_atmosphere;
    osg::ref_ptr<MoonGeode>           m_moon;
    osg::ref_ptr<StarsGeode>          m_stars;
    osg::ref_ptr<MilkyWayGeode>       m_milkyway;
    osg::ref_ptr<CloudLayerHighGeode> m_highLayer;
};

#endif // __PROCEDURALHIMMEL_H__