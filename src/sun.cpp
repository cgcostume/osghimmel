
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

#include "sun.h"

#include "earth.h"
#include "moon.h"
#include "sideraltime.h"
#include "mathmacros.h"

#include <assert.h>


// Mean anomaly (AA.45.3).

const long double sun_meanAnomaly(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    // seems most accurate... :o
    const long double M = 357.5291092
        + T * (+ 35999.0502909
        + T * (-     0.0001536
        + T * (+ 1.0 / 24490000.0)));

    // AA uses different coefficients all over the book...
    // ...taken the (probably) most accurate above

    // (AA.21 ...)
    //const long double M = 357.52772
    //    + T * (+  35999.050340
    //    + T * (-      0.0001603
    //    + T * (- 1.0 / 300000.0)));

    // (AA.24.3)
    //const long double M = 357.52910
    //    + T * (+ 35999.05030
    //    + T * (-     0.0001559
    //    + T * (-     0.00000048)));

    return _revd(M);
}


const long double sun_meanLongitude(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const long double L0 = 280.46645
        + T * (+ 36000.76983  
        + T * (+     0.0003032));

    return _revd(L0);
}


// (AA p152)

const long double sun_center(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));
    
    const long double M = _rad(sun_meanAnomaly(t));

    const long double C = 
        + (1.914600 - T * (0.004817 - T * 0.000014)) * sin(M)
        + (0.019993 - T *  0.000101) * sin(2.0 * M)
        +  0.000290 * sin(3.0 * M);

    return C;
}


const long double sun_trueAnomaly(const t_julianDay t)
{
    return sun_meanAnomaly(t) + sun_center(t); // v = M + C
}


// True geometric longitude referred to the mean equinox of the date.

const long double sun_trueLongitude(const t_julianDay t)
{
    return sun_meanLongitude(t) + sun_center(t); // Θ
}


const t_equd sun_apparentPosition(const t_julianDay t)
{
    t_equd equ;

    const t_julianDay T(jCenturiesSinceSE(t));

    const long double O = _rad(moon_meanOrbitLongitude(t));
    const long double e = _rad(earth_trueObliquity(t) + 0.00256 * cos(O));
    const long double l = _rad(sun_trueLongitude(t) - 0.00569 - 0.00478 * sin(O));

    const long double sinl = sin(l);

    equ.right_ascension = _revd(_deg(atan2(cos(e) * sinl, cos(l))));
    equ.declination = _deg(asin(sin(e) * sinl));

    return equ;
}


const t_hord sun_horizontalPosition(
    const t_aTime &aTime
,   const long double latitude
,   const long double longitude)
{
    t_julianDay t(jd(aTime));
    t_julianDay s(siderealTime(aTime));

    t_equd equ = sun_apparentPosition(t);

    return equ.toHorizontal(s, latitude, longitude);
}


// NOTE: This gives the distance from the center of the sun to the
// center of the earth.

const long double sun_distance(const t_julianDay t)
{
    // (AA.24.5)
    const long double e = earth_orbitEccentricity(t);

    const long double R = 1.000001018 * (1.0 - e * e) /
        (1.0 + e * cos(_rad(sun_trueAnomaly(t))));  // in AU

    return _kms(R);
}


const long double sun_meanRadius()
{
    // http://nssdc.gsfc.nasa.gov/planetary/factsheet/sunfact.html

    return 0.696e+6; // in kilometers
}


// LOW ACCURACY

const float sun_meanAnomaly_la(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    // Low Accuracy 
    // ("A Physically-Based Night Sky Model" - 2001 - Wann Jensen et al.)
    const float M = _deg(6.24 + 628.302 * T);

    // Low Accuracy - suggestion (seems insuficient) from 
    // http://wlym.com/~animations/ceres/calculatingposition/eccentric.html
    //const float M = 357.528 + 0.9856003 * t;

    return _revd(M);
}


const float sun_meanLongitude_la(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const float L0 = 280.4665
        + T * (+  36000.7698);

    return _revd(L0);
}


const t_equf sun_apparentPosition_la(const t_julianDay t)
{
    // ("A Physically-Based Night Sky Model" - 2001 - Wann Jensen et al.)

    const t_julianDay T(jCenturiesSinceSE(t));
    const float M = _rad(sun_meanAnomaly_la(t));

    t_eclf ecl;

    ecl.longitude = _deg(4.895048 + 628.331951 * T
        + (0.033417 - 0.000084 * T) * sin(M) + 0.000351 * sin(2 * M));
    ecl.latitude = 0;

    t_equf equ;
    equ = ecl.toEquatorial(earth_trueObliquity(t));

    return equ;
}


const t_horf sun_horizontalPosition_la(
    const t_aTime &aTime
,   const float latitude
,   const float longitude)
{
    t_julianDay t(jd(aTime));
    t_julianDay s(siderealTime(aTime));

    t_equf equ = sun_apparentPosition_la(t);

    return equ.toHorizontal(s, latitude, longitude);
}


// NOTE: This gives the distance from the center of the sun to the
// center of the earth.

const float sun_distance_la(const t_julianDay t)
{
    // ("A Physically-Based Night Sky Model" - 2001 - Wann Jensen et al.)
    const t_julianDay T(jCenturiesSinceSE(t));
    
    const float M = 6.24 + 628.302 * T;

    const float R = 1.000140 - (0.016708 - 0.000042 * T) * cos(M)
      - 0.000141 * cos(2 * M); // in AU

    return _kms(R);
}