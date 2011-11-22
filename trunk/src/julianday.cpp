
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

    const signed int i = aTime.year * 10000 + aTime.month * 100 + aTime.day;
    if(i >= 15821015)
    {
        // gregorian calendar
        const signed int a = static_cast<signed int>(aTime.year * 0.01);
        b = 2 - a + static_cast<signed int>(a * 0.25);
    }
    else if(i > 15821004)
        return 0.0; // The gregorian calender follows with 15. on the 4. 
                    // julian oktober in 1582 - poor people who missed a birthday these days :P

    return static_cast<int>(365.25 * (aTime.year + 4716)) 
        + static_cast<int>(30.6001 * (aTime.month + 1)) + aTime.day + h + b - 1524.5;
}


// Modified Julian Date.
const t_julianDate mjd(t_aTime aTime)
{
    return jd(aTime) - 2400000.5;
}