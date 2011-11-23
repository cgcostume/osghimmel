
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

#include "julianday.h"
#include "mathmacros.h"

#include <assert.h>


const t_julianDate jd(t_aTime aTime)
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

    const long double h(_hour(aTime.hour, aTime.minute, s));

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


// Modified Julian Date.
const t_julianDate mjd(t_aTime aTime)
{
    return jd(aTime) - 2400000.5;
}


const t_aTime makeTime(
    t_julianDate julianDate
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