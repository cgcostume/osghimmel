
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

#include "stars.h"

#include "mathmacros.h"
#include "sideraltime.h"


const t_equd star_apparentPosition(
    const t_julianDay t
,   const long double α2000
,   const long double δ2000
,   const long double mpα2000
,   const long double mpδ2000)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    // (AA.20.1)

    const long double m  = (_decimal(0, 0, 3.07496)) + (_decimal(0, 0, 0.00186)) * T * 100.0;
    const long double n  = _arcsecs(_decimal(0, 0, 1.33621)) - _arcsecs(_decimal(0, 0, 0.00057)) * T * 100.0;

    const long double Δα = m + n * sin(_rad(mpα2000)) * tan(_rad(mpδ2000));
    const long double Δβ = n * cos(_rad(mpα2000));

    t_equd equ;

    //equ.right_ascension = 




    return equ;
}


const t_hord star_horizontalPosition(
    const t_aTime &aTime
,   const long double latitude
,   const long double longitude
,   const long double α2000
,   const long double δ2000
,   const long double mpα2000
,   const long double mpδ2000)
{
    t_julianDay t(jd(aTime));
    t_julianDay s(siderealTime(aTime));

    t_equd equ = star_apparentPosition(t, α2000, δ2000, mpα2000, mpδ2000);

    return equ.toHorizontal(s, latitude, longitude);
}