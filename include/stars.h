
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
#ifndef __STARS_H__
#define __STARS_H__

#include "typedefs.h"
#include "julianday.h"
#include "coords.h"


class Stars
{
public:

    static const t_longf tempratureFromBV(const t_longf BtoV);

    static const osg::Vec2f planckianLocusInCieXYZ(const t_longf t /* temperature in K */);

    static const osg::Vec3f xyzTristimulus(const t_longf t /* temperature in K */);
    static const osg::Vec3f xyzTristimulus(const osg::Vec2f planckianLocus);

    static const osg::Vec3f sRgbColor(const t_longf t /* temperature in K */);
    static const osg::Vec3f sRgbColor(const osg::Vec2f planckianLocus);
    static const osg::Vec3f sRgbColor(const osg::Vec3f xyzTrisimulus);


    static const t_equd apparentPosition(
        const t_julianDay t
    ,   const t_longf a2000   /* right_ascension (RA) in decimal degrees, equinox J2000 */
    ,   const t_longf d2000   /* declination (DE) in decimal degrees, equinox J2000 */
    ,   const t_longf mpa2000 /* annual proper motion in RA J2000 */
    ,   const t_longf mpd2000 /* annual proper motion in DE J2000 */);

    static const t_hord horizontalPosition(
        const t_aTime &aTime
    ,   const t_longf latitude
    ,   const t_longf longitude
    ,   const t_longf a2000   /* right_ascension (RA) in decimal degrees, equinox J2000 */
    ,   const t_longf d2000   /* declination (DE) in decimal degrees, equinox J2000 */
    ,   const t_longf mpa2000 /* annual proper motion in RA J2000 */
    ,   const t_longf mpd2000 /* annual proper motion in DE J2000 */);
};

#endif // __STARS_H__