
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
#ifndef __ABSTRACTASTRONOMY_H__
#define __ABSTRACTASTRONOMY_H__

#include "declspec.h"
#include "atime.h"
#include "julianday.h"

#include <osg/Vec3>
#include <osg/Matrix>


namespace osgHimmel
{

class OSGH_API AbstractAstronomy
{
public:

    AbstractAstronomy();
    virtual ~AbstractAstronomy();


    void update(const t_aTime &aTime);

    inline const t_aTime &getATime() const
    {
        return m_aTime;
    }


    const float setLatitude(const float latitude);
    const float getLatitude() const;

    const float setLongitude(const float longitude);
    const float getLongitude() const;


    const osg::Matrixf getMoonOrientation() const;
    const osg::Matrixf getMoonOrientation(
        const t_aTime &aTime
    ,   const float latitude
    ,   const float longitude) const;

    const osg::Vec3f getMoonPosition(const bool refractionCorrected) const;
    const osg::Vec3f getMoonPosition(
        const t_aTime &aTime
    ,   const float latitude
    ,   const float longitude
    ,   const bool refractionCorrected) const;

    const osg::Vec3f getSunPosition(const bool refractionCorrected) const;
    const osg::Vec3f getSunPosition(
        const t_aTime &aTime
    ,   const float latitude
    ,   const float longitude
    ,   const bool refractionCorrected) const;

    const float getEarthShineIntensity() const;
    const float getEarthShineIntensity(
        const t_aTime &aTime
    ,   const float latitude
    ,   const float longitude) const;

    const float getSunDistance() const;
    const float getSunDistance(const t_aTime &aTime) const;

    const float getAngularSunRadius() const;
    const float getAngularSunRadius(const t_aTime &aTime) const;

    const float getMoonDistance() const;
    const float getMoonDistance(const t_aTime &aTime) const;

    const float getMoonRadius() const;

    const float getAngularMoonRadius() const;
    const float getAngularMoonRadius(const t_aTime &aTime) const;

    const osg::Matrixf getEquToHorTransform() const;
    const osg::Matrixf getEquToHorTransform(
        const t_aTime &aTime
    ,   const float latitude
    ,   const float longitude) const;


protected:

    virtual const osg::Vec3f moonPosition(
        const t_aTime &aTime
    ,   const float latitude
    ,   const float longitude
    ,   const bool refractionCorrected) const = 0;

    virtual const osg::Vec3f sunPosition(
        const t_aTime &aTime
    ,   const float latitude
    ,   const float longitude
    ,   const bool refractionCorrected) const = 0;

    virtual const osg::Matrixf moonOrientation(
        const t_aTime &aTime
    ,   const float latitude
    ,   const float longitude) const = 0;

    virtual const float earthShineIntensity(
        const t_aTime &aTime
    ,   const float latitude
    ,   const float longitude) const = 0;

    virtual const float sunDistance(const t_julianDay t) const = 0;
    virtual const float angularSunRadius(const t_julianDay t) const = 0;

    virtual const float moonRadius() const = 0;
    virtual const float moonDistance(const t_julianDay t) const = 0;
    virtual const float angularMoonRadius(const t_julianDay t) const = 0;

    virtual const osg::Matrixf equToHorTransform(
        const t_aTime &aTime
    ,   const float latitude
    ,   const float longitude) const = 0;

    inline const t_julianDay t() const 
    {
        return m_t;
    }

protected:

    t_aTime m_aTime;
    t_julianDay m_t;

    float m_latitude;
    float m_longitude;
};

} // namespace osgHimmel

#endif // __ABSTRACTASTRONOMY_H__