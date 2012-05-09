
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
#ifndef __ASTRONOMY2_H__
#define __ASTRONOMY2_H__

#include "declspec.h"
#include "abstractastronomy.h"


namespace osgHimmel
{

class OSGH_API Astronomy2 : public AbstractAstronomy
{
public:

    Astronomy2();

protected:

    virtual const osg::Vec3 moonPosition(
        const t_aTime &aTime
    ,   const float latitude
    ,   const float longitude
    ,   const bool refractionCorrected) const;

    virtual const osg::Vec3 sunPosition(
        const t_aTime &aTime
    ,   const float latitude
    ,   const float longitude
    ,   const bool refractionCorrected) const;

    virtual const osg::Matrix moonOrientation(
        const t_aTime &aTime
    ,   const float latitude
    ,   const float longitude) const;

    virtual const float earthShineIntensity(
        const t_aTime &aTime
    ,   const float latitude
    ,   const float longitude) const;

    virtual const osg::Matrix equToHorTransform(
        const t_aTime &aTime
    ,   const float latitude
    ,   const float longitude) const;

    virtual const float sunDistance(const t_julianDay t) const;
    virtual const float angularSunRadius(const t_julianDay t) const;

    virtual const float moonRadius() const;

    virtual const float moonDistance(const t_julianDay t) const;
    virtual const float angularMoonRadius(const t_julianDay t) const;
};

} // namespace osgHimmel

#endif // __ASTRONOMY2_H__