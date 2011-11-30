
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
    
    const long double cose(cos(obliquity));
    const long double sine(sin(obliquity));
    const long double sina(sin(right_ascension));

    ecl.latitude = atan2(
        sina * cose + tan(declination) * sine, cos(right_ascension));

    ecl.longitude = asin(
        sin(declination) * cose - cos(declination) * sine * sina);

    return ecl;
}


const s_HorizontalCoords s_EquatorialCoords::toHorizontal(
    const t_julianDay siderealTime       /* θ_0 */
,   const long double observersLatitude  /* Φ   */
,   const long double observersLongitude /* L   */) const
{
    t_horCoords hor;

    // local hour angle: H = θ - α
    const long double H = siderealTime - observersLongitude - right_ascension;

    const long double cosh(cos(H));
    const long double sinr(sin(observersLatitude));
    const long double cosr(cos(observersLatitude));

    hor.altitude = atan2(
        sin(H), cosh * sinr - tan(declination) * cosr);

    hor.azimuth = asin(
        sinr * sin(declination) + cosr * cos(declination) * cosh);

    return hor;
}


const s_EquatorialCoords s_EclipticalCoords::toEquatorial(const long double obliquity) const
{
    t_equCoords equ;

    const long double cose(cos(obliquity));
    const long double sine(sin(obliquity));

    const long double sinl(sin(longitude));

    equ.right_ascension = atan2(
        sinl * cose - tan(latitude) * sine, cos(longitude));

    equ.declination = asin(sin(latitude) * cose + cos(latitude) * sine * sinl);

    return equ;
}


const s_EquatorialCoords s_HorizontalCoords::toEquatorial(
    const t_julianDay siderealTime       /* θ_0 */
,   const long double observersLatitude  /* Φ   */
,   const long double observersLongitude /* L   */) const
{
    t_equCoords equ;

    const long double cosa(cos(altitude));

    const long double sinr(sin(observersLatitude));
    const long double cosr(cos(observersLatitude));

    const long double H = atan2(
        sin(altitude), cosa * sinr + tan(azimuth) * cosr);

    equ.right_ascension = siderealTime - observersLongitude - H;

    equ.declination = asin(
        sinr * sin(azimuth) - cosr * cos(azimuth) * cosa);

    return equ;
}


const osg::Vec3d s_HorizontalCoords::toEuclidean() const
{
    const long double cosa(cos(altitude));

    const double x(sin(azimuth) * cosa);
    const double y(cos(azimuth) * cosa);
    const double z(sin(altitude));

    return osg::Vec3d(x, y, z);
}
