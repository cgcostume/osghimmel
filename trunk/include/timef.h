
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

#pragma once
#ifndef __TIMEF_H__
#define __TIMEF_H__

#include <time.h>

namespace osg
{
    class Timer;
}


// TimeF manages an osg::Timer and features an interface for floating time
// in the closed interval [0;1] representing a full day and standard c 
// time (time_t) simultaneously. The time updates have to be requested 
// explicitly, thus simplifying usage between multiple recipients.
// The time starts cycling automatically, but can also be paused, stopped, 
// or set to a specific value.

class TimeF
{
    enum e_Mode
    {
        M_Running
    ,   M_Pausing
    };

public:
    
    TimeF(
        const long double time = 0.0
    ,   const long double secondsPerCycle = 0.0);

    TimeF(
        const time_t &time
    ,   const time_t &utcOffset // In seconds (UTC+01:00 is m_utcOffset = +3600).
    ,   const long double secondsPerCycle = 0.0);

    ~TimeF();

    // Increments time appropriate to secondsPerCycle.
    void update();

    // Cycling manipulation - does not tamper the time.

    void start(const bool forceUpdate = false);
    void pause(const bool forceUpdate = false);
    void reset(const bool forceUpdate = false); // Resets the time to initial value (secondsPerCycle remain unchanged).
    void stop (const bool forceUpdate = false); // Stops and resets the time.

    inline const long double getSecondsPerCycle() const
    {
        return m_secondsPerCycle;
    }

    const long double setSecondsPerCycle(const long double secondsPerCycle);


    // Float time in the intervall [0;1]
    inline const long double getf() const
    {
        return m_timef[1];
    }

    const long double getf(const bool updateFirst);

    // Sets only time, date remains unchanged.
    const long double setf(
        long double time
    ,   const bool forceUpdate = false);

    // Elapsed float time from initialized time.
    const long double getNonModf(const bool updateFirst = false);

    // Time in seconds from initial time.
    inline const time_t gett() const
    {
        return m_time[1] + _timezone;
    }

    const time_t gett(const bool updateFirst);
    const time_t sett(
        const time_t &time
    ,   const bool forceUpdate = false);

    const time_t getUtcOffset() const;
    const time_t setUtcOffset(const time_t &utcOffset /* In Seconds. */);

    // 

    const bool isRunning() const;

protected:
    static inline const long double secondsTof(const time_t &time);
    static inline const time_t fToSeconds(const long double time);

    void initialize();

protected:
    osg::Timer *m_timer;

    time_t m_utcOffset;

    time_t m_time[3];       // [2] is for stop
    long double m_timef[3]; // [2] is for stop

    long double m_offset;

    e_Mode m_mode;
    long double m_lastModeChangeTime;

    long double m_secondsPerCycle;
};

#endif // __TIMEF_H__