
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

#include "astronomy.h"

#include "earth.h"
#include "sun.h"
#include "moon.h"
#include "stars.h"
#include "siderealtime.h"


namespace osgHimmel
{

Astronomy::Astronomy()
{
}


const float Astronomy::angularSunRadius(const t_julianDay t) const
{
    return Earth::apparentAngularSunDiameter(t) * 0.5;
}


const float Astronomy::angularMoonRadius(const t_julianDay t) const
{
    return Earth::apparentAngularMoonDiameter(t) * 0.5;
}


const osg::Vec3 Astronomy::moonPosition(
    const t_aTime &aTime
,   const float latitude
,   const float longitude) const
{
    t_hord moon = Moon::horizontalPosition(aTime, latitude, longitude);

    osg::Vec3 moonv  = moon.toEuclidean();
    moonv.normalize();

    return moonv;
}


const osg::Vec3 Astronomy::sunPosition(
    const t_aTime &aTime
,   const float latitude
,   const float longitude) const
{
    t_hord sun = Sun::horizontalPosition(aTime, latitude, longitude);

    osg::Vec3 sunv  = sun.toEuclidean();
    sunv.normalize();

    return sunv;
}


const osg::Matrix Astronomy::moonOrientation(
    const t_aTime &aTime
,   const float latitude
,   const float longitude) const
{    
    const t_julianDay t(jd(aTime));

    t_longf l, b;
    Moon::opticalLibrations(t, l, b);

    const osg::Matrix libLat = osg::Matrix::rotate(_rad(b), -1, 0, 0);
    const osg::Matrix libLon = osg::Matrix::rotate(_rad(l),  0, 1, 0);

    const float a = _rad(Moon::positionAngleOfAxis(t));
    const float p = _rad(Moon::parallacticAngle(aTime, latitude, longitude));

    const osg::Matrix zenith = osg::Matrix::rotate(a - p, 0, 0, 1);

    // finalOrientationWithLibrations
    const osg::Matrix R(libLat * libLon * zenith);

    return R;
}


const osg::Matrix Astronomy::equToLocalHorizonMatrix() const
{
    const t_aTime aTime(getATime());

    const float s = siderealTime(aTime);

    const float la = getLatitude();
    const float lo = getLongitude();

    return osg::Matrix::scale                  (-1, 1, 1)
        * osg::Matrix::rotate( _rad(la) - _PI_2, 1, 0, 0)
        * osg::Matrix::rotate(-_rad(s + lo)    , 0, 0, 1);
}

} // namespace osgHimmel