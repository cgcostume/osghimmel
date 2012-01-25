
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

#include "julianday.h"

const long double earth_orbitEccentricity(const t_julianDay t);

const long double earth_apparentAngularSunDiameter(const t_julianDay t);
const long double earth_apparentAngularMoonDiameter(const t_julianDay t);

const long double earth_longitudeNutation(const t_julianDay t);
const long double earth_obliquityNutation(const t_julianDay t);

const long double earth_meanObliquity(const t_julianDay t);
const long double earth_trueObliquity(const t_julianDay t);

const long double earth_atmosphericRefraction(const long double altitude);

const long double earth_viewDistanceWithinAtmosphere(
    const osg::Vec3 &eye /* view direction on ground into the sky */);
const long double earth_viewDistanceWithinAtmosphere(
    const osg::Vec3 &eye
,   const long double t  /* atmosphere thickness with uniform density in km */
,   const long double r  /* meanRadius in km */);

const long double earth_meanRadius();
const long double earth_atmosphereThickness(); // if its density where uniform...

// Low Accuracy

const float earth_orbitEccentricity_la(const t_julianDay t);

const float earth_apparentAngularSunDiameter_la(const t_julianDay t);
const float earth_apparentAngularMoonDiameter_la(const t_julianDay t);

const float earth_longitudeNutation_la(const t_julianDay t);
const float earth_obliquityNutation_la(const t_julianDay t);

const float earth_meanObliquity_la(const t_julianDay t);
const float earth_trueObliquity_la(const t_julianDay t);

const float earth_viewDistanceWithinAtmosphere_la(
    const osg::Vec3 &eye /* view direction on ground into the sky */);
const float earth_viewDistanceWithinAtmosphere_la(
    const osg::Vec3 &eye
,   const long double t  /* atmosphere thickness with uniform density in km */);

#endif // __EARTH_H__