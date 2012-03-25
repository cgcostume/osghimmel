
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


#include "test_astronomy2.h"

#include "test.h"

#include "include/mathmacros.h"
#include "include/atime.h"
#include "include/julianday.h"
#include "include/sideraltime.h"
#include "include/coords.h"
#include "include/stars.h"
#include "include/moon2.h"
#include "include/sun2.h"
#include "include/earth2.h"


void test_coords2();
void test_sun2();
void test_moon2();
void test_stars2();

void test_astronomy2()
{
    // Run Tests.
    test_coords2();
    test_sun2();
    test_moon2();
//    test_stars2();

    TEST_REPORT();
}

void test_coords2()
{
    t_julianDay t;

    // Test nutation and obliquity. (AA. Example 21.a)

    t = jd(t_aTime(1987,  4, 10, 0, 0, 0));

    ASSERT_AB(long double, -0.127296372348, jCenturiesSinceSE(t),              0.00000001);
    ASSERT_AB(long double, _decimal(23, 26, 27.407), Earth2::meanObliquity(t), 0.00005);

    ASSERT_AB(long double, _decimal( 0,  0,  9.443), Earth2::obliquityNutation(t), 0.0005);
    ASSERT_AB(long double, _decimal(23, 26, 36.850), Earth2::trueObliquity(t),     0.0005);

    ASSERT_AB(long double, _decimal( 0,  0, -3.788), Earth2::longitudeNutation(t), 0.0005);

    ASSERT_AB(long double, 229.27840, Moon2::meanAnomaly(t),  0.01);


    t = jd(t_aTime(1992, 10, 13, 0, 0, 0));

    ASSERT_AB(long double, -0.072183436, jCenturiesSinceSE(t),  0.00000001);

    ASSERT_AB(long double, 201.80719, Sun2::meanLongitude(t),   0.0001);
    ASSERT_AB(long double, 278.99396, Sun2::meanAnomaly(t),     0.005);
    ASSERT_AB(long double,  23.44023, Earth2::meanObliquity(t), 0.0001);

    ASSERT_AB(long double, 0.99766, _AUs(Sun2::distance(t)),    0.00001);
    ASSERT_AB(long double, 0.016711651, Earth2::orbitEccentricity(), 0.00001);


    t_equf equ = Sun2::apparentPosition(t);

    ASSERT_AB(long double,  13.225388, _hours(equ.right_ascension), 0.00005);
    ASSERT_AB(long double, - 7.78507 , equ.declination,             0.001);
}



void test_sun2()
{
    t_aTime aTime;

    // Berlin
    
    long double lat = _decimal(52, 31, 0);
    long double lon = _decimal(13, 24, 0);

    // Test nutation and obliquity.

    // Azimuth is interpreted from north from:
    // http://www.sunposition.info/sunposition/spc/locations.php

    aTime = t_aTime(2011, 12, 4,  8, 0, 0, 1 * 3600); 
    {
        const int hour[9] = { 8, 9, 10, 11, 12, 13, 14, 15, 16 };
        const int azim[9] = { 127, 139, 152, 166, 180, 195, 208, 221, 233 };
        const int alt[9]  = { -1, 6, 11, 14, 15, 14, 11, 5, -1 };

        for(unsigned int i = 0; i < 9; ++i)
        {
            aTime.hour = hour[i];
            t_horf hor = Sun2::horizontalPosition(aTime, lat, lon);

            ASSERT_AB(int, azim[i] - 180, hor.azimuth, 1);
            ASSERT_AB(int, alt[i], hor.altitude, 1);
        }
    }

    aTime = t_aTime(2011,  6, 4,  8, 0, 0, 2 * 3600); 
    {
        const int hour[17] = { 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21 };
        const int azim[17] = { 52, 63, 75, 86, 98,  112, 129, 151, 177, 204, 227, 245, 259, 271, 283, 294, 305 };
        const int alt[17]  = { 1, 8, 17, 26, 35, 44, 52, 57, 60, 58, 53, 45, 36, 27, 18, 10, 2 };

        for(unsigned int i = 0; i < 17; ++i)
        {
            aTime.hour = hour[i];
            t_horf hor = Sun2::horizontalPosition(aTime, lat, lon);

            ASSERT_AB(int, azim[i] - 180, hor.azimuth, 1);
            ASSERT_AB(int, alt[i], hor.altitude, 1);
        }
    }

    // random: Peru, Pueblo Libre

    lat = _decimal(-12,  5, 0);
    lon = _decimal(-77,  4, 0);

    aTime = t_aTime(2007,  5, 16,  0, 0, 0, -5 * 3600); 
    {
        const int hour[12] = { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 };
        const int azim[12] = { 68, 63, 56, 44, 27, 2, 336, 317, 305, 297, 292, 289 };
        const int alt[12]  = { 9, 22, 35, 46, 55, 59, 56, 48, 37, 24, 11, -3 };

        for(unsigned int i = 0; i < 12; ++i)
        {
            aTime.hour = hour[i];
            t_horf hor = Sun2::horizontalPosition(aTime, lat, lon);

            ASSERT_AB(int, azim[i] - 180, hor.azimuth, 1);
            ASSERT_AB(int, alt[i], hor.altitude, 1);
        }
    }
}


void test_moon2()
{
    // Berlin
    
    long double lat = _decimal(52, 31, 0);
    long double lon = _decimal(13, 24, 0);

    // Test nutation and obliquity.

    // Azimuth is interpreted from north from:
    // http://www.sunposition.info/sunposition/spc/locations.php

    const t_aTime aTime(1992, 4, 12); 

    const t_julianDay t(jd(aTime));
    
    ASSERT_AB(long double,  97.643514, Sun2::meanAnomaly(t),     0.005);

    ASSERT_AB(long double, 134.290186, Moon2::meanLongitude(t),  0.01);
    ASSERT_AB(long double,   5.150839, Moon2::meanAnomaly(t),    0.01);
    ASSERT_AB(long double, 113.842309, Moon2::meanElongation(t), 0.01);
    ASSERT_AB(long double, 219.889726, Moon2::meanLatitude(t),   0.01);

    t_eclf ecl = Moon2::position(t);

    ASSERT_AB(long double, 133.167269, ecl.longitude, 0.02);
    ASSERT_AB(long double,  -3.229127, ecl.latitude,  0.02);

    ASSERT_AB(long double, 368409.7, Moon2::distance(t), 1000.0);

    ASSERT_AB(long double, _decimal(0, 0, -3.788), Earth2::longitudeNutation(
        jd(t_aTime(1987, 4, 10))), 0.0005);
    ASSERT_AB(long double, _decimal(0, 0, +9.443), Earth2::obliquityNutation(
        jd(t_aTime(1987, 4, 10))), 0.0005);

    // Lunar perigee and apogee, values from: http://en.wikipedia.org/wiki/File:Lunar_perigee_apogee.png

    const t_aTime aTime1(2007, 10, 26);
    const t_aTime aTime2(2007,  4,  3);

    const t_julianDay t1(jd(aTime1));
    const t_julianDay t2(jd(aTime2));

    ASSERT_AB(long double, Earth2::apparentAngularMoonDiameter(t1) 
        / Earth2::apparentAngularMoonDiameter(t2), 1.13, 0.01);
}
