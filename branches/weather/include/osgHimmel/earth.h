
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
#ifndef __EARTH_H__
#define __EARTH_H__

#include "declspec.h"
#include "julianday.h"
#include "typedefs.h"


namespace osgHimmel
{

class OSGH_API Earth
{
public:

    static const t_longf orbitEccentricity(const t_julianDay t);

    static const t_longf apparentAngularSunDiameter(const t_julianDay t);
    static const t_longf apparentAngularMoonDiameter(const t_julianDay t);

    static const t_longf longitudeNutation(const t_julianDay t);
    static const t_longf obliquityNutation(const t_julianDay t);

    static const t_longf meanObliquity(const t_julianDay t);
    static const t_longf trueObliquity(const t_julianDay t);

    static const t_longf atmosphericRefraction(const t_longf altitude);

    static const t_longf viewDistanceWithinAtmosphere(
        const t_longf y /* height component of the view direction on ground into the sky */
    ,   const bool refractionCorrected = false);

    static const t_longf meanRadius();
    static const t_longf atmosphereThickness(); // if its density were uniform...
    static const t_longf atmosphereThicknessNonUniform();

    static const t_longf apparentMagnitudeLimit();
};

} // namespace osgHimmel

#endif // __EARTH_H__