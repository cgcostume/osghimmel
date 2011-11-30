
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
#ifndef __COORDS_H__
#define __COORDS_H__

#include "julianday.h"
#include "pragmanote.h"

#include <osg/Vec3d>

// Explained in "Astronomical Algorithms" and http://en.wikipedia.org/wiki/Celestial_coordinate_system

struct s_EquatorialCoords;
struct s_EclipticalCoords;
struct s_HorizontalCoords;
struct s_GalacticCoords;


const long double longitudeNutation(const t_julianDay jd);
const long double obliquityNutation(const t_julianDay jd);

const long double meanObliquity(const t_julianDay jd);
const long double trueObliquity(const t_julianDay jd);


// http://en.wikipedia.org/wiki/Equatorial_coordinate_system

typedef struct s_EquatorialCoords 
{    
    s_EquatorialCoords();

    const s_EclipticalCoords toEcliptical(const long double obliquity) const;
    const s_HorizontalCoords toHorizontal(
        const t_julianDay siderealTime       /* θ_0 */
    ,   const long double observersLatitude  /* Φ   */
    ,   const long double observersLongitude /* L   */) const;

#pragma NOTE("const s_GalacticCoords toGalactic() const; not yet implemented")

    // Measures the angle (longitudinal) of an object east of the apparent 
    // location of the center of the Sun at the moment of the vernal equinox.
    long double right_ascension; // α

    // Measures the angle (latitudinal) of an object above or below the
    // celestial equator. Positive, if north of the celestial equator.
    long double declination;     // δ

    // Radial distance of this point to origin.
    long double r;

} t_equCoords;




// http://en.wikipedia.org/wiki/Ecliptic_coordinate_system

typedef struct s_EclipticalCoords 
{    
    s_EclipticalCoords();

    const s_EquatorialCoords toEquatorial(const long double obliquity) const;


    // Ecliptical longitude. Measured from the vernal equinox along the ecliptic.
    long double longitude; // λ

    // Ecliptical latitude. Positive if north of the ecliptic.
    long double latitude;  // β

} t_eclCoords;




// http://en.wikipedia.org/wiki/Horizontal_coordinate_system

typedef struct s_HorizontalCoords 
{    
    s_HorizontalCoords();

    const s_EquatorialCoords toEquatorial(
        const t_julianDay siderealTime       /* θ_0 */
    ,   const long double observersLatitude  /* Φ   */
    ,   const long double observersLongitude /* L   */) const;
    
    const osg::Vec3d toEuclidean() const;


    // Measured westwards from the south. (longitudinal)
    long double azimuth;  // h

    // Positive above, negative below horizon. (latitudinal) 
    long double altitude; // A

} t_horCoords;




// http://en.wikipedia.org/wiki/Galactic_coordinate_system

typedef struct s_GalacticCoords 
{    
    s_GalacticCoords();

#pragma NOTE("const s_EquatorialCoords toEquatorial() const; not yet implemented")

    long double longitude; // l
    long double latitude;  // b

} t_galCoords;

#endif // __COORDS_H__