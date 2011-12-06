
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
    //const long double Ω = 125.04
    //    + T * (- 1934.136);

    return _revd(Ω);
}


const long double moon_distance(const t_julianDay t)
{
    long double Σr = 0.0;

    const long double sM = _rad(sun_meanAnomaly(t));

    const long double mL = _rad(moon_meanLongitude(t));
    const long double mM = _rad(moon_meanAnomaly(t));
    const long double mD = _rad(moon_meanElongation(t));
    const long double mF = _rad(moon_meanLatitude(t));

    const t_julianDay T(jCenturiesSinceSE(t));

    const long double A1 = _rad(_revd(119.75 +    131.849 * T));
    const long double A2 = _rad(_revd( 53.09 + 479264.290 * T));
    const long double A3 = _rad(_revd(313.45 + 481266.484 * T));

    // (AA.45.6)
    const long double E = 1.0  
        + T * (- 0.002516 
        + T * (- 0.0000074));

    const long double sM1 = sM * E;
    const long double sM2 = sM * E * E;

    // (AA.45.A)

    Σr -=20905355 * cos(                  + 1 * mM         );
    Σr -= 3699111 * cos( 2 * mD           - 1 * mM         );
    Σr -= 2955968 * cos( 2 * mD                            );
    Σr -=  569925 * cos(                  + 2 * mM         );
    Σr +=   48888 * cos(        + 1 * sM1                  );
    Σr -=    3149 * cos(                           + 2 * mF);
    Σr +=  246158 * cos( 2 * mD           - 2 * mM         );
    Σr -=  152138 * cos( 2 * mD - 1 * sM1 - 1 * mM         );
    Σr -=  170733 * cos( 2 * mD           + 1 * mM         );
    Σr -=  204586 * cos( 2 * mD - 1 * sM1                  );
    Σr -=  129620 * cos(        + 1 * sM1 - 1 * mM         );
    Σr +=  108743 * cos( 1 * mD                            );
    Σr +=  104755 * cos(        + 1 * sM1 + 1 * mM         );
    Σr +=   10321 * cos( 2 * mD                    - 2 * mF);
    Σr +=   79661 * cos(                  + 1 * mM - 2 * mF);
    Σr -=   34782 * cos( 4 * mD           - 1 * mM         );
    Σr -=   23210 * cos(                  + 3 * mM         );
    Σr -=   21636 * cos( 4 * mD           - 2 * mM         );
    Σr +=   24208 * cos( 2 * mD + 1 * sM1 - 1 * mM         );
    Σr +=   30824 * cos( 2 * mD + 1 * sM1                  );
    Σr -=    8379 * cos( 1 * mD           - 1 * mM         );
    Σr -=   16675 * cos( 1 * mD + 1 * sM1                  );
    Σr -=   12831 * cos( 2 * mD - 1 * sM1 + 1 * mM         );
    Σr -=   10445 * cos( 2 * mD           + 2 * mM         );
    Σr -=   11650 * cos( 4 * mD                            );
    Σr +=   14403 * cos( 2 * mD           - 3 * mM         );
    Σr -=    7003 * cos(        + 1 * sM1 - 2 * mM         );
    Σr +=   10056 * cos( 2 * mD - 1 * sM1 - 2 * mM         );
    Σr +=    6322 * cos( 1 * mD           + 1 * mM         );
    Σr -=    9884 * cos( 2 * mD - 2 * sM2                  );
    Σr +=    5751 * cos(        + 1 * sM1 + 2 * mM         );
    Σr -=    4950 * cos( 2 * mD - 2 * sM2 - 1 * mM         );
    Σr +=    4130 * cos( 2 * mD           + 1 * mM - 2 * mF);
    Σr -=    3958 * cos( 4 * mD - 1 * sM1 - 1 * mM         );
    Σr +=    3258 * cos( 3 * mD           - 1 * mM         );
    Σr +=    2616 * cos( 2 * mD + 1 * sM1 + 1 * mM         );
    Σr -=    1897 * cos( 4 * mD - 1 * sM1 - 2 * mM         );
    Σr -=    2117 * cos(        + 2 * sM2 - 1 * mM         );
    Σr +=    2354 * cos( 2 * mD + 2 * sM2 - 1 * mM         );
    Σr -=    1423 * cos( 4 * mD           + 1 * mM         );
    Σr -=    1117 * cos(                  + 4 * mM         );
    Σr -=    1571 * cos( 4 * mD - 1 * sM1                  );
    Σr -=    1739 * cos( 1 * mD           - 2 * mM         );
    Σr -=    4421 * cos(                  + 2 * mM - 2 * mF);
    Σr +=    1165 * cos(        + 2 * sM2 + 1 * mM         );
    Σr +=    8752 * cos( 2 * mD           - 1 * mM - 2 * mF);

    const long double Δ = 385000.56 + Σr * 0.001; // in kilometers
    return Δ;
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

    // (AA.45.6)
    const long double E = 1.0  
        + T * (- 0.002516 
        + T * (- 0.0000074));

    const long double sM1 = sM * E;
    const long double sM2 = sM * E * E;

    // (AA.45.A)

    Σl += 6288774 * sin(                  + 1 * mM         );
    Σl += 1274027 * sin( 2 * mD           - 1 * mM         );
    Σl +=  658314 * sin( 2 * mD                            );
    Σl +=  213618 * sin(                  + 2 * mM         );
    Σl -=  185116 * sin(        + 1 * sM1                  );
    Σl -=  114332 * sin(                           + 2 * mF);
    Σl +=   58793 * sin( 2 * mD           - 2 * mM         );
    Σl +=   57066 * sin( 2 * mD - 1 * sM1 - 1 * mM         );
    Σl +=   53322 * sin( 2 * mD           + 1 * mM         );
    Σl +=   45758 * sin( 2 * mD - 1 * sM1                  );
    Σl -=   40923 * sin(        + 1 * sM1 - 1 * mM         );
    Σl -=   34720 * sin( 1 * mD                            );
    Σl -=   30383 * sin(        + 1 * sM1 + 1 * mM         );
    Σl +=   15327 * sin( 2 * mD                    - 2 * mF);
    Σl -=   12528 * sin(                  + 1 * mM + 2 * mF);
    Σl +=   10980 * sin(                  + 1 * mM - 2 * mF);
    Σl +=   10675 * sin( 4 * mD           - 1 * mM         );
    Σl +=   10034 * sin(                  + 3 * mM         );
    Σl +=    8548 * sin( 4 * mD           - 2 * mM         );
    Σl -=    7888 * sin( 2 * mD + 1 * sM1 - 1 * mM         );
    Σl -=    6766 * sin( 2 * mD + 1 * sM1                  );
    Σl -=    5163 * sin( 1 * mD           - 1 * mM         );
    Σl +=    4987 * sin( 1 * mD + 1 * sM1                  );
    Σl +=    4036 * sin( 2 * mD - 1 * sM1 + 1 * mM         );
    Σl +=    3994 * sin( 2 * mD           + 2 * mM         );
    Σl +=    3861 * sin( 4 * mD                            );
    Σl +=    3665 * sin( 2 * mD           - 3 * mM         );
    Σl -=    2689 * sin(        + 1 * sM1 - 2 * mM         );
    Σl -=    2602 * sin( 2 * mD           - 1 * mM + 2 * mF);
    Σl +=    2390 * sin( 2 * mD - 1 * sM1 - 2 * mM         );
    Σl -=    2348 * sin( 1 * mD           + 1 * mM         );
    Σl +=    2236 * sin( 2 * mD - 2 * sM2                  );
    Σl -=    2120 * sin(        + 1 * sM1 + 2 * mM         );
    Σl -=    2069 * sin(        + 2 * sM2                  );
    Σl +=    2048 * sin( 2 * mD - 2 * sM2 - 1 * mM         );
    Σl -=    1773 * sin( 2 * mD           + 1 * mM - 2 * mF);
    Σl -=    1595 * sin( 2 * mD                    + 2 * mF);
    Σl +=    1215 * sin( 4 * mD - 1 * sM1 - 1 * mM         );
    Σl -=    1110 * sin(                  + 2 * mM + 2 * mF);
    Σl -=     892 * sin( 3 * mD           - 1 * mM         );
    Σl -=     810 * sin( 2 * mD + 1 * sM1 + 1 * mM         );
    Σl +=     759 * sin( 4 * mD - 1 * sM1 - 2 * mM         );
    Σl -=     713 * sin(        + 2 * sM2 - 1 * mM         );
    Σl -=     700 * sin( 2 * mD + 2 * sM2 - 1 * mM         );
    Σl +=     691 * sin( 2 * mD + 1 * sM  - 2 * mM         );
    Σl +=     596 * sin( 2 * mD - 1 * sM1          - 2 * mF);
    Σl +=     549 * sin( 4 * mD           + 1 * mM         );
    Σl +=     537 * sin(                  + 4 * mM         );
    Σl +=     520 * sin( 4 * mD - 1 * sM1                  );
    Σl -=     487 * sin( 1 * mD           - 2 * mM         );
    Σl -=     399 * sin( 2 * mD + 1 * sM1          - 2 * mF);
    Σl -=     381 * sin(                  + 2 * mM - 2 * mF);
    Σl +=     351 * sin( 1 * mD + 1 * sM1 + 1 * mM         );
    Σl -=     340 * sin( 3 * mD           - 2 * mM         );
    Σl +=     330 * sin( 4 * mD           - 3 * mM         );
    Σl +=     327 * sin( 2 * mD - 1 * sM1 + 2 * mM         );
    Σl -=     323 * sin(        + 2 * sM2 + 1 * mM         );
    Σl +=     299 * sin( 1 * mD + 1 * sM1 - 1 * mM         );
    Σl +=     294 * sin( 2 * mD           + 3 * mM         );

    // (AA.45.B)

    Σb += 5128122 * sin(                           + 1 * mF);
    Σb +=  280602 * sin(                  + 1 * mM + 1 * mF);
    Σb +=  277693 * sin(                  + 1 * mM - 1 * mF);
    Σb +=  173237 * sin( 2 * mD                    - 1 * mF);
    Σb +=   55413 * sin( 2 * mD           - 1 * mM + 1 * mF);
    Σb +=   46271 * sin( 2 * mD           - 1 * mM - 1 * mF);
    Σb +=   32573 * sin( 2 * mD                    + 1 * mF);
    Σb +=   17198 * sin(                  + 2 * mM + 1 * mF);
    Σb +=    9266 * sin( 2 * mD           + 1 * mM - 1 * mF);
    Σb +=    8822 * sin(                  + 2 * mM - 1 * mF);
    Σb +=    8216 * sin( 2 * mD - 1 * sM1          - 1 * mF);
    Σb +=    4324 * sin( 2 * mD           - 2 * mM - 1 * mF);
    Σb +=    4200 * sin( 2 * mD           + 1 * mM + 1 * mF);
    Σb -=    3359 * sin( 2 * mD + 1 * sM1          - 1 * mF);
    Σb +=    2463 * sin( 2 * mD - 1 * sM1 - 1 * mM + 1 * mF);
    Σb +=    2211 * sin( 2 * mD - 1 * sM1          + 1 * mF);
    Σb +=    2065 * sin( 2 * mD - 1 * sM1 - 1 * mM - 1 * mF);
    Σb -=    1870 * sin(        + 1 * sM1 - 1 * mM - 1 * mF);
    Σb +=    1828 * sin( 4 * mD           - 1 * mM - 1 * mF);
    Σb -=    1794 * sin(        + 1 * sM1          + 1 * mF);
    Σb -=    1749 * sin(                           + 3 * mF);
    Σb -=    1565 * sin(        + 1 * sM1 - 1 * mM + 1 * mF);
    Σb -=    1491 * sin( 1 * mD                    + 1 * mF);
    Σb -=    1475 * sin(        + 1 * sM1 + 1 * mM + 1 * mF);
    Σb -=    1410 * sin(        + 1 * sM1 + 1 * mM - 1 * mF);
    Σb -=    1344 * sin(        + 1 * sM1          - 1 * mF);
    Σb -=    1335 * sin( 1 * mD                    - 1 * mF);
    Σb +=    1107 * sin(                  + 3 * mM + 1 * mF);
    Σb +=    1024 * sin( 4 * mD                    - 1 * mF);
    Σb +=     833 * sin( 4 * mD           - 1 * mM + 1 * mF);
    Σb +=     777 * sin(                  + 1 * mM - 3 * mF);
    Σb +=     671 * sin( 4 * mD           - 2 * mM + 1 * mF);
    Σb +=     607 * sin( 2 * mD                    - 3 * mF);
    Σb +=     596 * sin( 2 * mD           + 2 * mM - 1 * mF);
    Σb +=     491 * sin( 2 * mD - 1 * sM1 + 1 * mM - 1 * mF);
    Σb -=     451 * sin( 2 * mD           - 2 * mM + 1 * mF);
    Σb +=     439 * sin(                  + 3 * mM - 1 * mF);
    Σb +=     422 * sin( 2 * mD           + 2 * mM + 1 * mF);
    Σb +=     421 * sin( 2 * mD           - 3 * mM - 1 * mF);
    Σb -=     366 * sin( 2 * mD + 1 * sM1 - 1 * mM + 1 * mF);
    Σb -=     351 * sin( 2 * mD + 1 * sM1          + 1 * mF);
    Σb +=     331 * sin( 4 * mD                    + 1 * mF);
    Σb +=     315 * sin( 2 * mD - 1 * sM1 + 1 * mM + 1 * mF);
    Σb +=     302 * sin( 2 * mD - 2 * sM2          - 1 * mF);
    Σb -=     283 * sin(                  + 1 * mM + 3 * mF);
    Σb -=     229 * sin( 2 * mD + 1 * sM1 + 1 * mM - 1 * mF);
    Σb +=     223 * sin( 1 * mD + 1 * sM1          - 1 * mF);
    Σb +=     223 * sin( 1 * mD + 1 * sM1          + 1 * mF);
    Σb -=     220 * sin(        + 1 * sM1 - 2 * mM - 1 * mF);
    Σb -=     220 * sin( 2 * mD + 1 * sM1 - 1 * mM - 1 * mF);
    Σb -=     185 * sin( 1 * mD           + 1 * mM + 1 * mF);
    Σb +=     181 * sin( 2 * mD - 1 * sM1 - 2 * mM - 1 * mF);
    Σb -=     177 * sin(        + 1 * sM1 + 2 * mM + 1 * mF);
    Σb +=     176 * sin( 4 * mD           - 2 * mM - 1 * mF);
    Σb +=     166 * sin( 4 * mD - 1 * sM1 - 1 * mM - 1 * mF);
    Σb -=     164 * sin( 1 * mD           + 1 * mM - 1 * mF);
    Σb +=     132 * sin( 4 * mD           + 1 * mM - 1 * mF);
    Σb -=     119 * sin( 1 * mD           - 1 * mM - 1 * mF);
    Σb +=     115 * sin( 4 * mD - 1 * sM1          - 1 * mF);
    Σb +=     107 * sin( 2 * mD - 2 * sM2          + 1 * mF);
 
    //

    Σl +=  3958 * sin(A1)
         + 1962 * sin(mL - mF)
         +  318 * sin(A2);

    Σb += -2235 * sin(mL) 
         +  382 * sin(A3)
         +  175 * sin(A1 - mF)
         +  175 * sin(A1 + mF)
         +  127 * sin(mL - mM)
         -  115 * sin(mL + mM);

    t_eclCoords ecl;

    ecl.longitude = moon_meanLongitude(t) + Σl * 0.000001;
    ecl.latitude = Σb * 0.000001;

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