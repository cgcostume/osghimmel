
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

// The linear eccentricity of the earth orbit is about 2.5 * 10^6 km.
// Compared to the avg. distance of 149.6 * 10^6 km this is not much.
// http://www.greier-greiner.at/hc/ekliptik.htm

// P. Bretagnon, "Théorie du mouvement de l'ensamble des planètes. Solution VSOP82", 1982

const long double earth_orbitEccentricity(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const long double E = 0.01670862
        + T * (- 0.000042037
        + T * (- 0.0000001236
        + T * (+ 0.00000000004)));

    // (AA.24.4)
    //const long double E = 0.016708617
    //    + T * (- 0.000042037
    //    + T * (- 0.0000001236));

    return _revd(E);
}


const long double earth_apparentAngularSunDiameter(const t_julianDay t)
{
    return _adiameter(sun_distance(t), sun_meanRadius());
}


const long double earth_apparentAngularMoonDiameter(const t_julianDay t)
{
    return _adiameter(moon_distance(t), moon_meanRadius());
}


const long double earth_longitudeNutation(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const long double sM = _rad(sun_meanAnomaly(t));

    const long double mM = _rad(moon_meanAnomaly(t));
    const long double mD = _rad(moon_meanElongation(t));
    const long double mF = _rad(moon_meanLatitude(t));    
    const long double Ω  = _rad(moon_meanOrbitLongitude(t));
   
    // (AA.21.A)

    long double Δψ = 0.0;

    Δψ -= (17.1996 - 0.01742 * T) * sin(                                   + 1 * Ω);
    Δψ -= ( 1.3187 - 0.00016 * T) * sin(-2 * mD                   + 2 * mF + 2 * Ω);
    Δψ -= ( 0.2274 - 0.00002 * T) * sin(                          + 2 * mF + 2 * Ω);
    Δψ += ( 0.2062 + 0.00002 * T) * sin(                                   + 2 * Ω);
    Δψ += ( 0.1426 - 0.00034 * T) * sin(        + 1 * sM                          );
    Δψ += ( 0.0712 + 0.00001 * T) * sin(                 + 1 * mM                 );
    Δψ += ( 0.0517 + 0.00012 * T) * sin(-2 * mD + 1 * sM          + 2 * mF + 2 * Ω);
    Δψ -= ( 0.0386 - 0.00004 * T) * sin(                          + 2 * mF + 1 * Ω);
    Δψ -= ( 0.0301              ) * sin(                 + 1 * mM + 2 * mF + 2 * Ω);
    Δψ += ( 0.0217 - 0.00005 * T) * sin(-2 * mD - 1 * sM          + 2 * mF + 2 * Ω);
    Δψ -= ( 0.0158              ) * sin(-2 * mD          + 1 * mM                 );
    Δψ += ( 0.0129 + 0.00001 * T) * sin(-2 * mD                   + 2 * mF + 1 * Ω);
    Δψ += ( 0.0123              ) * sin(                 - 1 * mM + 2 * mF + 2 * Ω);
    Δψ += ( 0.0063              ) * sin( 2 * mD                                   );
    Δψ += ( 0.0063 + 0.00001 * T) * sin(                 + 1 * mM          + 1 * Ω);
    Δψ -= ( 0.0059              ) * sin( 2 * mD          - 1 * mM + 2 * mF + 2 * Ω);
    Δψ -= ( 0.0058 - 0.00001 * T) * sin(                 - 1 * mM          + 1 * Ω);
    Δψ -= ( 0.0051              ) * sin(                 + 1 * mM + 2 * mF + 1 * Ω);
    Δψ += ( 0.0048              ) * sin(-2 * mD          + 2 * mM                 );
    Δψ += ( 0.0046              ) * sin(                 - 2 * mM + 2 * mF + 1 * Ω);
    Δψ -= ( 0.0038              ) * sin( 2 * mD                   + 2 * mF + 2 * Ω);
    Δψ -= ( 0.0031              ) * sin(                 + 2 * mM + 2 * mF + 2 * Ω);
    Δψ += ( 0.0029              ) * sin(                 + 2 * mM                 );
    Δψ += ( 0.0029              ) * sin( 2 * mD          + 1 * mM + 2 * mF + 2 * Ω);
    Δψ += ( 0.0026              ) * sin(                          + 2 * mF        );
    Δψ -= ( 0.0022              ) * sin(-2 * mD                   + 2 * mF        );
    Δψ += ( 0.0021              ) * sin(                 - 1 * mM + 2 * mF + 1 * Ω);
    Δψ += ( 0.0017 - 0.00001 * T) * sin(        + 2 * sM                          );
    Δψ += ( 0.0016              ) * sin( 2 * mD          - 1 * mM          + 1 * Ω);
    Δψ -= ( 0.0016 + 0.00001 * T) * sin(-2 * mD + 2 * sM          + 2 * mF + 2 * Ω);
    Δψ -= ( 0.0015              ) * sin(        + 1 * sM                   + 1 * Ω);
    Δψ -= ( 0.0013              ) * sin(-2 * mD          + 1 * mM          + 1 * Ω);
    Δψ -= ( 0.0012              ) * sin(        - 1 * sM                   + 1 * Ω);
    Δψ += ( 0.0011              ) * sin(                 + 2 * mM - 2 * mF        );
    Δψ -= ( 0.0010              ) * sin( 2 * mD          - 1 * mM + 2 * mF + 1 * Ω);
    Δψ -= ( 0.0008              ) * sin( 2 * mD          + 1 * mM + 2 * mF + 2 * Ω);
    Δψ += ( 0.0007              ) * sin(        + 1 * sM          + 2 * mF + 2 * Ω);
    Δψ += ( 0.0007              ) * sin(-2 * mD + 1 * sM + 1 * mM                 );
    Δψ -= ( 0.0007              ) * sin(        - 1 * sM          + 2 * mF + 2 * Ω);
    Δψ -= ( 0.0007              ) * sin( 2 * mD                   + 2 * mF + 1 * Ω);
    Δψ += ( 0.0006              ) * sin( 2 * mD          + 1 * mM                 );
    Δψ += ( 0.0006              ) * sin(-2 * mD          + 2 * mM + 2 * mF + 2 * Ω);
    Δψ += ( 0.0006              ) * sin(-2 * mD          + 1 * mM + 2 * mF + 1 * Ω);
    Δψ -= ( 0.0006              ) * sin( 2 * mD          - 2 * mM          + 1 * Ω);
    Δψ -= ( 0.0006              ) * sin( 2 * mD                            + 1 * Ω);
    Δψ += ( 0.0005              ) * sin(        - 1 * sM + 1 * mM                 );
    Δψ += ( 0.0005              ) * sin(-2 * mD - 1 * sM          + 2 * mF + 1 * Ω);
    Δψ -= ( 0.0005              ) * sin(-2 * mD                            + 1 * Ω);
    Δψ -= ( 0.0005              ) * sin(                 + 2 * mM + 2 * mF + 1 * Ω);
    Δψ += ( 0.0004              ) * sin(-2 * mD          + 2 * mM          + 1 * Ω);
    Δψ += ( 0.0004              ) * sin(-2 * mD + 1 * sM          + 2 * mF + 1 * Ω);
    Δψ += ( 0.0004              ) * sin(                 + 1 * mM - 2 * mF        );
    Δψ -= ( 0.0004              ) * sin(-1 * mD          + 1 * mM                 );
    Δψ -= ( 0.0004              ) * sin(-2 * mD + 1 * sM                          );
    Δψ -= ( 0.0004              ) * sin( 1 * mD                                   );
    Δψ += ( 0.0003              ) * sin(                 + 1 * mM + 2 * mF        );
    Δψ -= ( 0.0003              ) * sin(                 - 2 * mM + 2 * mF + 2 * Ω);
    Δψ -= ( 0.0003              ) * sin(-1 * mD - 1 * sM + 1 * mM                 );
    Δψ -= ( 0.0003              ) * sin(        + 1 * sM + 1 * mM                 );
    Δψ -= ( 0.0003              ) * sin(        - 1 * sM + 1 * mM + 2 * mF + 2 * Ω);
    Δψ -= ( 0.0003              ) * sin( 2 * mD - 1 * sM - 1 * mM + 2 * mF + 2 * Ω);
    Δψ -= ( 0.0003              ) * sin(                 + 3 * mM + 2 * mF + 2 * Ω);
    Δψ -= ( 0.0003              ) * sin( 2 * mD - 1 * sM          + 2 * mF + 2 * Ω);

    return _decimal(0, 0, Δψ);
}


// (AA.21)

const long double earth_obliquityNutation(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const long double sM = _rad(sun_meanAnomaly(t));

    const long double mM = _rad(moon_meanAnomaly(t));
    const long double mD = _rad(moon_meanElongation(t));
    const long double mF = _rad(moon_meanLatitude(t));    
    const long double Ω  = _rad(moon_meanOrbitLongitude(t));

    // (AA.21.A)

    long double Δε = 0.0;

    Δε += ( 9.2025 + 0.00089 * T) * cos(                                   + 1 * Ω);
    Δε += ( 0.5736 - 0.00031 * T) * cos(-2 * mD                   + 2 * mF + 2 * Ω);
    Δε += ( 0.0977 - 0.00005 * T) * cos(                          + 2 * mF + 2 * Ω);
    Δε -= ( 0.0895 + 0.00005 * T) * cos(                                   + 2 * Ω);
    Δε += ( 0.0054 - 0.00001 * T) * cos(        + 1 * sM                          );
    Δε -= ( 0.0007              ) * cos(                 + 1 * mM                 );
    Δε += ( 0.0224 - 0.00006 * T) * cos(-2 * mD + 1 * sM          + 2 * mF + 2 * Ω);
    Δε += ( 0.0200              ) * cos(                          + 2 * mF + 1 * Ω);
    Δε += ( 0.0129 - 0.00001 * T) * cos(                 + 1 * mM + 2 * mF + 2 * Ω);
    Δε -= ( 0.0095 + 0.00003 * T) * cos(-2 * mD - 1 * sM          + 2 * mF + 2 * Ω);
    Δε -= ( 0.0070              ) * cos(-2 * mD                   + 2 * mF + 1 * Ω);
    Δε -= ( 0.0053              ) * cos(                 - 1 * mM + 2 * mF + 2 * Ω);
    Δε -= ( 0.0033              ) * cos(                 + 1 * mM          + 1 * Ω);
    Δε += ( 0.0026              ) * cos( 2 * mD          - 1 * mM + 2 * mF + 2 * Ω);
    Δε += ( 0.0032              ) * cos(                 - 1 * mM          + 1 * Ω);
    Δε += ( 0.0027              ) * cos(                 + 1 * mM + 2 * mF + 1 * Ω);
    Δε -= ( 0.0024              ) * cos(                 - 2 * mM + 2 * mF + 1 * Ω);
    Δε += ( 0.0016              ) * cos( 2 * mD                   + 2 * mF + 2 * Ω);
    Δε += ( 0.0013              ) * cos(                 + 2 * mM + 2 * mF + 2 * Ω);
    Δε -= ( 0.0012              ) * cos( 2 * mD          + 1 * mM + 2 * mF + 2 * Ω);
    Δε -= ( 0.0010              ) * cos(                 - 1 * mM + 2 * mF + 1 * Ω);
    Δε -= ( 0.0008              ) * cos( 2 * mD          - 1 * mM          + 1 * Ω);
    Δε += ( 0.0007              ) * cos(-2 * mD + 2 * sM          + 2 * mF + 2 * Ω);
    Δε += ( 0.0009              ) * cos(        + 1 * sM                   + 1 * Ω);
    Δε += ( 0.0007              ) * cos(-2 * mD          + 1 * mM          + 1 * Ω);
    Δε += ( 0.0006              ) * cos(        - 1 * sM                   + 1 * Ω);
    Δε += ( 0.0005              ) * cos( 2 * mD          - 1 * mM + 2 * mF + 1 * Ω);
    Δε += ( 0.0003              ) * cos( 2 * mD          + 1 * mM + 2 * mF + 2 * Ω);
    Δε -= ( 0.0003              ) * cos(        + 1 * sM          + 2 * mF + 2 * Ω);
    Δε += ( 0.0003              ) * cos(        - 1 * sM          + 2 * mF + 2 * Ω);
    Δε += ( 0.0003              ) * cos( 2 * mD                   + 2 * mF + 1 * Ω);
    Δε -= ( 0.0003              ) * cos(-2 * mD          + 2 * mM + 2 * mF + 2 * Ω);
    Δε -= ( 0.0003              ) * cos(-2 * mD          + 1 * mM + 2 * mF + 1 * Ω);
    Δε += ( 0.0003              ) * cos( 2 * mD          - 2 * mM          + 1 * Ω);
    Δε += ( 0.0003              ) * cos( 2 * mD                            + 1 * Ω);
    Δε += ( 0.0003              ) * cos(-2 * mD - 1 * sM          + 2 * mF + 1 * Ω);
    Δε += ( 0.0003              ) * cos(-2 * mD                            + 1 * Ω);
    Δε += ( 0.0003              ) * cos(                 + 2 * mM + 2 * mF + 1 * Ω);

    return _decimal(0, 0, Δε);
}


const long double earth_trueObliquity(const t_julianDay t)
{
    return earth_meanObliquity(t) + earth_obliquityNutation(t); // ε
}


// Inclination of the Earth's axis of rotation. (AA.21.3)
// By J. Laskar, "Astronomy and Astrophysics" 1986

const long double earth_meanObliquity(const t_julianDay t)
{
    const t_julianDay U = jCenturiesSinceSE(t) * 0.01;

    assert(_abs(U) < 1.0);

    const long double ε0 = 0.0
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

    return _decimal(23, 26, 21.448) + _decimal(0, 0, ε0);
}


// This is, if required, approximatelly refraction corrected...

const long double earth_viewDistanceWithinAtmosphere(
    const long double y
,   const bool refractionCorrected)
{
    const long double t = earth_atmosphereThickness();
    const long double r = earth_meanRadius();

    // This works, since dot product of [0, 1, 0] and 
    // eye with [x, y, z] gives y.

    long double h = asin(y * (1.0 - 1.e-12)); // correction is required to 
                       // gain t at y = 1.0 - fix for long double accuracy.

    if(refractionCorrected)
        h += _rad(earth_atmosphericRefraction(_deg(asin(y))));

    const long double cosa = cos(h);
    const long double rt = r + t;

    // Using law of sine for arbitrary triangle with two sides and one angle known.
    // Since the angle is (π/2 + a), cos is used instead of sine.

    const long double distance = cos(h + asin(cosa * r / rt)) * rt / cosa;

    return distance;
}


// Effect of refraction for true altitudes (AA.15.4).
// G.G. Bennet, "The Calculation of the Astronomical Refraction in marine Navigation", 1982

const long double earth_atmosphericRefraction(const long double altitude)
{
    long double R = 1.02 / 
        tan(_rad(altitude + 10.3 / (altitude + 5.11))) + 0.0019279;

    return _decimal(0, R, 0); // (since R is in minutes)
}


const long double earth_meanRadius()
{
    // http://nssdc.gsfc.nasa.gov/planetary/factsheet/earthfact.html

    return  6371.0; // in kilometers
}


const long double earth_atmosphereThickness()
{
    // Thickness of atmosphere if the density were uniform.
    
    // 8000 ("Precomputed Atmospheric Scattering" - 2008 - Bruneton, Neyret)
    // 7994 ("Display of the earth taking into account atmospheric scattering" - 1993 - Nishita et al.)
    return 7.994;
}


const long double earth_apparentMagnitudeLimit()
{
    // http://www.astronomynotes.com/starprop/s4.htm
    return 6.5;
}


// LOW ACCURACY

const float earth_orbitEccentricity_la(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    // Low Accuracy (http://nssdc.gsfc.nasa.gov/planetary/factsheet/earthfact.html)
    const float E = 0.01671022;

    return _revd(E);
}


const float earth_apparentAngularSunDiameter_la(const t_julianDay t)
{
    return _adiameter(sun_distance_la(t), sun_meanRadius());
}


const float earth_apparentAngularMoonDiameter_la(const t_julianDay t)
{
    return _adiameter(moon_distance_la(t), moon_meanRadius());
}


const float earth_longitudeNutation_la(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const float sM = _rad(sun_meanAnomaly_la(t));
    const float mM = _rad(moon_meanAnomaly_la(t));
    const float Ω  = _rad(moon_meanOrbitLongitude_la(t));

    // (AA.21 p132)

    const float ψ =
        - _decimal(0, 0, 17.20) * sin(Ω)
        - _decimal(0, 0,  1.32) * sin(2.0 * sM)
        - _decimal(0, 0,  0.23) * sin(2.0 * mM)
        + _decimal(0, 0,  0.21) * sin(2.0 * Ω );

    return ψ;
}


const float earth_obliquityNutation_la(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    const float Ω  = _rad(moon_meanOrbitLongitude_la(t));
    const float Ls = _rad(sun_meanAnomaly_la(t));
    const float Lm = _rad(moon_meanAnomaly_la(t));

    // (AA.21 p132)
    const float ε =
        + _decimal(0, 0,  9.20) * cos(Ω)
        + _decimal(0, 0,  0.57) * cos(2.0 * Ls)
        + _decimal(0, 0,  0.10) * cos(2.0 * Lm)
        - _decimal(0, 0,  0.09) * cos(2.0 * Ω );

    return ε;
}


const float earth_trueObliquity_la(const t_julianDay t)
{
    return earth_meanObliquity_la(t) + earth_obliquityNutation_la(t); // ε
}


const float earth_meanObliquity_la(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    // ("A Physically-Based Night Sky Model" - 2001 - Wann Jensen et al.)
    const float ε0 = _deg(0.409093 - 0.000227 * T);

    // Low Accuracy (AA.21.2)
    //const float ε0 = _decimal(23, 26, 21.448) 
    //    + T * (- _decimal(0, 0, 46.8150)
    //    + T * (- _decimal(0, 0,  0.00059)
    //    + T * (+ _decimal(0, 0,  0.001813))));

    return ε0;
}


// This is not refraction corrected.

const float earth_viewDistanceWithinAtmosphere_la(
    const float y)
{
    const float t = earth_atmosphereThickness();
    const float r = earth_meanRadius();

    // This works only for mean radius of earth.
    return t * 1116.0 / ((y + 0.004) * 1.1116);
}