
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

#include "julianday.h"

#include "mathmacros.h"

#include <assert.h>


const t_julianDay jd(t_aTime aTime)
{
    assert(aTime.month > 0);

    if(aTime.month < 3)
    {
        // "If the date is in January or February, it is concidered to
        // be 13th or 14th month of the preceding year."

        --aTime.year;
        aTime.month += 12;
    }

    // TODO: from "A Physically-Based Night Sky Model" - 2001 - Wann Jensen et. al.
    // "Local time is GMT with a zone correction. Terrestrial Time (TT)
    // is essentially the time kept by atomic clocks. As it is not corrected
    // for the slowing of the Earth’s rotation, it gains on GMT by about 
    // a second per year. The current difference T is about 64 sec in 2000.
    // It should be added to s in all equations for precise computation..."

    unsigned int s = aTime.second;

    const long double h(_day(aTime.hour, aTime.minute, s));

    signed int b = 0;

    const int i = aTime.year * 10000 + aTime.month * 100 + aTime.day;
    if(i >= 15821015)
    {
        // gregorian calendar
        const signed int a = _int(aTime.year * 0.01);
        b = 2 - a + _int(a * 0.25);
    }
    else if(i > 15821004)
        return 0.0; // The gregorian calender follows with 15. on the 4. 
                    // julian oktober in 1582 - poor people who missed a birthday these days :P

    return _int(365.25 * (aTime.year + 4716)) 
        + _int(30.600001 * (aTime.month + 1)) + aTime.day + h + b - 1524.5;
}


const t_julianDay jdUT(const t_aTime &aTime)
{
    return jd(0 == aTime.utcOffset ? aTime : makeUT(aTime));
}


const t_julianDay jd0UT(t_aTime aTime)
{
    aTime.second = 0;
    aTime.minute = 0;
    aTime.hour   = 0;

    return jd(0 == aTime.utcOffset ? aTime : makeUT(aTime));
}


// Modified Julian Date.
const t_julianDay mjd(t_aTime aTime)
{
    return jd(aTime) - 2400000.5;
}


const t_aTime makeTime(
    t_julianDay julianDate
,   const short GMTOffset)
{
    assert(julianDate >= 0.0);

    julianDate += 0.5;

    const int z = _int(julianDate);
    const long double f = _frac(julianDate);

    long int a(z); // julian
    if(z >= 2299161) // gregorian
    {
        const int g = _int((z - 1867216.25) / 36524.25);
        a = z + 1 + g - _int(g / 4);
    }

    const int b = _int(a + 1524);
    const int c = _int((b - 122.1) / 365.25);
    const int d = _int(365.25 * c);
    const int e = _int((b - d) / 30.600001);


    const short day    = b - d - _int(30.600001 * e);
    const short month  = e < 14 ? e - 1 : e - 13;
    const short year   = month > 2 ? c - 4716 : c - 4715;

    const long double h = f * 24.0;
    const long double m = _frac(h) * 60.0;
    const long double s = _frac(m) * 60.0001;

    const short hour   = _short(h);
    const short minute = _short(m);
    const short second = _short(s);

    return t_aTime(year, month, day, hour, minute, second, GMTOffset);
}


const t_aTime makeUT(const t_aTime &aTime)
{
    return makeTime(jd(aTime) - aTime.utcOffset / 3600.0 / 24.0, 0);
}


// http://en.wikipedia.org/wiki/Equatorial_coordinate_system
// Over long periods of time, precession and nutation effects alter the Earth's orbit and thus 
// the apparent location of the stars. Likewise, proper motion of the stars themselves will affect 
// their coordinates as seen from Earth. When considering observations separated by long intervals, 
// it is necessary to specify an epoch (frequently J2000.0, for older data B1950.0) when specifying 
// coordinates of planets, stars, galaxies, etc.

const t_julianDay standardEquinox()
{
    return j2000();
}


const t_julianDay j2000()
{
    return 2451545.0; // 2000 January 1 noon TT
    // 2451543.5 was used by http://www.stjarnhimlen.se/comp/tutorial.html
}


const t_julianDay j2050()
{
    return 2469807.5; // 2050 January 1 noon TT
}


const t_julianDay b1900()
{
    return 2415020.3135; // 1900 January 0.8135 TT
}


const t_julianDay b1950()
{
    return 2433282.4235; // 1950 January 0.9235 TT
}


const t_julianDay jdSinceSE(const t_julianDay jd)
{
    return jd - standardEquinox();
}


const t_julianDay jCenturiesSinceSE(const t_julianDay jd)   // T
{
    // Even if google says that 1 century has currently 36 524.2199 days, 
    // the julian century is explicitly defined as 36525 days...

    // Used in http://en.wikipedia.org/wiki/Julian_year_(astronomy) and 
    // "Astronomical Algorithms"
    return jdSinceSE(jd) / 36525.0; 
}
