
// Copyright (c) 2011, Daniel Müller <dm@g4t3.de>
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

#include "earth.h"
#include "sun.h"
#include "moon.h"

#include "mathmacros.h"

#include <assert.h>

// The linear eccentricity of the earth orbit is about 2.5 * 10^6 km.
// Compared to the avg. distance of 149.6 * 10^6 km this is not much.
// http://www.greier-greiner.at/hc/ekliptik.htm

// P. Bretagnon, "Théorie du mouvement de l'ensamble des planètes. Solution VSOP82", 1982

const long double earth_orbitEccentricity(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const long double E = 0.01670862
        + T * (- 0.000042037
        + T * (- 0.0000001236
        + T * (+ 0.00000000004)));

    return _revd(E);
}


const long double earth_sunDistance(const t_julianDay t)
{
    const long double e = earth_orbitEccentricity(t);

    const long double R = 1.000001018 * (1.0 - e * e) /
        (1.0 + e * cos(_rad(sun_trueAnomaly(t))));

    return R;
}


const long double earth_longitudeNutation(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const long double Ω  = _rad(moon_meanOrbitLongitude(t));
    const long double Ls = _rad(280.4664  +  36000.7698   * T);
    const long double Lm = _rad(218.3165  + 481267.8813   * T);

    const long double ψ =
        - _decimal(0, 0, 17.20) * sin(Ω)
        - _decimal(0, 0,  1.32) * sin(2.0 * Ls)
        - _decimal(0, 0,  0.23) * sin(2.0 * Lm)
        + _decimal(0, 0,  0.21) * sin(2.0 * Ω );

    return ψ;
}


const long double earth_obliquityNutation(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const long double Ω  = _rad(moon_meanOrbitLongitude(t));
    const long double Ls = _rad(280.4664  +  36000.7698   * T);
    const long double Lm = _rad(218.3165  + 481267.8813   * T);

    const long double ε =
        + _decimal(0, 0,  9.20) * cos(Ω)
        + _decimal(0, 0,  0.57) * cos(2.0 * Ls)
        + _decimal(0, 0,  0.10) * cos(2.0 * Lm)
        - _decimal(0, 0,  0.09) * cos(2.0 * Ω );

    return ε;
}


const long double earth_trueObliquity(const t_julianDay t)
{
    return earth_meanObliquity(t) + earth_obliquityNutation(t); // ε
}


// Inclination of the Earth's axis of rotation.
// By J. Laskar, "Astronomy and Astrophysics" 1986

const long double earth_meanObliquity(const t_julianDay t)
{
/*    const t_julianDay T(jCenturiesSinceSE(t));

    const long double ε0 = _decimal(23, 26, 21.448) 
        + T * (- _decimal(0, 0, 46.8150)
        + T * (- _decimal(0, 0,  0.00059)
        + T * (+ _decimal(0, 0,  0.001813))));
*/

    const t_julianDay U = jCenturiesSinceSE(t) * 0.01;

    assert(_abs(U) < 1.0);

    const long double ε0 = _decimal(23, 26, 21.448) 
        + U * (- _decimal(0, 0, 4680.93)
        + U * (- _decimal(0, 0,    1.55)
        + U * (+ _decimal(0, 0, 1999.25)
        + U * (- _decimal(0, 0,   51.38)
        + U * (- _decimal(0, 0,  249.67)
        + U * (- _decimal(0, 0,   39.05)
        + U * (+ _decimal(0, 0,    7.12)
        + U * (+ _decimal(0, 0,   27.87)
        + U * (+ _decimal(0, 0,    5.79)
        + U * (+ _decimal(0, 0,    2.45)))))))))));

    return ε0;
}


// Effect of refraction for true altitudes (AA.15.4).
// G.G. Bennet, "The Calculation of the Astronomical Refraction in marine Navigation", 1982

// This should not be used for procedural himmel, since the refraction,
// which depends on the lights' wave-length, is already taken int account 
// in the calculations there.

const long double earth_atmosphericRefraction(const long double altitude)
{
    long double R = 1.02 / 
        tan(_rad(altitude + 10.3 / (altitude + 5.11))) + 0.0019279;

    return _decimal(0, R, 0); // (since R is in minutes)
}