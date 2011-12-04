
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

#include "coords.h"
#include "mathmacros.h"

#include <assert.h>


s_EquatorialCoords::s_EquatorialCoords()
:   right_ascension(0.0)
,   declination(0.0)
,   r(1.0)
{
}


s_EclipticalCoords::s_EclipticalCoords()
:   longitude(0.0)
,   latitude(0.0)
{
}


s_HorizontalCoords::s_HorizontalCoords()
:   azimuth(0.0)
,   altitude(0.0)
{
}


s_GalacticCoords::s_GalacticCoords()
:   longitude(0.0)
,   latitude(0.0)
{
}



// To remove 180° ambiquity, atan2 is applied ("Astronomical Algorithms")

const s_EclipticalCoords s_EquatorialCoords::toEcliptical(const long double obliquity) const
{
    t_eclCoords ecl;
    
    const long double cose(cos(_rad(obliquity)));
    const long double sine(sin(_rad(obliquity)));
    const long double sina(sin(_rad(right_ascension)));

    ecl.latitude = _deg(atan2(
        sina * cose + tan(_rad(declination)) * sine, cos(_rad(right_ascension))));

    ecl.longitude = _deg(asin(
        sin(_rad(declination)) * cose - cos(_rad(declination)) * sine * sina));

    return ecl;
}


const s_HorizontalCoords s_EquatorialCoords::toHorizontal(
    const t_julianDay siderealTime       /* θ_0 */
,   const long double observersLatitude  /* Φ   */
,   const long double observersLongitude /* L   */) const
{
    t_horCoords hor;

    // local hour angle: H = θ - α
    const long double H = _rad(siderealTime * 15.0 + observersLongitude - right_ascension);

    const long double cosh(cos(H));
    const long double sinr(sin(_rad(observersLatitude)));
    const long double cosr(cos(_rad(observersLatitude)));

    hor.altitude = _deg(asin(
        sinr * sin(_rad(declination)) + cosr * cos(_rad(declination)) * cosh));

    hor.azimuth = _deg(atan2(
        sin(H), cosh * sinr - tan(_rad(declination)) * cosr));

    return hor;
}


const s_EquatorialCoords s_EclipticalCoords::toEquatorial(const long double obliquity) const
{
    t_equCoords equ;

    const long double cose(cos(_rad(obliquity)));
    const long double sine(sin(_rad(obliquity)));

    const long double sinl(sin(_rad(longitude)));

    equ.right_ascension = _deg(atan2(
        sinl * cose - tan(_rad(latitude)) * sine, cos(_rad(longitude))));

    equ.declination = _deg(asin(sin(_rad(latitude)) * cose + cos(_rad(latitude)) * sine * sinl));

    return equ;
}


const s_EquatorialCoords s_HorizontalCoords::toEquatorial(
    const t_julianDay siderealTime       /* θ_0 */
,   const long double observersLatitude  /* Φ   */
,   const long double observersLongitude /* L   */) const
{
    t_equCoords equ;

    const long double cosa(cos(_rad(altitude)));

    const long double sinr(sin(_rad(observersLatitude)));
    const long double cosr(cos(_rad(observersLatitude)));

    const long double H = _deg(atan2(
        sin(_rad(altitude)), cosa * sinr + tan(_rad(azimuth)) * cosr));

    equ.right_ascension = siderealTime - observersLongitude - H;

    equ.declination = _deg(asin(
        sinr * sin(_rad(azimuth)) - cosr * cos(_rad(azimuth)) * cosa));

    return equ;
}


const osg::Vec3d s_HorizontalCoords::toEuclidean() const
{
    const long double cosa(cos(_rad(altitude)));

    const double x(sin(_rad(azimuth)) * cosa);
    const double y(cos(_rad(azimuth)) * cosa);
    const double z(sin(_rad(altitude)));

    return osg::Vec3d(x, y, z);
}
