
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

#include "atime.h"

#include "mathmacros.h"
#include "timef.h"

#include <assert.h>


namespace osgHimmel
{

s_AstronomicalTime::s_AstronomicalTime()
:   year(0)
,   month(0)
,   day(0)
,   hour(0)
,   minute(0)
,   second(0)
,   utcOffset(0)
{
}


s_AstronomicalTime::s_AstronomicalTime(
    const short year
,   const short month
,   const short day
,   const short hour
,   const short minute
,   const short second
,   const short utcOffset)
:   year(year)
,   month(month)
,   day(day)
,   hour(hour)
,   minute(minute)
,   second(second)
,   utcOffset(utcOffset)
{
}


s_AstronomicalTime::s_AstronomicalTime(
    const short year
,   const short month
,   const t_longf day
,   const short utcOffset)
:   year(year)
,   month(month)
,   day(static_cast<unsigned short>(day))
,   utcOffset(utcOffset)
{
    const t_longf h = _frac(day) * 24.0;
    const t_longf m = _frac(h) * 60.0;
    const t_longf s = _frac(m) * 60.001;

    this->hour   = _short(h);
    this->minute = _short(m);
    this->second = _short(s);
}


const s_AstronomicalTime s_AstronomicalTime::fromTimeT(
    const time_t &time
,   const time_t &utcOffset)
{
    // Daylight saving time should not be concidered here -> julian time functions ignore this.

#ifdef __GNUC__
    struct tm lcl(*localtime(&time));
#else // __GNUC__
    struct tm lcl;
    localtime_s(&lcl, &time);
#endif // __GNUC__

    return s_AstronomicalTime(
        static_cast<short>(lcl.tm_year + 1900)
    ,   static_cast<short>(lcl.tm_mon + 1)
    ,   static_cast<short>(lcl.tm_mday)
    ,   static_cast<short>(lcl.tm_hour)
    ,   static_cast<short>(lcl.tm_min)
    ,   static_cast<short>(lcl.tm_sec)
    ,   static_cast<short>(utcOffset));
}


const s_AstronomicalTime s_AstronomicalTime::fromTimeF(const TimeF &t)
{
    return fromTimeT(t.gett(), t.getUtcOffset());
}


const time_t s_AstronomicalTime::toTime_t() const
{
    time_t t = 0;

#ifdef __GNUC__
    struct tm lcl(*localtime(&t));
#else // __GNUC__
    struct tm lcl;
    localtime_s(&lcl, &t);
#endif // __GNUC__

    lcl.tm_year = year - 1900;
    lcl.tm_mon  = month - 1;
    lcl.tm_mday = day;

    lcl.tm_hour = hour;
    lcl.tm_min  = minute;
    lcl.tm_sec  = second;

    lcl.tm_isdst = 0;

    t = mktime(&lcl);

    return t;
}


const t_longf s_AstronomicalTime::dayf() const
{
    return day + _day(hour, minute, second);
}

} // namespace osgHimmel