
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

#pragma once
#ifndef __ASTRONOMICAL_TIME_H__
#define __ASTRONOMICAL_TIME_H__

#include <time.h>

typedef struct s_AstronomicalTime
{
    s_AstronomicalTime(
        const short year
    ,   const short month
    ,   const short day
    ,   const short hour
    ,   const short minute
    ,   const short second
    ,   const short gmtOffset = 0);


    s_AstronomicalTime::s_AstronomicalTime(
        const short year
    ,   const short month
    ,   const long double day
    ,   const short gmtOffset = 0);

    s_AstronomicalTime(const time_t &t);


    const long double decimalDay() const;


    const bool operator==(const s_AstronomicalTime &other) const
    {
        return other.year   == year
            && other.month  == month
            && other.day    == day
            && other.hour   == hour
            && other.minute == minute
            && other.second == second;
    }

public:

    short year;
    short month;
    short day;
    short hour;
    short minute;
    short second;

    short gmtOffset;

} t_aTime;

#endif // __ASTRONOMICAL_TIME_H__