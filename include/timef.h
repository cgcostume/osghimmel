
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
#ifndef __TIMEF_H__
#define __TIMEF_H__

#include <time.h>

namespace osg
{
    class Timer;
}


class TimeF
{
public:
    TimeF(
        const float time = 0.f
    ,   const float secondsPerCycle = 0.f);

    TimeF(
        const time_t &time
    ,   const float secondsPerCycle = 0.f);

    ~TimeF();

    void update();

    inline const float getf() const
    {
        return m_timef[1];
    }

    const float getf(const bool forceUpdate);
    const float setf(
        const float time
    ,   const bool forceUpdate = false);

    inline const time_t gett() const
    {
        return m_time[1];
    }

    const time_t gett(const bool forceUpdate);
    const time_t sett(
        const time_t &time
    ,   const bool forceUpdate = false);

    inline const float getSecondsPerCycle() const
    {
        return m_secondsPerCycle;
    }

    const float setSecondsPerCycle(const float secondsPerCycle);

protected:
    static inline const float secondsTof(const time_t &time);
    static inline const time_t fToSeconds(const float time);

protected:
    osg::Timer *m_timer;

    time_t m_time[2];
    float m_timef[2];

    float m_secondsPerCycle;
};

#endif __TIMEF_H__