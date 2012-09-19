
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
#ifndef __INTERPOLATE_H__
#define __INTERPOLATE_H__

#include <math.h>


namespace osgHimmel
{

#define _linear(t)       (t)

#define _smoothstep(t)   ((t) * (t) * (3 - 2 * (t)))
#define _smoothstep2(t)  (_smoothstep(_smoothstep(t)))
#define _smoothstep3(t)  (_smoothstep(_smoothstep2(t)))

#define _smootherstep(t) ((t) * (t) * (t) * ((t) * (6 * (t) - 15) + 10))

#define _squared(t)      ((t) * (t))
#define _invsquared(t)   (1 - (1 - (t)) * (1 - (t)))

#define _cubed(t)        ((t) * (t) * (t))
#define _invcubed(t)     (1 - (1 - (t)) * (1 - (t)) * (1 - (t)))

#define _sin(t)          (sin(t * 1.57079632679489661923))
#define _invsin(t)       (1 - sin((1 - (t)) * 1.57079632679489661923))


#define _smoothstep_ext(t, l, r) \
    ((t) < (l) ? 0 : (r) < (t) ? 1 : _smoothstep(((t) - (l)) / ((r) - (l))))

// Several interpolation methods in action: http://sol.gfxile.net/interpolation/

enum e_InterpolationMethod
{
    IM_Linear
,   IM_SmoothStep
,   IM_SmoothStep2
,   IM_SmoothStep3
,   IM_SmootherStep // Ken Perlin
,   IM_Squared
,   IM_InvSquared
,   IM_Cubed
,   IM_InvCubed
,   IM_Sin          // strong in, soft out
,   IM_InvSin       // soft in, strong out
};


template<typename T>
inline const T interpolate(
    const T t
,   const e_InterpolationMethod function = IM_Linear)
{
    switch(function)
    {
    case IM_SmoothStep:
        return _smoothstep(t);
    case IM_SmoothStep2:
        return _smoothstep2(t);
    case IM_SmoothStep3:
        return _smoothstep3(t);
    case IM_SmootherStep:
        return _smootherstep(t);
    case IM_Squared:
        return _squared(t);
    case IM_InvSquared:
        return _invsquared(t);
    case IM_Cubed:
        return _cubed(t);
    case IM_InvCubed:
        return _invcubed(t);
    case IM_Sin:
        return static_cast<T>(_sin(t));
    case IM_InvSin:
        return static_cast<T>(_invsin(t));
    default:
    case IM_Linear:
        return _linear(t);
    }
}

} // namespace osgHimmel

#endif // __INTERPOLATE_H__