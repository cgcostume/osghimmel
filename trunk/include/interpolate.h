
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


#define interpolate_linear(t)       (t)

#define interpolate_smoothstep(t)   (t * t * (3 - 2 * t))
#define interpolate_smoothstep2(t)  (interpolate_smoothstep(interpolate_smoothstep(t)))
#define interpolate_smoothstep3(t)  (interpolate_smoothstep(interpolate_smoothstep2(t)))

#define interpolate_smootherstep(t) (t * t * t * (t * (6 * t - 15) + 10))

#define interpolate_squared(t)      (t * t)
#define interpolate_invsquared(t)   (1 - (1 - t) * (1 - t))

#define interpolate_cubed(t)        (t * t * t)
#define interpolate_invcubed(t)     (1 - (1 - t) * (1 - t) * (1 - t))

#define interpolate_sin(t)          (sin(t * 1.57079632679489661923))
#define interpolate_invsin(t)       (1 - sin((1 - t) * 1.57079632679489661923))


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
        return interpolate_smoothstep(t);
    case IM_SmoothStep2:
        return interpolate_smoothstep2(t);
    case IM_SmoothStep3:
        return interpolate_smoothstep3(t);
    case IM_SmootherStep:
        return interpolate_smootherstep(t);
    case IM_Squared:
        return interpolate_squared(t);
    case IM_InvSquared:
        return interpolate_invsquared(t);
    case IM_Cubed:
        return interpolate_cubed(t);
    case IM_InvCubed:
        return interpolate_invcubed(t);
    case IM_Sin:
        return static_cast<T>(interpolate_sin(t));
    case IM_InvSin:
        return static_cast<T>(interpolate_invsin(t));
    default:
    case IM_Linear:
        return interpolate_linear(t);
    }
}

#endif // __INTERPOLATE_H__