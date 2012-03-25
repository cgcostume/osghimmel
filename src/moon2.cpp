
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

#include "moon2.h"

#include "moon.h"
#include "sun2.h"
#include "earth2.h"
#include "sideraltime.h"
#include "mathmacros.h"

#include <assert.h>


namespace osgHimmel
{

// LOW ACCURACY

const float Moon2::meanLongitude(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    // ("A Physically-Based Night Sky Model" - 2001 - Wann Jensen et al.)
    const float L0 = _deg(3.8104 + 8399.7091 * T);

    // (AA.21 p132)
    //const float L0 = 218.3165
    //    + T * (+  481267.8813);

    // (http://www.jgiesen.de/moonmotion/index.html)
    //const float L0 = 218.31617f 
    //    + T * (+ 481267.88088
    //    + T * (-      0.00112778));

    return _revd(L0);
}


const float Moon2::meanElongation(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    // ("A Physically-Based Night Sky Model" - 2001 - Wann Jensen et al.)
    const float D = _deg(5.1985 + 7771.3772 * T);

    return _revd(D);
}


const float Moon2::meanAnomaly(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    // ("A Physically-Based Night Sky Model" - 2001 - Wann Jensen et al.)
    const float M = _deg(2.3554 + 8328.6911 * T);

    // (AA.21...)
    //const float M = 134.96298 
    //    + T * (+ 477198.867398
    //    + T * (+      0.0086972
    //    + T * (+ 1.0 / 56250.0)));

    // (http://www.jgiesen.de/moonmotion/index.html)
    //const float M = 134.96292 
    //    + T * (+ 477198.86753
    //    + T * (+      0.00923611));

    return _revd(M);
}


const float Moon2::meanLatitude(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    // ("A Physically-Based Night Sky Model" - 2001 - Wann Jensen et al.)
    const float F = _deg(1.6280 + 8433.4663 * T);

    return _revd(F);
}


const float Moon2::meanOrbitLongitude(const t_julianDay t)
{
    const t_julianDay T(jCenturiesSinceSE(t));

    // (AA p152)
    const float O = 125.04
        + T * (- 1934.136);

    return _revd(O);
}


const t_eclf Moon2::position(const t_julianDay t)
{
    const float sM = _rad(Sun2::meanAnomaly(t));

    const float mL = _rad(meanLongitude(t));
    const float mM = _rad(meanAnomaly(t));
    const float mD = _rad(meanElongation(t));
    const float mF = _rad(meanLatitude(t));

    // ("A Physically-Based Night Sky Model" - 2001 - Wann Jensen et al.)

    float Sl = mL;

    Sl += 0.1098 * sin(                 + 1 * mM         );
    Sl += 0.0222 * sin( 2 * mD          - 1 * mM         );
    Sl += 0.0115 * sin( 2 * mD                           );
    Sl += 0.0037 * sin(                 + 2 * mM         );
    Sl -= 0.0032 * sin(        + 1 * sM                  );
    Sl -= 0.0020 * sin(                          + 2 * mF);
    Sl += 0.0010 * sin( 2 * mD          - 2 * mM         );
    Sl += 0.0010 * sin( 2 * mD - 1 * sM - 1 * mM         );
    Sl += 0.0009 * sin( 2 * mD          + 1 * mM         );
    Sl += 0.0008 * sin( 2 * mD - 1 * sM                  );
    Sl -= 0.0007 * sin(        + 1 * sM - 1 * mM         );
    Sl -= 0.0006 * sin( 1 * mD                           );
    Sl -= 0.0005 * sin(        + 1 * sM + 1 * mM         );

    float Sb = 0.0;

    Sb += 0.0895 * sin(                          + 1 * mF);
    Sb += 0.0049 * sin(                 + 1 * mM + 1 * mF);
    Sb += 0.0048 * sin(                 + 1 * mM - 1 * mF);
    Sb += 0.0030 * sin( 2 * mD                   - 1 * mF);
    Sb += 0.0010 * sin( 2 * mD          - 1 * mM + 1 * mF);
    Sb += 0.0008 * sin( 2 * mD          - 1 * mM - 1 * mF);
    Sb += 0.0006 * sin( 2 * mD                   + 1 * mF);

    t_eclf ecl;

    ecl.longitude = _deg(Sl);
    ecl.latitude = _deg(Sb);

    return ecl;
}


const t_equf Moon2::apparentPosition(const t_julianDay t)
{
    t_eclf ecl = position(t);

    return ecl.toEquatorial(Earth2::trueObliquity(t));
}


const t_horf Moon2::horizontalPosition(
    const t_aTime &aTime
,   const float latitude
,   const float longitude)
{
    t_julianDay t(jd(aTime));
    t_julianDay s(siderealTime(aTime));

    t_equf equ = apparentPosition(t);

    return equ.toHorizontal(s, latitude, longitude);
}


// NOTE: This gives the distance from the center of the moon to the
// center of the earth. 

const float Moon2::distance(const t_julianDay t)
{
    const float sM = _rad(Sun2::meanAnomaly(t));

    const float mM = _rad(meanAnomaly(t));
    const float mD = _rad(meanElongation(t));

    // ("A Physically-Based Night Sky Model" - 2001 - Wann Jensen et al.)

    float Sr = 0.016593;

    Sr += 0.000904 * cos(                 + 1 * mM);
    Sr += 0.000166 * cos( 2 * mD          - 1 * mM);
    Sr += 0.000137 * cos( 2 * mD                  );
    Sr += 0.000049 * cos(                 + 2 * mM);
    Sr += 0.000015 * cos( 2 * mD          + 1 * mM);
    Sr += 0.000009 * cos( 2 * mD - 1 * sM         );

    return Earth2::meanRadius() / Sr;
}


const t_longf Moon2::meanRadius()
{
    return Moon::meanRadius();
}

} // namespace osgHimmel