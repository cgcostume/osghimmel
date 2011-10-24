
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

#include "timef.h"

#include <math.h>

#include <osg/Timer>


TimeF::TimeF(
    const float time
,   const float secondsPerCycle)
:   m_timer(new osg::Timer())
,   m_secondsPerCycle(secondsPerCycle)
,   m_mode(M_Running)
,   m_offset(0.f)
,   m_lastModeChangeTime(0.f)
{
    initialize();
    setf(time, true);
}


TimeF::TimeF(
    const time_t &time
,   const float secondsPerCycle)
:   m_timer(new osg::Timer())
,   m_secondsPerCycle(secondsPerCycle)
,   m_mode(M_Running)
,   m_offset(0.f)
,   m_lastModeChangeTime(0.f)
{
    initialize();
    sett(time, true);
}

void TimeF::initialize()
{
    m_lastModeChangeTime = m_timer->time_s();

    m_timef[0] = 0.f;
    m_timef[1] = 0.f;
    m_timef[2] = 0.f;

    m_time[0]  = 0;
    m_time[1]  = 0;
    m_time[2]  = 0;
}


TimeF::~TimeF()
{
    delete m_timer;
}


void TimeF::update()
{
    float elapsed(M_Running == m_mode ? m_timer->time_s() : m_lastModeChangeTime);
    elapsed -= m_offset;

    if(M_Running == m_mode)
    {
        const float elapsedTimef(elapsed / m_secondsPerCycle);

        m_timef[1] = m_timef[0] + elapsedTimef;
        m_timef[1] -= static_cast<int>(m_timef[1]);

        m_time[1] = fToSeconds(elapsedTimef) + static_cast<float>(m_time[0]);
    }
    else
    {
        m_timef[1] = m_timef[0];
        m_time[1] = m_time[0];
    }
}


const float TimeF::getf(const bool updateFirst)
{
    if(updateFirst)
        update();

    return m_timef[1];
}


const float TimeF::setf(
    float timef
,   const bool forceUpdate)
{
    if(1.f == timef)
        timef = 0.f;

    m_timef[0] = timef;
    m_timef[2] = m_timef[0];

    const time_t seconds(fToSeconds(timef));
    struct tm lcl(*localtime(&m_time[0]));

    /*struct tm gmt(*gmtime(&m_time[0]));

    const int lcls(mktime(&lcl));
    const int gmts(mktime(&gmt));

    const time_t offseth((lcls - gmts) / 3600);
    */

    lcl.tm_hour = seconds / 3600; // - offseth;
    lcl.tm_min  = seconds % 3600 / 60;
    lcl.tm_sec  = seconds % 60;

    m_time[0] = mktime(&lcl);
    m_time[2] = m_time[0];

    if(forceUpdate)
        update();

    return getf();
}


const time_t TimeF::gett(const bool updateFirst)
{
    if(updateFirst)
        update();

    return m_time[1];
}


const time_t TimeF::sett(
    const time_t &time
,   const bool forceUpdate)
{
    m_time[0] = time;
    m_time[2] = m_time[0];

    m_timef[0] = secondsTof(time);
    m_timef[0] -= static_cast<int>(m_timef[0]);

    m_timef[2] = m_timef[0];

    if(forceUpdate)
        update();

    return gett();
}


const float TimeF::setSecondsPerCycle(const float secondsPerCycle)
{
    m_secondsPerCycle = secondsPerCycle;
    return getSecondsPerCycle();
}


inline const float TimeF::secondsTof(const time_t &time)
{
    return static_cast<float>(time / (60.f * 60.f * 24.f));
}


inline const time_t TimeF::fToSeconds(const float time)
{
    return static_cast<time_t>(time * 60.f * 60.f * 24.f);
}


void TimeF::run()
{
    if(M_Pausing == m_mode)
    {
        const float t(m_timer->time_s());
        m_offset += t - m_lastModeChangeTime;
        m_lastModeChangeTime = t;
    }
    m_mode = M_Running;

    update();
}


void TimeF::pause()
{
    if(M_Running == m_mode)
    {
        m_lastModeChangeTime = m_timer->time_s();
    }
    m_mode = M_Pausing;

    update();
}
 

void TimeF::reset()
{
    m_offset = 0.f;
    m_lastModeChangeTime = 0.f;

    m_timef[0] = m_timef[2];
    m_time[0] = m_time[0];

    delete m_timer;
    m_timer = new osg::Timer();

    update();
}
