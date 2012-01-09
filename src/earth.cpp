
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


// NOTE: This gives the distance from the center of the sun to the
// center of the earth.

const long double earth_sunDistance(const t_julianDay t)
{
    const long double e = earth_orbitEccentricity(t);

    // (AA.24.5)
    const long double R = 1.000001018 * (1.0 - e * e) /
        (1.0 + e * cos(_rad(sun_trueAnomaly(t))));  // in AU

    return _kms(R);
}


const long double earth_apparentAngularSunDiameter(const t_julianDay t)
{
    return _adiameter(earth_sunDistance(t), sun_meanRadius());
}


// NOTE: This gives the distance from the center of the moon to the
// center of the earth. 

const long double earth_moonDistance(const t_julianDay t)
{
    long double Σr = 0.0;

    const long double sM = _rad(sun_meanAnomaly(t));

    const long double mL = _rad(moon_meanLongitude(t));
    const long double mM = _rad(moon_meanAnomaly(t));
    const long double mD = _rad(moon_meanElongation(t));
    const long double mF = _rad(moon_meanLatitude(t));

    const t_julianDay T(jCenturiesSinceSE(t));

    // Correction for eccentricity of the Earth's orbit around the sun.

    // (AA.45.6)

    const long double E = 1.0  
        + T * (- 0.002516 
        + T * (- 0.0000074));

    const long double EE = E * E;

    // (AA.45.A)

    Σr -= 20905.355 * cos(                 + 1 * mM         );
    Σr -=  3699.111 * cos( 2 * mD          - 1 * mM         );
    Σr -=  2955.968 * cos( 2 * mD                           );
    Σr -=   569.925 * cos(                 + 2 * mM         );
    Σr +=    48.888 * cos(        + 1 * sM                  ) * E;
    Σr -=     3.149 * cos(                          + 2 * mF);
    Σr +=   246.158 * cos( 2 * mD          - 2 * mM         );
    Σr -=   152.138 * cos( 2 * mD - 1 * sM - 1 * mM         ) * E;
    Σr -=   170.733 * cos( 2 * mD          + 1 * mM         );
    Σr -=   204.586 * cos( 2 * mD - 1 * sM                  ) * E;
    Σr -=   129.620 * cos(        + 1 * sM - 1 * mM         ) * E;
    Σr +=   108.743 * cos( 1 * mD                           );
    Σr +=   104.755 * cos(        + 1 * sM + 1 * mM         ) * E;
    Σr +=    10.321 * cos( 2 * mD                   - 2 * mF);
    Σr +=    79.661 * cos(                 + 1 * mM - 2 * mF);
    Σr -=    34.782 * cos( 4 * mD          - 1 * mM         );
    Σr -=    23.210 * cos(                 + 3 * mM         );
    Σr -=    21.636 * cos( 4 * mD          - 2 * mM         );
    Σr +=    24.208 * cos( 2 * mD + 1 * sM - 1 * mM         ) * E;
    Σr +=    30.824 * cos( 2 * mD + 1 * sM                  ) * E;
    Σr -=     8.379 * cos( 1 * mD          - 1 * mM         );
    Σr -=    16.675 * cos( 1 * mD + 1 * sM                  ) * E;
    Σr -=    12.831 * cos( 2 * mD - 1 * sM + 1 * mM         ) * E;
    Σr -=    10.445 * cos( 2 * mD          + 2 * mM         );
    Σr -=    11.650 * cos( 4 * mD                           );
    Σr +=    14.403 * cos( 2 * mD          - 3 * mM         );
    Σr -=     7.003 * cos(        + 1 * sM - 2 * mM         ) * E;
    Σr +=    10.056 * cos( 2 * mD - 1 * sM - 2 * mM         ) * E;
    Σr +=     6.322 * cos( 1 * mD          + 1 * mM         );
    Σr -=     9.884 * cos( 2 * mD - 2 * sM                  ) * EE;
    Σr +=     5.751 * cos(        + 1 * sM + 2 * mM         ) * E;
    Σr -=     4.950 * cos( 2 * mD - 2 * sM - 1 * mM         ) * EE;
    Σr +=     4.130 * cos( 2 * mD          + 1 * mM - 2 * mF);
    Σr -=     3.958 * cos( 4 * mD - 1 * sM - 1 * mM         ) * E;
    Σr +=     3.258 * cos( 3 * mD          - 1 * mM         );
    Σr +=     2.616 * cos( 2 * mD + 1 * sM + 1 * mM         ) * E;
    Σr -=     1.897 * cos( 4 * mD - 1 * sM - 2 * mM         ) * E;
    Σr -=     2.117 * cos(        + 2 * sM - 1 * mM         ) * EE;
    Σr +=     2.354 * cos( 2 * mD + 2 * sM - 1 * mM         ) * EE;
    Σr -=     1.423 * cos( 4 * mD          + 1 * mM         );
    Σr -=     1.117 * cos(                 + 4 * mM         );
    Σr -=     1.571 * cos( 4 * mD - 1 * sM                  ) * E;
    Σr -=     1.739 * cos( 1 * mD          - 2 * mM         );
    Σr -=     4.421 * cos(                 + 2 * mM - 2 * mF);
    Σr +=     1.165 * cos(        + 2 * sM + 1 * mM         ) * EE;
    Σr +=     8.752 * cos( 2 * mD          - 1 * mM - 2 * mF);

    const long double Δ = 385000.56 + Σr; // in kilometers
    return Δ;
}

const long double earth_apparentAngularMoonDiameter(const t_julianDay t)
{
    return _adiameter(earth_moonDistance(t), moon_meanRadius());
}


// (AA.21)

const long double earth_longitudeNutation(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const long double Ω  = _rad(moon_meanOrbitLongitude(t));
    const long double Ls = _rad(sun_meanAnomaly(t));
    const long double Lm = _rad(moon_meanAnomaly(t));

#pragma NOTE("Fix mixing of high-precision with low precision formulars (use high-precision only).");
    // TODO: Mixing high-precision with low-precision (below) formulars here.
    // Perhaps adapt nightsky papers model anytime in the future.

    const long double ψ =
        - _decimal(0, 0, 17.20) * sin(Ω)
        - _decimal(0, 0,  1.32) * sin(2.0 * Ls)
        - _decimal(0, 0,  0.23) * sin(2.0 * Lm)
        + _decimal(0, 0,  0.21) * sin(2.0 * Ω );

    return ψ;
}


// (AA.21)

const long double earth_obliquityNutation(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const long double Ω  = _rad(moon_meanOrbitLongitude(t));
    const long double Ls = _rad(sun_meanAnomaly(t));
    const long double Lm = _rad(moon_meanAnomaly(t));

#pragma NOTE("Fix mixing of high-precision with low precision formulars (use high-precision only).");
    // TODO: Mixing high-precision with low-precision (below) formulars here.
    // Perhaps adapt nightsky papers model anytime in the future.

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


// Inclination of the Earth's axis of rotation. (AA.21.3)
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


const long double earth_meanRadius()
{
    // http://nssdc.gsfc.nasa.gov/planetary/factsheet/earthfact.html

    return  6371.0; // in kilometers
}
