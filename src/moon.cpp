
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
#include "siderealtime.h"
#include "mathmacros.h"

#include <assert.h>


namespace osgHimmel
{

// Mean longitude, referred to the mean equinox of the date (AA.45.1).

const t_longf Moon::meanLongitude(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const t_longf L0 = 218.3164591 
        + T * (+ 481267.88134236
        + T * (-      0.0013268
        + T * (+ 1.0 / 528841.0
        + T * (- 1.0 / 65194000.0))));

    return _revd(L0);
}


// Mean elongation (AA.45.2).

const t_longf Moon::meanElongation(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const t_longf D = 297.8502042 
        + T * (+ 445267.1115168
        + T * (-      0.0016300
        + T * (+ 1.0 / 545868.0
        + T * (- 1.0 / 113065000.0))));

    return _revd(D);
}


// Mean anomaly (AA.45.4).

const t_longf Moon::meanAnomaly(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const t_longf M = 134.9634114 
        + T * (+ 477198.8676313
        + T * (+      0.0089970
        + T * (+ 1.0 / 69699.0
        + T * (- 1.0 / 14712000.0))));

    return _revd(M);
}


// Mean distance of the Moon from its ascending node (AA.45.5)

const t_longf Moon::meanLatitude(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const t_longf F = 93.2720993 
        + T * (+ 483202.0175273
        + T * (-      0.0034029
        + T * (- 1.0 / 3526000.0
        + T * (+ 1.0 / 863310000.0))));

    return _revd(F);
}


const t_longf Moon::meanOrbitLongitude(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const t_longf O = 125.04452 
        + T * (- 1934.136261
        + T * (+    0.0020708
        + T * (+ 1.0 / 450000.0)));

    return _revd(O);
}


const t_ecld Moon::position(const t_julianDay t)
{
    const t_longf sM = _rad(Sun::meanAnomaly(t));

    const t_longf mL = _rad(meanLongitude(t));
    const t_longf mM = _rad(meanAnomaly(t));
    const t_longf mD = _rad(meanElongation(t));
    const t_longf mF = _rad(meanLatitude(t));

    const t_julianDay T(jCenturiesSinceSE(t));

    const t_longf A1 = _rad(_revd(119.75 +    131.849 * T));
    const t_longf A2 = _rad(_revd( 53.09 + 479264.290 * T));
    const t_longf A3 = _rad(_revd(313.45 + 481266.484 * T));

    //const t_longf E = earth_orbitEccentricity(t);
    // -> does not apply here - the eccentricity of the earths' orbit 
    // in 45.6 is about 60. times the earth_orbitEccentricity...?

    // Correction for eccentricity of the Earth's orbit around the sun.

    // (AA.45.6)
    const t_longf E = 1.0  
        + T * (- 0.002516 
        + T * (- 0.0000074));

    const t_longf EE = E * E;

    // (AA.45.A)

    t_longf Sl = 0.0;

    Sl += 6288.774 * sin(                 + 1 * mM         );
    Sl += 1274.027 * sin( 2 * mD          - 1 * mM         );
    Sl +=  658.314 * sin( 2 * mD                           );
    Sl +=  213.618 * sin(                 + 2 * mM         );
    Sl -=  185.116 * sin(        + 1 * sM                  ) * E;
    Sl -=  114.332 * sin(                          + 2 * mF);
    Sl +=   58.793 * sin( 2 * mD          - 2 * mM         );
    Sl +=   57.066 * sin( 2 * mD - 1 * sM - 1 * mM         ) * E;
    Sl +=   53.322 * sin( 2 * mD          + 1 * mM         );
    Sl +=   45.758 * sin( 2 * mD - 1 * sM                  ) * E;
    Sl -=   40.923 * sin(        + 1 * sM - 1 * mM         ) * E;
    Sl -=   34.720 * sin( 1 * mD                           );
    Sl -=   30.383 * sin(        + 1 * sM + 1 * mM         ) * E;
    Sl +=   15.327 * sin( 2 * mD                   - 2 * mF);
    Sl -=   12.528 * sin(                 + 1 * mM + 2 * mF);
    Sl +=   10.980 * sin(                 + 1 * mM - 2 * mF);
    Sl +=   10.675 * sin( 4 * mD          - 1 * mM         );
    Sl +=   10.034 * sin(                 + 3 * mM         );
    Sl +=    8.548 * sin( 4 * mD          - 2 * mM         );
    Sl -=    7.888 * sin( 2 * mD + 1 * sM - 1 * mM         ) * E;
    Sl -=    6.766 * sin( 2 * mD + 1 * sM                  ) * E;
    Sl -=    5.163 * sin( 1 * mD          - 1 * mM         );
    Sl +=    4.987 * sin( 1 * mD + 1 * sM                  ) * E;
    Sl +=    4.036 * sin( 2 * mD - 1 * sM + 1 * mM         ) * E;
    Sl +=    3.994 * sin( 2 * mD          + 2 * mM         );
    Sl +=    3.861 * sin( 4 * mD                           );
    Sl +=    3.665 * sin( 2 * mD          - 3 * mM         );
    Sl -=    2.689 * sin(        + 1 * sM - 2 * mM         ) * E;
    Sl -=    2.602 * sin( 2 * mD          - 1 * mM + 2 * mF);
    Sl +=    2.390 * sin( 2 * mD - 1 * sM - 2 * mM         ) * E;
    Sl -=    2.348 * sin( 1 * mD          + 1 * mM         );
    Sl +=    2.236 * sin( 2 * mD - 2 * sM                  ) * EE;
    Sl -=    2.120 * sin(        + 1 * sM + 2 * mM         ) * E;
    Sl -=    2.069 * sin(        + 2 * sM                  ) * EE;
    Sl +=    2.048 * sin( 2 * mD - 2 * sM - 1 * mM         ) * EE;
    Sl -=    1.773 * sin( 2 * mD          + 1 * mM - 2 * mF);
    Sl -=    1.595 * sin( 2 * mD                   + 2 * mF);
    Sl +=    1.215 * sin( 4 * mD - 1 * sM - 1 * mM         ) * E;
    Sl -=    1.110 * sin(                 + 2 * mM + 2 * mF);
    Sl -=    0.892 * sin( 3 * mD          - 1 * mM         );
    Sl -=    0.810 * sin( 2 * mD + 1 * sM + 1 * mM         ) * E;
    Sl +=    0.759 * sin( 4 * mD - 1 * sM - 2 * mM         ) * E;
    Sl -=    0.713 * sin(        + 2 * sM - 1 * mM         ) * EE;
    Sl -=    0.700 * sin( 2 * mD + 2 * sM - 1 * mM         ) * EE;
    Sl +=    0.691 * sin( 2 * mD + 1 * sM - 2 * mM         );
    Sl +=    0.596 * sin( 2 * mD - 1 * sM          - 2 * mF) * E;
    Sl +=    0.549 * sin( 4 * mD          + 1 * mM         );
    Sl +=    0.537 * sin(                 + 4 * mM         );
    Sl +=    0.520 * sin( 4 * mD - 1 * sM                  ) * E;
    Sl -=    0.487 * sin( 1 * mD          - 2 * mM         );
    Sl -=    0.399 * sin( 2 * mD + 1 * sM          - 2 * mF) * E;
    Sl -=    0.381 * sin(                 + 2 * mM - 2 * mF);
    Sl +=    0.351 * sin( 1 * mD + 1 * sM + 1 * mM         ) * E;
    Sl -=    0.340 * sin( 3 * mD          - 2 * mM         );
    Sl +=    0.330 * sin( 4 * mD          - 3 * mM         );
    Sl +=    0.327 * sin( 2 * mD - 1 * sM + 2 * mM         ) * E;
    Sl -=    0.323 * sin(        + 2 * sM + 1 * mM         ) * EE;
    Sl +=    0.299 * sin( 1 * mD + 1 * sM - 1 * mM         ) * E;
    Sl +=    0.294 * sin( 2 * mD          + 3 * mM         );

    // (AA.45.B)

    t_longf Sb = 0.0;

    Sb += 5128.122 * sin(                          + 1 * mF);
    Sb +=  280.602 * sin(                 + 1 * mM + 1 * mF);
    Sb +=  277.693 * sin(                 + 1 * mM - 1 * mF);
    Sb +=  173.237 * sin( 2 * mD                   - 1 * mF);
    Sb +=   55.413 * sin( 2 * mD          - 1 * mM + 1 * mF);
    Sb +=   46.271 * sin( 2 * mD          - 1 * mM - 1 * mF);
    Sb +=   32.573 * sin( 2 * mD                   + 1 * mF);
    Sb +=   17.198 * sin(                 + 2 * mM + 1 * mF);
    Sb +=    9.266 * sin( 2 * mD          + 1 * mM - 1 * mF);
    Sb +=    8.822 * sin(                 + 2 * mM - 1 * mF);
    Sb +=    8.216 * sin( 2 * mD - 1 * sM          - 1 * mF) * E;
    Sb +=    4.324 * sin( 2 * mD          - 2 * mM - 1 * mF);
    Sb +=    4.200 * sin( 2 * mD          + 1 * mM + 1 * mF);
    Sb -=    3.359 * sin( 2 * mD + 1 * sM          - 1 * mF) * E;
    Sb +=    2.463 * sin( 2 * mD - 1 * sM - 1 * mM + 1 * mF) * E;
    Sb +=    2.211 * sin( 2 * mD - 1 * sM          + 1 * mF) * E;
    Sb +=    2.065 * sin( 2 * mD - 1 * sM - 1 * mM - 1 * mF) * E;
    Sb -=    1.870 * sin(        + 1 * sM - 1 * mM - 1 * mF) * E;
    Sb +=    1.828 * sin( 4 * mD          - 1 * mM - 1 * mF);
    Sb -=    1.794 * sin(        + 1 * sM          + 1 * mF) * E;
    Sb -=    1.749 * sin(                          + 3 * mF);
    Sb -=    1.565 * sin(        + 1 * sM - 1 * mM + 1 * mF) * E;
    Sb -=    1.491 * sin( 1 * mD                   + 1 * mF);
    Sb -=    1.475 * sin(        + 1 * sM + 1 * mM + 1 * mF) * E;
    Sb -=    1.410 * sin(        + 1 * sM + 1 * mM - 1 * mF) * E;
    Sb -=    1.344 * sin(        + 1 * sM          - 1 * mF) * E;
    Sb -=    1.335 * sin( 1 * mD                   - 1 * mF);
    Sb +=    1.107 * sin(                 + 3 * mM + 1 * mF);
    Sb +=    1.024 * sin( 4 * mD                   - 1 * mF);
    Sb +=    0.833 * sin( 4 * mD          - 1 * mM + 1 * mF);
    Sb +=    0.777 * sin(                 + 1 * mM - 3 * mF);
    Sb +=    0.671 * sin( 4 * mD          - 2 * mM + 1 * mF);
    Sb +=    0.607 * sin( 2 * mD                   - 3 * mF);
    Sb +=    0.596 * sin( 2 * mD          + 2 * mM - 1 * mF);
    Sb +=    0.491 * sin( 2 * mD - 1 * sM + 1 * mM - 1 * mF) * E;
    Sb -=    0.451 * sin( 2 * mD          - 2 * mM + 1 * mF);
    Sb +=    0.439 * sin(                 + 3 * mM - 1 * mF);
    Sb +=    0.422 * sin( 2 * mD          + 2 * mM + 1 * mF);
    Sb +=    0.421 * sin( 2 * mD          - 3 * mM - 1 * mF);
    Sb -=    0.366 * sin( 2 * mD + 1 * sM - 1 * mM + 1 * mF) * E;
    Sb -=    0.351 * sin( 2 * mD + 1 * sM          + 1 * mF) * E;
    Sb +=    0.331 * sin( 4 * mD                   + 1 * mF);
    Sb +=    0.315 * sin( 2 * mD - 1 * sM + 1 * mM + 1 * mF) * E;
    Sb +=    0.302 * sin( 2 * mD - 2 * sM          - 1 * mF) * EE;
    Sb -=    0.283 * sin(                 + 1 * mM + 3 * mF);
    Sb -=    0.229 * sin( 2 * mD + 1 * sM + 1 * mM - 1 * mF) * E;
    Sb +=    0.223 * sin( 1 * mD + 1 * sM          - 1 * mF) * E;
    Sb +=    0.223 * sin( 1 * mD + 1 * sM          + 1 * mF) * E;
    Sb -=    0.220 * sin(        + 1 * sM - 2 * mM - 1 * mF) * E;
    Sb -=    0.220 * sin( 2 * mD + 1 * sM - 1 * mM - 1 * mF) * E;
    Sb -=    0.185 * sin( 1 * mD          + 1 * mM + 1 * mF);
    Sb +=    0.181 * sin( 2 * mD - 1 * sM - 2 * mM - 1 * mF) * E;
    Sb -=    0.177 * sin(        + 1 * sM + 2 * mM + 1 * mF) * E;
    Sb +=    0.176 * sin( 4 * mD          - 2 * mM - 1 * mF);
    Sb +=    0.166 * sin( 4 * mD - 1 * sM - 1 * mM - 1 * mF) * E;
    Sb -=    0.164 * sin( 1 * mD          + 1 * mM - 1 * mF);
    Sb +=    0.132 * sin( 4 * mD          + 1 * mM - 1 * mF);
    Sb -=    0.119 * sin( 1 * mD          - 1 * mM - 1 * mF);
    Sb +=    0.115 * sin( 4 * mD - 1 * sM          - 1 * mF) * E;
    Sb +=    0.107 * sin( 2 * mD - 2 * sM          + 1 * mF) * EE;

    // Add corrective Terms

    Sl +=  3.958 * sin(A1)
         + 1.962 * sin(mL - mF)
         + 0.318 * sin(A2);

    Sb += -2.235 * sin(mL) 
         + 0.382 * sin(A3)
         + 0.175 * sin(A1 - mF)
         + 0.175 * sin(A1 + mF)
         + 0.127 * sin(mL - mM)
         - 0.115 * sin(mL + mM);

    t_ecld ecl;

    ecl.longitude = meanLongitude(t) + Sl * 0.001 + Earth::longitudeNutation(t);
    ecl.latitude = Sb * 0.001;

    return ecl;
}


const t_equd Moon::apparentPosition(const t_julianDay t)
{
    t_ecld ecl = position(t);
    ecl.longitude += Earth::longitudeNutation(t);

    return ecl.toEquatorial(Earth::meanObliquity(t));
}


const t_hord Moon::horizontalPosition(
    const t_aTime &aTime
,   const t_longf latitude
,   const t_longf longitude)
{
    t_julianDay t(jd(aTime));
    t_julianDay s(siderealTime(aTime));

    t_equd equ = apparentPosition(t);

    return equ.toHorizontal(s, latitude, longitude);
}


// NOTE: This gives the distance from the center of the moon to the
// center of the earth. 

const t_longf Moon::distance(const t_julianDay t)
{
    const t_longf sM = _rad(Sun::meanAnomaly(t));

    const t_longf mM = _rad(meanAnomaly(t));
    const t_longf mD = _rad(meanElongation(t));
    const t_longf mF = _rad(meanLatitude(t));

    const t_julianDay T(jCenturiesSinceSE(t));

    // Correction for eccentricity of the Earth's orbit around the sun.

    // (AA.45.6)

    const t_longf E = 1.0  
        + T * (- 0.002516 
        + T * (- 0.0000074));

    const t_longf EE = E * E;

    // (AA.45.A)

    t_longf Sr = 0.0;

    Sr -= 20905.355 * cos(                 + 1 * mM         );
    Sr -=  3699.111 * cos( 2 * mD          - 1 * mM         );
    Sr -=  2955.968 * cos( 2 * mD                           );
    Sr -=   569.925 * cos(                 + 2 * mM         );
    Sr +=    48.888 * cos(        + 1 * sM                  ) * E;
    Sr -=     3.149 * cos(                          + 2 * mF);
    Sr +=   246.158 * cos( 2 * mD          - 2 * mM         );
    Sr -=   152.138 * cos( 2 * mD - 1 * sM - 1 * mM         ) * E;
    Sr -=   170.733 * cos( 2 * mD          + 1 * mM         );
    Sr -=   204.586 * cos( 2 * mD - 1 * sM                  ) * E;
    Sr -=   129.620 * cos(        + 1 * sM - 1 * mM         ) * E;
    Sr +=   108.743 * cos( 1 * mD                           );
    Sr +=   104.755 * cos(        + 1 * sM + 1 * mM         ) * E;
    Sr +=    10.321 * cos( 2 * mD                   - 2 * mF);
    Sr +=    79.661 * cos(                 + 1 * mM - 2 * mF);
    Sr -=    34.782 * cos( 4 * mD          - 1 * mM         );
    Sr -=    23.210 * cos(                 + 3 * mM         );
    Sr -=    21.636 * cos( 4 * mD          - 2 * mM         );
    Sr +=    24.208 * cos( 2 * mD + 1 * sM - 1 * mM         ) * E;
    Sr +=    30.824 * cos( 2 * mD + 1 * sM                  ) * E;
    Sr -=     8.379 * cos( 1 * mD          - 1 * mM         );
    Sr -=    16.675 * cos( 1 * mD + 1 * sM                  ) * E;
    Sr -=    12.831 * cos( 2 * mD - 1 * sM + 1 * mM         ) * E;
    Sr -=    10.445 * cos( 2 * mD          + 2 * mM         );
    Sr -=    11.650 * cos( 4 * mD                           );
    Sr +=    14.403 * cos( 2 * mD          - 3 * mM         );
    Sr -=     7.003 * cos(        + 1 * sM - 2 * mM         ) * E;
    Sr +=    10.056 * cos( 2 * mD - 1 * sM - 2 * mM         ) * E;
    Sr +=     6.322 * cos( 1 * mD          + 1 * mM         );
    Sr -=     9.884 * cos( 2 * mD - 2 * sM                  ) * EE;
    Sr +=     5.751 * cos(        + 1 * sM + 2 * mM         ) * E;
    Sr -=     4.950 * cos( 2 * mD - 2 * sM - 1 * mM         ) * EE;
    Sr +=     4.130 * cos( 2 * mD          + 1 * mM - 2 * mF);
    Sr -=     3.958 * cos( 4 * mD - 1 * sM - 1 * mM         ) * E;
    Sr +=     3.258 * cos( 3 * mD          - 1 * mM         );
    Sr +=     2.616 * cos( 2 * mD + 1 * sM + 1 * mM         ) * E;
    Sr -=     1.897 * cos( 4 * mD - 1 * sM - 2 * mM         ) * E;
    Sr -=     2.117 * cos(        + 2 * sM - 1 * mM         ) * EE;
    Sr +=     2.354 * cos( 2 * mD + 2 * sM - 1 * mM         ) * EE;
    Sr -=     1.423 * cos( 4 * mD          + 1 * mM         );
    Sr -=     1.117 * cos(                 + 4 * mM         );
    Sr -=     1.571 * cos( 4 * mD - 1 * sM                  ) * E;
    Sr -=     1.739 * cos( 1 * mD          - 2 * mM         );
    Sr -=     4.421 * cos(                 + 2 * mM - 2 * mF);
    Sr +=     1.165 * cos(        + 2 * sM + 1 * mM         ) * EE;
    Sr +=     8.752 * cos( 2 * mD          - 1 * mM - 2 * mF);

    const t_longf D = 385000.56 + Sr; // in kilometers

    return D;
}


void Moon::opticalLibrations(
    const t_julianDay t
,   t_longf &l /* librations in longitude */
,   t_longf &b /* librations in latitude  */)
{
    // (AA.51.1)

    const t_longf Dr = _rad(Earth::longitudeNutation(t));

    const t_longf F  = _rad(meanLatitude(t));
    const t_longf O  = _rad(meanOrbitLongitude(t));

    const t_ecld ecl = position(t);
    const t_longf lo = _rad(ecl.longitude);
    const t_longf la = _rad(ecl.latitude);

    static const t_longf I = _rad(1.54242);

    const t_longf cos_la = cos(la);
    const t_longf sin_la = sin(la);
    const t_longf cos_I  = cos(I);
    const t_longf sin_I  = sin(I);

    const t_longf W  = _rev(lo - Dr - O);
    const t_longf sin_W  = sin(W);

    const t_longf A  = _rev(atan2(sin_W * cos_la * cos_I - sin_la * sin_I, cos(W) * cos_la));

    l = _deg(A - F);
    b = _deg(asin(-sin_W * cos_la * sin_I - sin_la * cos_I));
}


const t_longf Moon::parallacticAngle(
    const t_aTime &aTime
,   const t_longf latitude
,   const t_longf longitude)
{
    // (AA.13.1)

    const t_julianDay t(jd(aTime));

    const t_longf la = _rad(latitude);
    const t_longf lo = _rad(longitude);

    const t_equd pos = apparentPosition(t);
    const t_longf ra = _rad(pos.right_ascension);
    const t_longf de = _rad(pos.declination);
     
    const t_longf s  = _rad(siderealTime(aTime));

    // (AA.p88) - local hour angle

    const t_longf H = s + lo - ra;

    const t_longf cos_la = cos(la);
    const t_longf P = atan2(sin(H) * cos_la, sin(la) * cos(de) - sin(de) * cos_la * cos(H));

    return _deg(P);
}


const t_longf Moon::positionAngleOfAxis(const t_julianDay t)
{
    // (AA.p344)

    const t_equd pos = apparentPosition(t);

    const t_longf a  = _rad(pos.right_ascension);
    const t_longf e  = _rad(Earth::meanObliquity(t));

    const t_longf Dr = _rad(Earth::longitudeNutation(t));
    const t_longf O  = _rad(meanOrbitLongitude(t));

    const t_longf V  = O + Dr;

    static const t_longf I = _rad(1.54242);
    const t_longf sin_I  = sin(I);

    const t_longf X  = sin_I * sin(V);
    const t_longf Y  = sin_I * cos(V) * cos(e) - cos(I) * sin(e);

    // optical libration in latitude

    const t_ecld ecl = position(t);

    const t_longf lo = _rad(ecl.longitude);
    const t_longf la = _rad(ecl.latitude);

    const t_longf W  = _rev(lo - Dr - O);
    const t_longf b = asin(-sin(W) * cos(la) * sin_I - sin(la) * cos(I));

    // final angle

    const t_longf w  = _rev(atan2(X, Y));
    const t_longf P = asin(sqrt(X * X + Y * Y) * cos(a - w) / cos(b));

    return _deg(P);
}


const t_longf Moon::meanRadius()
{
    // http://nssdc.gsfc.nasa.gov/planetary/factsheet/moonfact.html

    static const t_longf r = 1737.1; // in kilometers

    return r; 
}

} // namespace osgHimmel