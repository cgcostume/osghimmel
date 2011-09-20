
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
#ifndef __TWOUNITSCHANGER_H__
#define __TWOUNITSCHANGER_H__

#include <map>
#include <osg/GL>

class TwoUnitsChanger
{
public:
    TwoUnitsChanger();
    virtual ~TwoUnitsChanger();

    // Specify the duration in timef for blending between two textures.
    // To avoid faulty blending, this should be smaller than the smallest 
    // time range, since only two textures will be blended at a time.

    const float setTransitionDuration(const float duration);
    inline const float getTransitionDuration() const
    {
        return m_transitionDuration;
    }

    // Append a time range using the given texture unit. Example: If you
    // add unit 0 at time 0.2 and unit 1 at 0.8, then with a blendDurationf
    // of 0.1 the following blending will occur:

    // time  0.0  0.1  0.2  0.3  0.4  0.5  0.6  0.7  0.8  0.9  1.0/0.0
    // unit   1    1   1/0   0    0    0    0    0   0/1   1    1

    void pushUnit(
        const GLint unit
    ,   const float time = 1.f);


    // Getter (they call update if required).

    inline const GLint getBack(const float time) const;
    inline const GLint getSrc (const float time) const;

    inline const float getSrcAlpha(const float time) const;

    inline const bool hasBackChanged() const
    {
        return m_backHasChanged;
    }
    inline const bool hasSrcChanged() const
    {
        return m_srcHasChanged;
    }

protected:

    void update(const float time) const;

    void updateSmallestRange();
    void checkAndAdjustTransitionDuration();

    // This is required, since the transition duration can change, 
    // and the back units by time change.
    void updateBackUnits() const; 

protected:

    float m_transitionDuration;

    typedef std::map<float, GLint> t_unitsByTime;

    t_unitsByTime m_unitsByTime;
    mutable t_unitsByTime m_backUnitsByTime;

    float m_smallestRange;

    // cache (make mutable to provide const getter)

    mutable float m_lastTime;

    mutable GLint m_back;
    mutable GLint m_src;

    mutable float m_srcAlpha;

    
    mutable bool m_backHasChanged;
    mutable bool m_srcHasChanged;
};

#endif __TWOUNITSCHANGER_H__