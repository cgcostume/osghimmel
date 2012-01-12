
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

#include "moon.h"
#include "sun.h"
#include "earth.h"
#include "sideraltime.h"
#include "mathmacros.h"

#include <assert.h>


// Mean longitude, referred to the mean equinox of the date (AA.45.1).

const long double moon_meanLongitude(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const long double L = 218.3164591 
        + T * (+ 481267.88134236
        + T * (-      0.0013268
        + T * (+ 1.0 / 528841.0
        + T * (- 1.0 / 65194000.0))));

    return _revd(L);
}


// Mean elongation (AA.45.2).

const long double moon_meanElongation(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const long double D = 297.8502042 
        + T * (+ 445267.1115168
        + T * (-      0.0016300
        + T * (+ 1.0 / 545868.0
        + T * (- 1.0 / 113065000.0))));

    return _revd(D);
}


// Mean anomaly (AA.45.4).

const long double moon_meanAnomaly(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const long double M = 134.9634114 
        + T * (+ 477198.8676313
        + T * (+      0.0089970
        + T * (+ 1.0 / 69699.0
        + T * (- 1.0 / 14712000.0))));

    // AA.21 ...
    //const long double M = 134.96298 
    //    + T * (+ 477198.867398
    //    + T * (+      0.0086972
    //    + T * (+ 1.0 / 56250.0)));

    return _revd(M);
}


// Mean distance of the Moon from its ascending node (AA.45.5)

const long double moon_meanLatitude(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const long double F = 93.2720993 
        + T * (+ 483202.0175273
        + T * (-      0.0034029
        + T * (- 1.0 / 3526000.0
        + T * (+ 1.0 / 863310000.0))));

    return _revd(F);
}


const long double moon_meanOrbitLongitude(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const long double Ω = 125.04452 
        + T * (- 1934.136261
        + T * (+    0.0020708
        + T * (+ 1.0 / 450000.0)));

    //// From ((AA p152)
    //const long double Ω = 12504
    //    + T * (- 1934.136);

    return _revd(Ω);
}


const t_eclCoords moon_position(const t_julianDay t)
{
    long double Σl = 0.0;
    long double Σb = 0.0;

    const long double sM = _rad(sun_meanAnomaly(t));

    const long double mL = _rad(moon_meanLongitude(t));
    const long double mM = _rad(moon_meanAnomaly(t));
    const long double mD = _rad(moon_meanElongation(t));
    const long double mF = _rad(moon_meanLatitude(t));

    const t_julianDay T(jCenturiesSinceSE(t));

    const long double A1 = _rad(_revd(119.75 +    131.849 * T));
    const long double A2 = _rad(_revd( 53.09 + 479264.290 * T));
    const long double A3 = _rad(_revd(313.45 + 481266.484 * T));

    //const long double E = earth_orbitEccentricity(t);
    // -> does not apply here - the eccentricity of the earths' orbit 
    // in 45.6 is about 60. times the earth_orbitEccentricity...?

    // Correction for eccentricity of the Earth's orbit around the sun.

    // (AA.45.6)
    const long double E = 1.0  
        + T * (- 0.002516 
        + T * (- 0.0000074));

    const long double EE = E * E;

    // (AA.45.A)

    Σl += 6288.774 * sin(                 + 1 * mM         );
    Σl += 1274.027 * sin( 2 * mD          - 1 * mM         );
    Σl +=  658.314 * sin( 2 * mD                           );
    Σl +=  213.618 * sin(                 + 2 * mM         );
    Σl -=  185.116 * sin(        + 1 * sM                  ) * E;
    Σl -=  114.332 * sin(                          + 2 * mF);
    Σl +=   58.793 * sin( 2 * mD          - 2 * mM         );
    Σl +=   57.066 * sin( 2 * mD - 1 * sM - 1 * mM         ) * E;
    Σl +=   53.322 * sin( 2 * mD          + 1 * mM         );
    Σl +=   45.758 * sin( 2 * mD - 1 * sM                  ) * E;
    Σl -=   40.923 * sin(        + 1 * sM - 1 * mM         ) * E;
    Σl -=   34.720 * sin( 1 * mD                           );
    Σl -=   30.383 * sin(        + 1 * sM + 1 * mM         ) * E;
    Σl +=   15.327 * sin( 2 * mD                   - 2 * mF);
    Σl -=   12.528 * sin(                 + 1 * mM + 2 * mF);
    Σl +=   10.980 * sin(                 + 1 * mM - 2 * mF);
    Σl +=   10.675 * sin( 4 * mD          - 1 * mM         );
    Σl +=   10.034 * sin(                 + 3 * mM         );
    Σl +=    8.548 * sin( 4 * mD          - 2 * mM         );
    Σl -=    7.888 * sin( 2 * mD + 1 * sM - 1 * mM         ) * E;
    Σl -=    6.766 * sin( 2 * mD + 1 * sM                  ) * E;
    Σl -=    5.163 * sin( 1 * mD          - 1 * mM         );
    Σl +=    4.987 * sin( 1 * mD + 1 * sM                  ) * E;
    Σl +=    4.036 * sin( 2 * mD - 1 * sM + 1 * mM         ) * E;
    Σl +=    3.994 * sin( 2 * mD          + 2 * mM         );
    Σl +=    3.861 * sin( 4 * mD                           );
    Σl +=    3.665 * sin( 2 * mD          - 3 * mM         );
    Σl -=    2.689 * sin(        + 1 * sM - 2 * mM         ) * E;
    Σl -=    2.602 * sin( 2 * mD          - 1 * mM + 2 * mF);
    Σl +=    2.390 * sin( 2 * mD - 1 * sM - 2 * mM         ) * E;
    Σl -=    2.348 * sin( 1 * mD          + 1 * mM         );
    Σl +=    2.236 * sin( 2 * mD - 2 * sM                  ) * EE;
    Σl -=    2.120 * sin(        + 1 * sM + 2 * mM         ) * E;
    Σl -=    2.069 * sin(        + 2 * sM                  ) * EE;
    Σl +=    2.048 * sin( 2 * mD - 2 * sM - 1 * mM         ) * EE;
    Σl -=    1.773 * sin( 2 * mD          + 1 * mM - 2 * mF);
    Σl -=    1.595 * sin( 2 * mD                   + 2 * mF);
    Σl +=    1.215 * sin( 4 * mD - 1 * sM - 1 * mM         ) * E;
    Σl -=    1.110 * sin(                 + 2 * mM + 2 * mF);
    Σl -=    0.892 * sin( 3 * mD          - 1 * mM         );
    Σl -=    0.810 * sin( 2 * mD + 1 * sM + 1 * mM         ) * E;
    Σl +=    0.759 * sin( 4 * mD - 1 * sM - 2 * mM         ) * E;
    Σl -=    0.713 * sin(        + 2 * sM - 1 * mM         ) * EE;
    Σl -=    0.700 * sin( 2 * mD + 2 * sM - 1 * mM         ) * EE;
    Σl +=    0.691 * sin( 2 * mD + 1 * sM - 2 * mM         );
    Σl +=    0.596 * sin( 2 * mD - 1 * sM          - 2 * mF) * E;
    Σl +=    0.549 * sin( 4 * mD          + 1 * mM         );
    Σl +=    0.537 * sin(                 + 4 * mM         );
    Σl +=    0.520 * sin( 4 * mD - 1 * sM                  ) * E;
    Σl -=    0.487 * sin( 1 * mD          - 2 * mM         );
    Σl -=    0.399 * sin( 2 * mD + 1 * sM          - 2 * mF) * E;
    Σl -=    0.381 * sin(                 + 2 * mM - 2 * mF);
    Σl +=    0.351 * sin( 1 * mD + 1 * sM + 1 * mM         ) * E;
    Σl -=    0.340 * sin( 3 * mD          - 2 * mM         );
    Σl +=    0.330 * sin( 4 * mD          - 3 * mM         );
    Σl +=    0.327 * sin( 2 * mD - 1 * sM + 2 * mM         ) * E;
    Σl -=    0.323 * sin(        + 2 * sM + 1 * mM         ) * EE;
    Σl +=    0.299 * sin( 1 * mD + 1 * sM - 1 * mM         ) * E;
    Σl +=    0.294 * sin( 2 * mD          + 3 * mM         );

    // (AA.45.B)

    Σb += 5128.122 * sin(                          + 1 * mF);
    Σb +=  280.602 * sin(                 + 1 * mM + 1 * mF);
    Σb +=  277.693 * sin(                 + 1 * mM - 1 * mF);
    Σb +=  173.237 * sin( 2 * mD                   - 1 * mF);
    Σb +=   55.413 * sin( 2 * mD          - 1 * mM + 1 * mF);
    Σb +=   46.271 * sin( 2 * mD          - 1 * mM - 1 * mF);
    Σb +=   32.573 * sin( 2 * mD                   + 1 * mF);
    Σb +=   17.198 * sin(                 + 2 * mM + 1 * mF);
    Σb +=    9.266 * sin( 2 * mD          + 1 * mM - 1 * mF);
    Σb +=    8.822 * sin(                 + 2 * mM - 1 * mF);
    Σb +=    8.216 * sin( 2 * mD - 1 * sM          - 1 * mF) * E;
    Σb +=    4.324 * sin( 2 * mD          - 2 * mM - 1 * mF);
    Σb +=    4.200 * sin( 2 * mD          + 1 * mM + 1 * mF);
    Σb -=    3.359 * sin( 2 * mD + 1 * sM          - 1 * mF) * E;
    Σb +=    2.463 * sin( 2 * mD - 1 * sM - 1 * mM + 1 * mF) * E;
    Σb +=    2.211 * sin( 2 * mD - 1 * sM          + 1 * mF) * E;
    Σb +=    2.065 * sin( 2 * mD - 1 * sM - 1 * mM - 1 * mF) * E;
    Σb -=    1.870 * sin(        + 1 * sM - 1 * mM - 1 * mF) * E;
    Σb +=    1.828 * sin( 4 * mD          - 1 * mM - 1 * mF);
    Σb -=    1.794 * sin(        + 1 * sM          + 1 * mF) * E;
    Σb -=    1.749 * sin(                          + 3 * mF);
    Σb -=    1.565 * sin(        + 1 * sM - 1 * mM + 1 * mF) * E;
    Σb -=    1.491 * sin( 1 * mD                   + 1 * mF);
    Σb -=    1.475 * sin(        + 1 * sM + 1 * mM + 1 * mF) * E;
    Σb -=    1.410 * sin(        + 1 * sM + 1 * mM - 1 * mF) * E;
    Σb -=    1.344 * sin(        + 1 * sM          - 1 * mF) * E;
    Σb -=    1.335 * sin( 1 * mD                   - 1 * mF);
    Σb +=    1.107 * sin(                 + 3 * mM + 1 * mF);
    Σb +=    1.024 * sin( 4 * mD                   - 1 * mF);
    Σb +=    0.833 * sin( 4 * mD          - 1 * mM + 1 * mF);
    Σb +=    0.777 * sin(                 + 1 * mM - 3 * mF);
    Σb +=    0.671 * sin( 4 * mD          - 2 * mM + 1 * mF);
    Σb +=    0.607 * sin( 2 * mD                   - 3 * mF);
    Σb +=    0.596 * sin( 2 * mD          + 2 * mM - 1 * mF);
    Σb +=    0.491 * sin( 2 * mD - 1 * sM + 1 * mM - 1 * mF) * E;
    Σb -=    0.451 * sin( 2 * mD          - 2 * mM + 1 * mF);
    Σb +=    0.439 * sin(                 + 3 * mM - 1 * mF);
    Σb +=    0.422 * sin( 2 * mD          + 2 * mM + 1 * mF);
    Σb +=    0.421 * sin( 2 * mD          - 3 * mM - 1 * mF);
    Σb -=    0.366 * sin( 2 * mD + 1 * sM - 1 * mM + 1 * mF) * E;
    Σb -=    0.351 * sin( 2 * mD + 1 * sM          + 1 * mF) * E;
    Σb +=    0.331 * sin( 4 * mD                   + 1 * mF);
    Σb +=    0.315 * sin( 2 * mD - 1 * sM + 1 * mM + 1 * mF) * E;
    Σb +=    0.302 * sin( 2 * mD - 2 * sM          - 1 * mF) * EE;
    Σb -=    0.283 * sin(                 + 1 * mM + 3 * mF);
    Σb -=    0.229 * sin( 2 * mD + 1 * sM + 1 * mM - 1 * mF) * E;
    Σb +=    0.223 * sin( 1 * mD + 1 * sM          - 1 * mF) * E;
    Σb +=    0.223 * sin( 1 * mD + 1 * sM          + 1 * mF) * E;
    Σb -=    0.220 * sin(        + 1 * sM - 2 * mM - 1 * mF) * E;
    Σb -=    0.220 * sin( 2 * mD + 1 * sM - 1 * mM - 1 * mF) * E;
    Σb -=    0.185 * sin( 1 * mD          + 1 * mM + 1 * mF);
    Σb +=    0.181 * sin( 2 * mD - 1 * sM - 2 * mM - 1 * mF) * E;
    Σb -=    0.177 * sin(        + 1 * sM + 2 * mM + 1 * mF) * E;
    Σb +=    0.176 * sin( 4 * mD          - 2 * mM - 1 * mF);
    Σb +=    0.166 * sin( 4 * mD - 1 * sM - 1 * mM - 1 * mF) * E;
    Σb -=    0.164 * sin( 1 * mD          + 1 * mM - 1 * mF);
    Σb +=    0.132 * sin( 4 * mD          + 1 * mM - 1 * mF);
    Σb -=    0.119 * sin( 1 * mD          - 1 * mM - 1 * mF);
    Σb +=    0.115 * sin( 4 * mD - 1 * sM          - 1 * mF) * E;
    Σb +=    0.107 * sin( 2 * mD - 2 * sM          + 1 * mF) * EE;
 
    //

    Σl +=  3.958 * sin(A1)
         + 1.962 * sin(mL - mF)
         + 0.318 * sin(A2);

    Σb += -2.235 * sin(mL) 
         + 0.382 * sin(A3)
         + 0.175 * sin(A1 - mF)
         + 0.175 * sin(A1 + mF)
         + 0.127 * sin(mL - mM)
         - 0.115 * sin(mL + mM);

    t_eclCoords ecl;

    ecl.longitude = moon_meanLongitude(t) + Σl * 0.001 + earth_longitudeNutation(t);
    ecl.latitude = Σb * 0.001;

    return ecl;
}


const t_equCoords moon_apparentPosition(const t_julianDay t)
{
    t_eclCoords ecl = moon_position(t);
    ecl.longitude += earth_longitudeNutation(t);

    return ecl.toEquatorial(earth_meanObliquity(t));
}


const t_horCoords moon_horizontalPosition(
    const t_aTime &aTime
,   const long double latitude
,   const long double longitude)
{
    t_julianDay t(jd(aTime));
    t_julianDay s(siderealTime(aTime));

    t_equCoords equ = moon_apparentPosition(t);

    return equ.toHorizontal(s, latitude, longitude);
}


const long double moon_meanRadius()
{
    // http://nssdc.gsfc.nasa.gov/planetary/factsheet/moonfact.html

    return 1737.1; // in kilometers
}