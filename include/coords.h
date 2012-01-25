
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
#ifndef __COORDS_H__
#define __COORDS_H__

#include "julianday.h"
#include "pragmanote.h"

#include <osg/Vec3>

// Explained in "Astronomical Algorithms" and http://en.wikipedia.org/wiki/Celestial_coordinate_system

template<typename T>
struct s_EquatorialCoords;

template<typename T>
struct s_EclipticalCoords;

template<typename T>
struct s_HorizontalCoords;

template<typename T>
struct s_GalacticCoords;


// http://en.wikipedia.org/wiki/Equatorial_coordinate_system

template<typename T>
struct s_EquatorialCoords 
{    
    s_EquatorialCoords();

    const s_EclipticalCoords<T> toEcliptical(const T obliquity) const;

    const s_HorizontalCoords<T> toHorizontal(
        const t_julianDay siderealTime /* θ_0 */
    ,   const T observersLatitude      /* Φ   */
    ,   const T observersLongitude     /* L   */) const;

    const osg::Vec3f toEuclidean() const;

// Not required for now...
//#pragma NOTE("const s_GalacticCoords toGalactic() const; not yet implemented")

    // Measures the angle (longitudinal) of an object east of the apparent 
    // location of the center of the Sun at the moment of the vernal equinox.
    T right_ascension; // α

    // Measures the angle (latitudinal) of an object above or below the
    // celestial equator. Positive, if north of the celestial equator.
    T declination;     // δ

    // Radial distance of this point to origin.
    T r;
};

typedef s_EquatorialCoords<long double> t_equd;
typedef s_EquatorialCoords<float> t_equf;


// http://en.wikipedia.org/wiki/Ecliptic_coordinate_system

template<typename T>
struct s_EclipticalCoords 
{    
    s_EclipticalCoords();

    const s_EquatorialCoords<T> toEquatorial(const T obliquity) const;


    // Ecliptical longitude. Measured from the vernal equinox along the ecliptic.
    T longitude; // λ

    // Ecliptical latitude. Positive if north of the ecliptic.
    T latitude;  // β
};

typedef s_EclipticalCoords<long double> t_ecld;
typedef s_EclipticalCoords<float> t_eclf;



// http://en.wikipedia.org/wiki/Horizontal_coordinate_system

template<typename T>
struct s_HorizontalCoords 
{    
    s_HorizontalCoords();

    const s_EquatorialCoords<T> toEquatorial(
        const t_julianDay siderealTime /* θ_0 */
    ,   const T observersLatitude      /* Φ   */
    ,   const T observersLongitude     /* L   */) const;

    const osg::Vec3f toEuclidean() const;

    // Measured westwards from the south. (longitudinal)
    T azimuth;  // h

    // Positive above, negative below horizon. (latitudinal) 
    T altitude; // A
};

typedef s_HorizontalCoords<long double> t_hord;
typedef s_HorizontalCoords<float> t_horf;


// Not required for now...

//// http://en.wikipedia.org/wiki/Galactic_coordinate_system
//
//typedef struct s_GalacticCoords 
//{    
//    s_GalacticCoords();
//
//#pragma NOTE("const s_EquatorialCoords toEquatorial() const; not yet implemented")
//
//    long double longitude; // l
//    long double latitude;  // b
//
//} t_galCoords;




#include "mathmacros.h"

#include <assert.h>

template<typename T>
s_EquatorialCoords<T>::s_EquatorialCoords()
:   right_ascension(0.0)
,   declination(0.0)
,   r(1.0)
{
}


template<typename T>
s_EclipticalCoords<T>::s_EclipticalCoords()
:   longitude(0.0)
,   latitude(0.0)
{
}


template<typename T>
s_HorizontalCoords<T>::s_HorizontalCoords()
:   azimuth(0.0)
,   altitude(0.0)
{
}


//s_GalacticCoords::s_GalacticCoords()
//:   longitude(0.0)
//,   latitude(0.0)
//{
//}



// To remove 180° ambiquity, atan2 is applied ("Astronomical Algorithms")

template<typename T>
const s_EclipticalCoords<T> s_EquatorialCoords<T>::toEcliptical(const T obliquity) const
{
    s_EclipticalCoords<T> ecl;
    
    const T cose(cos(_rad(obliquity)));
    const T sine(sin(_rad(obliquity)));
    const T sina(sin(_rad(right_ascension)));

    ecl.latitude = _deg(atan2(
        sina * cose + tan(_rad(declination)) * sine, cos(_rad(right_ascension))));

    ecl.longitude = _deg(asin(
        sin(_rad(declination)) * cose - cos(_rad(declination)) * sine * sina));

    return ecl;
}


// (AA.12.5) and (AA.12.6)

template<typename T>
const s_HorizontalCoords<T> s_EquatorialCoords<T>::toHorizontal(
    const t_julianDay siderealTime /* θ_0 */
,   const T observersLatitude      /* Φ   */
,   const T observersLongitude     /* L   */) const
{
    s_HorizontalCoords<T> hor;

    // local hour angle: H = θ - α (AA.p88)
    const T H = _rad(siderealTime * 15.0 + observersLongitude - right_ascension);

    const T cosh(cos(H));
    const T sinr(sin(_rad(observersLatitude)));
    const T cosr(cos(_rad(observersLatitude)));

    hor.altitude = _deg(asin(
        sinr * sin(_rad(declination)) + cosr * cos(_rad(declination)) * cosh));

    hor.azimuth = _deg(atan2(static_cast<T>(
        sin(H)), static_cast<T>(cosh * sinr - tan(_rad(declination)) * cosr)));

    return hor;
}


template<typename T>
const osg::Vec3f s_EquatorialCoords<T>::toEuclidean() const
{
    const T cosd(cos(_rad(declination)));

    const T x(r * sin(_rad(right_ascension)) * cosd);
    const T y(r * cos(_rad(right_ascension)) * cosd);
    const T z(r * sin(_rad(declination)));

    return osg::Vec3(x, y, z);
}


template<typename T>
const s_EquatorialCoords<T> s_EclipticalCoords<T>::toEquatorial(const T obliquity) const
{
    s_EquatorialCoords<T> equ;

    const T cose(cos(_rad(obliquity)));
    const T sine(sin(_rad(obliquity)));

    const T sinl(sin(_rad(longitude)));

    equ.right_ascension = _deg(atan2(
        sinl * cose - tan(_rad(latitude)) * sine, cos(_rad(longitude))));

    equ.declination = _deg(asin(sin(_rad(latitude)) * cose + cos(_rad(latitude)) * sine * sinl));

    return equ;
}


template<typename T>
const s_EquatorialCoords<T> s_HorizontalCoords<T>::toEquatorial(
    const t_julianDay siderealTime /* θ_0 */
,   const T observersLatitude  /* Φ   */
,   const T observersLongitude /* L   */) const
{
    s_EquatorialCoords<T> equ;

    const T cosa(cos(_rad(altitude)));

    const T sinr(sin(_rad(observersLatitude)));
    const T cosr(cos(_rad(observersLatitude)));

    const T H = _deg(atan2(
        sin(_rad(altitude)), cosa * sinr + tan(_rad(azimuth)) * cosr));

    equ.right_ascension = siderealTime - observersLongitude - H;

    equ.declination = _deg(asin(
        sinr * sin(_rad(azimuth)) - cosr * cos(_rad(azimuth)) * cosa));

    return equ;
}


template<typename T>
const osg::Vec3f s_HorizontalCoords<T>::toEuclidean() const
{
    const T cosa(cos(_rad(altitude)));

    const T x(sin(_rad(azimuth)) * cosa);
    const T y(cos(_rad(azimuth)) * cosa);
    const T z(sin(_rad(altitude)));

    return osg::Vec3(x, y, z);
}

#endif // __COORDS_H__