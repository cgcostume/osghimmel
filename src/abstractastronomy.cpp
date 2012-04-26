
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

#include "abstractastronomy.h"

#include "mathmacros.h"


namespace osgHimmel
{

AbstractAstronomy::AbstractAstronomy()
:
    m_latitude(0.f)
,   m_longitude(0.f)

,   m_overrideMoonPosition(false)
,   m_overrideSunPosition(false)
{
}


AbstractAstronomy::~AbstractAstronomy()
{
}


void AbstractAstronomy::update(const t_aTime &aTime)
{
    m_aTime = aTime;
    m_t = jd(aTime);
}


const float AbstractAstronomy::setLatitude(const float latitude)
{
    if(latitude != m_latitude)
        m_latitude = _clamp(-90, +90, latitude);

    return getLatitude();
}

const float AbstractAstronomy::getLatitude() const
{
    return m_latitude;
}


const float AbstractAstronomy::setLongitude(const float longitude)
{
    if(longitude != m_longitude)
        m_longitude = _clamp(-180, +180, longitude);

    return getLongitude();
}

const float AbstractAstronomy::getLongitude() const
{
    return m_longitude;
}


const osg::Matrix AbstractAstronomy::getMoonOrientation() const
{
    return moonOrientation(getATime(), getLatitude(), getLongitude());
}

const osg::Matrix AbstractAstronomy::getMoonOrientation(
    const t_aTime &aTime
,   const float latitude
,   const float longitude) const
{
    return moonOrientation(aTime, latitude, longitude);
}


const bool AbstractAstronomy::setOverrideMoonPosition(const bool enabled)
{
    if(enabled != m_overrideMoonPosition)
        m_overrideMoonPosition = enabled;

    return getOverrideMoonPosition();
}

const bool AbstractAstronomy::getOverrideMoonPosition() const
{
    return m_overrideMoonPosition;
}


const osg::Vec3 AbstractAstronomy::setMoonPosition(const osg::Vec3 &position)
{
    if(position != m_moonPosition)
        m_moonPosition = position;

    return m_moonPosition;
}

const osg::Vec3 AbstractAstronomy::getMoonPosition(
    const bool refractionCorrected) const
{
    if(m_overrideMoonPosition)
        return m_moonPosition;

    return moonPosition(getATime(), getLatitude(), getLongitude(), refractionCorrected);
}

const osg::Vec3 AbstractAstronomy::getMoonPosition(
    const t_aTime &aTime
,   const float latitude
,   const float longitude
,   const bool refractionCorrected) const
{
    return moonPosition(aTime, latitude, longitude, refractionCorrected);
}


const bool AbstractAstronomy::setOverrideSunPosition(const bool enabled)
{
    if(enabled != m_overrideSunPosition)
        m_overrideSunPosition = enabled;

    return getOverrideSunPosition();
}

const bool AbstractAstronomy::getOverrideSunPosition() const
{
    return m_overrideSunPosition;
}


const osg::Vec3 AbstractAstronomy::setSunPosition(const osg::Vec3 &position)
{
    if(position != m_sunPosition)
        m_sunPosition = position;

    return m_sunPosition;
}

const osg::Vec3 AbstractAstronomy::getSunPosition(
    const bool refractionCorrected) const
{
    if(m_overrideSunPosition)
        return m_sunPosition;

    return sunPosition(getATime(), getLatitude(), getLongitude(), refractionCorrected);
}

const osg::Vec3 AbstractAstronomy::getSunPosition(
    const t_aTime &aTime
,   const float latitude
,   const float longitude
,   const bool refractionCorrected) const
{
    return sunPosition(aTime, latitude, longitude, refractionCorrected);
}


const float AbstractAstronomy::getEarthShineIntensity() const
{
    return earthShineIntensity(getATime(), getLatitude(), getLongitude());
}


const float AbstractAstronomy::getEarthShineIntensity(
    const t_aTime &aTime
,   const float latitude
,   const float longitude) const
{
    return earthShineIntensity(aTime, latitude, longitude);
}


const float AbstractAstronomy::getAngularSunRadius() const
{
    return angularSunRadius(t());
}

const float AbstractAstronomy::getAngularSunRadius(const t_aTime &aTime) const
{
    return angularSunRadius(jd(aTime));
}


const float AbstractAstronomy::getAngularMoonRadius() const
{
    return angularMoonRadius(t());
}

const float AbstractAstronomy::getAngularMoonRadius(const t_aTime &aTime) const
{
    return angularMoonRadius(jd(aTime));
}

} // namespace osgHimmel