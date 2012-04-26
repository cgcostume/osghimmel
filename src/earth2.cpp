
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

#include "earth2.h"

#include "earth.h"
#include "sun2.h"
#include "moon2.h"
#include "mathmacros.h"

#include <assert.h>


namespace osgHimmel
{

const float Earth2::orbitEccentricity()
{
    // http://nssdc.gsfc.nasa.gov/planetary/factsheet/earthfact.html
    const float E = 0.01671022;

    return _revd(E);
}


const float Earth2::apparentAngularSunDiameter(const t_julianDay t)
{
    return _adiameter(Sun2::distance(t), Sun2::meanRadius());
}


const float Earth2::apparentAngularMoonDiameter(const t_julianDay t)
{
    return _adiameter(Moon2::distance(t), Moon2::meanRadius());
}


const float Earth2::longitudeNutation(const t_julianDay t)
{
    const float sM = _rad(Sun2::meanAnomaly(t));
    const float mM = _rad(Moon2::meanAnomaly(t));
    const float O  = _rad(Moon2::meanOrbitLongitude(t));

    // (AA.21 p132)

    const float r =
        - _decimal(0, 0, 17.20) * sin(O)
        - _decimal(0, 0,  1.32) * sin(2.0 * sM)
        - _decimal(0, 0,  0.23) * sin(2.0 * mM)
        + _decimal(0, 0,  0.21) * sin(2.0 * O );

    return r;
}


const float Earth2::obliquityNutation(const t_julianDay t)
{
    const float O  = _rad(Moon2::meanOrbitLongitude(t));
    const float Ls = _rad(Sun2::meanAnomaly(t));
    const float Lm = _rad(Moon2::meanAnomaly(t));

    // (AA.21 p132)
    const float e =
        + _decimal(0, 0,  9.20) * cos(O)
        + _decimal(0, 0,  0.57) * cos(2.0 * Ls)
        + _decimal(0, 0,  0.10) * cos(2.0 * Lm)
        - _decimal(0, 0,  0.09) * cos(2.0 * O );

    return e;
}


const float Earth2::trueObliquity(const t_julianDay t)
{
    return meanObliquity(t) + obliquityNutation(t); // e
}


const float Earth2::meanObliquity(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    // ("A Physically-Based Night Sky Model" - 2001 - Wann Jensen et al.)
    const float e0 = _deg(0.409093 - 0.000227 * T);

    // (AA.21.2)
    //const float e0 = _decimal(23, 26, 21.448) 
    //    + T * (- _decimal(0, 0, 46.8150)
    //    + T * (- _decimal(0, 0,  0.00059)
    //    + T * (+ _decimal(0, 0,  0.001813))));

    return e0;
}


// This is not refraction corrected.

const float Earth2::viewDistanceWithinAtmosphere(
    const float y)
{
    const float t = atmosphereThickness();
    //const float r = meanRadius();

    // This works only for mean radius of earth.
    return t * 1116.0 / ((y + 0.004) * 1.1116);
}


const float Earth2::atmosphericRefraction(const float altitude)
{
    return Earth::atmosphericRefraction(altitude);
}


const float Earth2::meanRadius()
{
    return Earth::meanRadius(); 
}


const float Earth2::atmosphereThickness()
{
    return Earth::atmosphereThickness();
}


const float Earth2::atmosphereThicknessNonUniform()
{
    return Earth::atmosphereThicknessNonUniform();
}


const float Earth2::apparentMagnitudeLimit()
{
    return Earth::apparentMagnitudeLimit();
}

} // namespace osgHimmel