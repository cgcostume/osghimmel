
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
#ifndef __MATHMACROS_H__
#define __MATHMACROS_H__

#include <math.h>

#define _PI 3.1415926535897932384626433832795
#define _PI2  _PI * 2.00
#define _PI_2 _PI * 0.50
#define _PI4  _PI * 4.00
#define _PI_4 _PI * 0.25

#define _abs(v) \
    ((v < 0) ? -(v) : (v))

#define _int(v) \
    (static_cast<int>(v))

#define _short(v) \
    (static_cast<short>(v))

#define _mod(a, m) \
    ((a) - (m) * (_int((a) / (m)) - (a < 0 ? 1 : 0)))

#define _frac(x) \
    ((x) - _int(x))

#define _deg(rad) \
    ((rad) * 180.0 / _PI)

#define _rad(deg) \
    ((deg) * _PI / 180.0)

#define _decimal(deg, min, sec) \
    ((deg) + (min) / 60.0 + (sec) / 3600.0)

#define _sind(deg) \
    (sin(_rad(deg)))

#define _cosd(deg) \
    (cos(_rad(deg)))

#define _tand(deg) \
    (tan(_rad(deg)))

#define _asind(rad) \
    (_deg(asin(rad)))

#define _acosd(rad) \
    (_deg(acos(rad)))

#define _atand(rad) \
    (_deg(atan(rad)))

#define _atan2d(x, y) \
    (_deg(atan2(x, y)))

// normalizes an angle to between 0 and 2PI radians
#define _rev(rad) \
    ((rad) - floor((rad) / _PI2) * _PI2)

// normalizes an angle to between 0 and 360 degrees
#define _revd(deg) \
    ((deg) - floor((deg) / 360.0) * 360.0)

// cube root (e.g. needed for parabolic orbits)
#define _cbrt(x) \
    ((x > 0.0) ? exp(log(x) / 3.0) : (((x) < 0.0) ? -cbrt(-(x)) : 0.0))

#define _rightascd(deg, min, sec) \
    ((_decimal(deg, min, sec) * 15.0)

#define _rightasc(deg, min, sec) \
    (_rad(_rightascd(deg, min, sec)))

#define _day(h, m, s) \
    (_hour(h, m, s) / 24.0)

#define _hour(h, m, s) \
    ((h) + ((m) + (s) / 60.0) / 60.0)

#define _hours(t) \
    (_mod(t / 15.0, 24.0))

// When using powers, try to use Horner's Method


#endif // __MATHMACROS_H__