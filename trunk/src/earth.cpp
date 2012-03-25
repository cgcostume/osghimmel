
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

#include "earth.h"

#include "sun.h"
#include "moon.h"
#include "mathmacros.h"

#include <assert.h>


namespace osgHimmel
{

// The linear eccentricity of the earth orbit is about 2.5 * 10^6 km.
// Compared to the avg. distance of 149.6 * 10^6 km this is not much.
// http://www.greier-greiner.at/hc/ekliptik.htm

// P. Bretagnon, "Théorie du mouvement de l'ensamble des planètes. Solution VSOP82", 1982

const t_longf Earth::orbitEccentricity(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const t_longf E = 0.01670862
        + T * (- 0.000042037
        + T * (- 0.0000001236
        + T * (+ 0.00000000004)));

    // (AA.24.4)
    //const t_longf E = 0.016708617
    //    + T * (- 0.000042037
    //    + T * (- 0.0000001236));

    return _revd(E);
}


const t_longf Earth::apparentAngularSunDiameter(const t_julianDay t)
{
    return _adiameter(Sun::distance(t), Sun::meanRadius());
}


const t_longf Earth::apparentAngularMoonDiameter(const t_julianDay t)
{
    return _adiameter(Moon::distance(t), Moon::meanRadius());
}


const t_longf Earth::longitudeNutation(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const t_longf sM = _rad(Sun::meanAnomaly(t));

    const t_longf mM = _rad(Moon::meanAnomaly(t));
    const t_longf mD = _rad(Moon::meanElongation(t));
    const t_longf mF = _rad(Moon::meanLatitude(t));    
    const t_longf O  = _rad(Moon::meanOrbitLongitude(t));
   
    // (AA.21.A)

    t_longf Dr = 0.0;

    Dr -= (17.1996 - 0.01742 * T) * sin(                                   + 1 * O);
    Dr -= ( 1.3187 - 0.00016 * T) * sin(-2 * mD                   + 2 * mF + 2 * O);
    Dr -= ( 0.2274 - 0.00002 * T) * sin(                          + 2 * mF + 2 * O);
    Dr += ( 0.2062 + 0.00002 * T) * sin(                                   + 2 * O);
    Dr += ( 0.1426 - 0.00034 * T) * sin(        + 1 * sM                          );
    Dr += ( 0.0712 + 0.00001 * T) * sin(                 + 1 * mM                 );
    Dr += ( 0.0517 + 0.00012 * T) * sin(-2 * mD + 1 * sM          + 2 * mF + 2 * O);
    Dr -= ( 0.0386 - 0.00004 * T) * sin(                          + 2 * mF + 1 * O);
    Dr -= ( 0.0301              ) * sin(                 + 1 * mM + 2 * mF + 2 * O);
    Dr += ( 0.0217 - 0.00005 * T) * sin(-2 * mD - 1 * sM          + 2 * mF + 2 * O);
    Dr -= ( 0.0158              ) * sin(-2 * mD          + 1 * mM                 );
    Dr += ( 0.0129 + 0.00001 * T) * sin(-2 * mD                   + 2 * mF + 1 * O);
    Dr += ( 0.0123              ) * sin(                 - 1 * mM + 2 * mF + 2 * O);
    Dr += ( 0.0063              ) * sin( 2 * mD                                   );
    Dr += ( 0.0063 + 0.00001 * T) * sin(                 + 1 * mM          + 1 * O);
    Dr -= ( 0.0059              ) * sin( 2 * mD          - 1 * mM + 2 * mF + 2 * O);
    Dr -= ( 0.0058 - 0.00001 * T) * sin(                 - 1 * mM          + 1 * O);
    Dr -= ( 0.0051              ) * sin(                 + 1 * mM + 2 * mF + 1 * O);
    Dr += ( 0.0048              ) * sin(-2 * mD          + 2 * mM                 );
    Dr += ( 0.0046              ) * sin(                 - 2 * mM + 2 * mF + 1 * O);
    Dr -= ( 0.0038              ) * sin( 2 * mD                   + 2 * mF + 2 * O);
    Dr -= ( 0.0031              ) * sin(                 + 2 * mM + 2 * mF + 2 * O);
    Dr += ( 0.0029              ) * sin(                 + 2 * mM                 );
    Dr += ( 0.0029              ) * sin( 2 * mD          + 1 * mM + 2 * mF + 2 * O);
    Dr += ( 0.0026              ) * sin(                          + 2 * mF        );
    Dr -= ( 0.0022              ) * sin(-2 * mD                   + 2 * mF        );
    Dr += ( 0.0021              ) * sin(                 - 1 * mM + 2 * mF + 1 * O);
    Dr += ( 0.0017 - 0.00001 * T) * sin(        + 2 * sM                          );
    Dr += ( 0.0016              ) * sin( 2 * mD          - 1 * mM          + 1 * O);
    Dr -= ( 0.0016 + 0.00001 * T) * sin(-2 * mD + 2 * sM          + 2 * mF + 2 * O);
    Dr -= ( 0.0015              ) * sin(        + 1 * sM                   + 1 * O);
    Dr -= ( 0.0013              ) * sin(-2 * mD          + 1 * mM          + 1 * O);
    Dr -= ( 0.0012              ) * sin(        - 1 * sM                   + 1 * O);
    Dr += ( 0.0011              ) * sin(                 + 2 * mM - 2 * mF        );
    Dr -= ( 0.0010              ) * sin( 2 * mD          - 1 * mM + 2 * mF + 1 * O);
    Dr -= ( 0.0008              ) * sin( 2 * mD          + 1 * mM + 2 * mF + 2 * O);
    Dr += ( 0.0007              ) * sin(        + 1 * sM          + 2 * mF + 2 * O);
    Dr += ( 0.0007              ) * sin(-2 * mD + 1 * sM + 1 * mM                 );
    Dr -= ( 0.0007              ) * sin(        - 1 * sM          + 2 * mF + 2 * O);
    Dr -= ( 0.0007              ) * sin( 2 * mD                   + 2 * mF + 1 * O);
    Dr += ( 0.0006              ) * sin( 2 * mD          + 1 * mM                 );
    Dr += ( 0.0006              ) * sin(-2 * mD          + 2 * mM + 2 * mF + 2 * O);
    Dr += ( 0.0006              ) * sin(-2 * mD          + 1 * mM + 2 * mF + 1 * O);
    Dr -= ( 0.0006              ) * sin( 2 * mD          - 2 * mM          + 1 * O);
    Dr -= ( 0.0006              ) * sin( 2 * mD                            + 1 * O);
    Dr += ( 0.0005              ) * sin(        - 1 * sM + 1 * mM                 );
    Dr += ( 0.0005              ) * sin(-2 * mD - 1 * sM          + 2 * mF + 1 * O);
    Dr -= ( 0.0005              ) * sin(-2 * mD                            + 1 * O);
    Dr -= ( 0.0005              ) * sin(                 + 2 * mM + 2 * mF + 1 * O);
    Dr += ( 0.0004              ) * sin(-2 * mD          + 2 * mM          + 1 * O);
    Dr += ( 0.0004              ) * sin(-2 * mD + 1 * sM          + 2 * mF + 1 * O);
    Dr += ( 0.0004              ) * sin(                 + 1 * mM - 2 * mF        );
    Dr -= ( 0.0004              ) * sin(-1 * mD          + 1 * mM                 );
    Dr -= ( 0.0004              ) * sin(-2 * mD + 1 * sM                          );
    Dr -= ( 0.0004              ) * sin( 1 * mD                                   );
    Dr += ( 0.0003              ) * sin(                 + 1 * mM + 2 * mF        );
    Dr -= ( 0.0003              ) * sin(                 - 2 * mM + 2 * mF + 2 * O);
    Dr -= ( 0.0003              ) * sin(-1 * mD - 1 * sM + 1 * mM                 );
    Dr -= ( 0.0003              ) * sin(        + 1 * sM + 1 * mM                 );
    Dr -= ( 0.0003              ) * sin(        - 1 * sM + 1 * mM + 2 * mF + 2 * O);
    Dr -= ( 0.0003              ) * sin( 2 * mD - 1 * sM - 1 * mM + 2 * mF + 2 * O);
    Dr -= ( 0.0003              ) * sin(                 + 3 * mM + 2 * mF + 2 * O);
    Dr -= ( 0.0003              ) * sin( 2 * mD - 1 * sM          + 2 * mF + 2 * O);

    return _decimal(0, 0, Dr);
}


// (AA.21)

const t_longf Earth::obliquityNutation(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const t_longf sM = _rad(Sun::meanAnomaly(t));

    const t_longf mM = _rad(Moon::meanAnomaly(t));
    const t_longf mD = _rad(Moon::meanElongation(t));
    const t_longf mF = _rad(Moon::meanLatitude(t));    
    const t_longf O  = _rad(Moon::meanOrbitLongitude(t));

    // (AA.21.A)

    t_longf De = 0.0;

    De += ( 9.2025 + 0.00089 * T) * cos(                                   + 1 * O);
    De += ( 0.5736 - 0.00031 * T) * cos(-2 * mD                   + 2 * mF + 2 * O);
    De += ( 0.0977 - 0.00005 * T) * cos(                          + 2 * mF + 2 * O);
    De -= ( 0.0895 + 0.00005 * T) * cos(                                   + 2 * O);
    De += ( 0.0054 - 0.00001 * T) * cos(        + 1 * sM                          );
    De -= ( 0.0007              ) * cos(                 + 1 * mM                 );
    De += ( 0.0224 - 0.00006 * T) * cos(-2 * mD + 1 * sM          + 2 * mF + 2 * O);
    De += ( 0.0200              ) * cos(                          + 2 * mF + 1 * O);
    De += ( 0.0129 - 0.00001 * T) * cos(                 + 1 * mM + 2 * mF + 2 * O);
    De -= ( 0.0095 + 0.00003 * T) * cos(-2 * mD - 1 * sM          + 2 * mF + 2 * O);
    De -= ( 0.0070              ) * cos(-2 * mD                   + 2 * mF + 1 * O);
    De -= ( 0.0053              ) * cos(                 - 1 * mM + 2 * mF + 2 * O);
    De -= ( 0.0033              ) * cos(                 + 1 * mM          + 1 * O);
    De += ( 0.0026              ) * cos( 2 * mD          - 1 * mM + 2 * mF + 2 * O);
    De += ( 0.0032              ) * cos(                 - 1 * mM          + 1 * O);
    De += ( 0.0027              ) * cos(                 + 1 * mM + 2 * mF + 1 * O);
    De -= ( 0.0024              ) * cos(                 - 2 * mM + 2 * mF + 1 * O);
    De += ( 0.0016              ) * cos( 2 * mD                   + 2 * mF + 2 * O);
    De += ( 0.0013              ) * cos(                 + 2 * mM + 2 * mF + 2 * O);
    De -= ( 0.0012              ) * cos( 2 * mD          + 1 * mM + 2 * mF + 2 * O);
    De -= ( 0.0010              ) * cos(                 - 1 * mM + 2 * mF + 1 * O);
    De -= ( 0.0008              ) * cos( 2 * mD          - 1 * mM          + 1 * O);
    De += ( 0.0007              ) * cos(-2 * mD + 2 * sM          + 2 * mF + 2 * O);
    De += ( 0.0009              ) * cos(        + 1 * sM                   + 1 * O);
    De += ( 0.0007              ) * cos(-2 * mD          + 1 * mM          + 1 * O);
    De += ( 0.0006              ) * cos(        - 1 * sM                   + 1 * O);
    De += ( 0.0005              ) * cos( 2 * mD          - 1 * mM + 2 * mF + 1 * O);
    De += ( 0.0003              ) * cos( 2 * mD          + 1 * mM + 2 * mF + 2 * O);
    De -= ( 0.0003              ) * cos(        + 1 * sM          + 2 * mF + 2 * O);
    De += ( 0.0003              ) * cos(        - 1 * sM          + 2 * mF + 2 * O);
    De += ( 0.0003              ) * cos( 2 * mD                   + 2 * mF + 1 * O);
    De -= ( 0.0003              ) * cos(-2 * mD          + 2 * mM + 2 * mF + 2 * O);
    De -= ( 0.0003              ) * cos(-2 * mD          + 1 * mM + 2 * mF + 1 * O);
    De += ( 0.0003              ) * cos( 2 * mD          - 2 * mM          + 1 * O);
    De += ( 0.0003              ) * cos( 2 * mD                            + 1 * O);
    De += ( 0.0003              ) * cos(-2 * mD - 1 * sM          + 2 * mF + 1 * O);
    De += ( 0.0003              ) * cos(-2 * mD                            + 1 * O);
    De += ( 0.0003              ) * cos(                 + 2 * mM + 2 * mF + 1 * O);

    return _decimal(0, 0, De);
}


const t_longf Earth::trueObliquity(const t_julianDay t)
{
    return meanObliquity(t) + obliquityNutation(t); // e
}


// Inclination of the Earth's axis of rotation. (AA.21.3)
// By J. Laskar, "Astronomy and Astrophysics" 1986

const t_longf Earth::meanObliquity(const t_julianDay t)
{
    const t_julianDay U = jCenturiesSinceSE(t) * 0.01;

    assert(_abs(U) < 1.0);

    const t_longf e0 = 0.0
        + U * (- 4680.93
        + U * (-    1.55
        + U * (+ 1999.25
        + U * (-   51.38
        + U * (-  249.67
        + U * (-   39.05
        + U * (+    7.12
        + U * (+   27.87
        + U * (+    5.79
        + U * (+    2.45))))))))));

    return _decimal(23, 26, 21.448) + _decimal(0, 0, e0);
}


// This is, if required, approximatelly refraction corrected...

const t_longf Earth::viewDistanceWithinAtmosphere(
    const t_longf y
,   const bool refractionCorrected)
{
    const t_longf t = atmosphereThickness();
    const t_longf r = meanRadius();

    // This works, since dot product of [0, 1, 0] and 
    // eye with [x, y, z] gives y.

    t_longf h = asin(y * (1.0 - 1.e-12)); // correction is required to 
                       // gain t at y = 1.0 - fix for t_longf accuracy.

    if(refractionCorrected)
        h += _rad(atmosphericRefraction(_deg(asin(y))));

    const t_longf cosa = cos(h);
    const t_longf rt = r + t;

    // Using law of sine for arbitrary triangle with two sides and one angle known.
    // Since the angle is (π/2 + a), cos is used instead of sine.

    const t_longf distance = cos(h + asin(cosa * r / rt)) * rt / cosa;

    return distance;
}


// Effect of refraction for true altitudes (AA.15.4).
// G.G. Bennet, "The Calculation of the Astronomical Refraction in marine Navigation", 1982

const t_longf Earth::atmosphericRefraction(const t_longf altitude)
{
    t_longf R = 1.02 / 
        tan(_rad(altitude + 10.3 / (altitude + 5.11))) + 0.0019279;

    return _decimal(0, R, 0); // (since R is in minutes)
}


const t_longf Earth::meanRadius()
{
    // http://nssdc.gsfc.nasa.gov/planetary/factsheet/earthfact.html

    return  6371.0; // in kilometers
}


const t_longf Earth::atmosphereThickness()
{
    // Thickness of atmosphere if the density were uniform.
    
    // 8000 ("Precomputed Atmospheric Scattering" - 2008 - Bruneton, Neyret)
    // 7994 ("Display of the earth taking into account atmospheric scattering" - 1993 - Nishita et al.)
    return 7.994;
}


const t_longf Earth::atmosphereThicknessNonUniform()
{
    // Thickness of atmosphere.
    return 85.0; // ~
}


const t_longf Earth::apparentMagnitudeLimit()
{
    // http://www.astronomynotes.com/starprop/s4.htm
    return 6.5;
}

} // namespace osgHimmel