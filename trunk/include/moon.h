
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
#ifndef __MOON_H__
#define __MOON_H__

#include "julianday.h"
#include "coords.h"


const long double moon_meanLongitude(const t_julianDay t); 
const long double moon_meanElongation(const t_julianDay t);
const long double moon_meanAnomaly(const t_julianDay t);
const long double moon_meanLatitude(const t_julianDay t);

const long double moon_meanOrbitLongitude(const t_julianDay t);

const t_ecld moon_position(const t_julianDay t);
const t_equd moon_apparentPosition(const t_julianDay t);

const t_hord moon_horizontalPosition(
    const t_aTime &aTime
,   const long double latitude
,   const long double longitude);

const long double moon_distance(const t_julianDay t);

const long double moon_meanRadius();


// Low Accuracy

const float moon_meanLongitude_la(const t_julianDay t); 
const float moon_meanElongation_la(const t_julianDay t);
const float moon_meanAnomaly_la(const t_julianDay t);
const float moon_meanLatitude_la(const t_julianDay t);

const float moon_meanOrbitLongitude_la(const t_julianDay t);

const t_eclf moon_position_la(const t_julianDay t);
const t_equf moon_apparentPosition_la(const t_julianDay t);

const t_horf moon_horizontalPosition_la(
    const t_aTime &aTime
,   const float latitude
,   const float longitude);

const float moon_distance_la(const t_julianDay t);

#endif // __MOON_H__