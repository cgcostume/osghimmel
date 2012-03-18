
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

#include "bruneton_transmittance.h"

#include "bruneton_common.h"
#include "common.h"
#include "pragma_once.h"

// based on Brunetons free code (http://www-evasion.imag.fr/Members/Eric.Bruneton/PrecomputedAtmosphericScattering2.zip)

/**
    * Precomputed Atmospheric Scattering
    * Copyright (c) 2008 INRIA
    * All rights reserved.
    *
    * Redistribution and use in source and binary forms, with or without
    * modification, are permitted provided that the following conditions
    * are met:
    * 1. Redistributions of source code must retain the above copyright
    *    notice, this list of conditions and the following disclaimer.
    * 2. Redistributions in binary form must reproduce the above copyright
    *    notice, this list of conditions and the following disclaimer in the
    *    documentation and/or other materials provided with the distribution.
    * 3. Neither the name of the copyright holders nor the names of its
    *    contributors may be used to endorse or promote products derived from
    *    this software without specific prior written permission.
    *
    * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
    * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
    * THE POSSIBILITY OF SUCH DAMAGE.
    */

/**
    * Author: Eric Bruneton
    */

// computes transmittance table T using Eq (5)

const std::string glsl_bruneton_f_transmittance
(
    glsl_bruneton_const_Samples
+   glsl_cmn_uniform
+   glsl_bruneton_const_R
+   glsl_bruneton_const_M
+   glsl_bruneton_const_TSize

+   glsl_bruneton_limit
+   glsl_bruneton_transmittanceRMu

+   PRAGMA_ONCE(main,
    
    "float opticalDepth(float H, float r, float mu) {\n"
    "    float result = 0.0;\n"
    "    float dx = limit(r, mu) / float(TRANSMITTANCE_INTEGRAL_SAMPLES);\n"
    "    float xi = 0.0;\n"
    "    float yi = exp(-(r - cmn[1]) / H);\n"
    "    for (int i = 1; i <= TRANSMITTANCE_INTEGRAL_SAMPLES; ++i) {\n"
    "        float xj = float(i) * dx;\n"
    "        float yj = exp(-(sqrt(r * r + xj * xj + 2.0 * xj * r * mu) - cmn[1]) / H);\n"
    "        result += (yi + yj) / 2.0 * dx;\n"
    "        xi = xj;\n"
    "        yi = yj;\n"
    "    }\n"
    "    return mu < -sqrt(1.0 - (cmn[1] / r) * (cmn[1] / r)) ? 1e9 : result;\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    float r, muS;\n"
    "    getTransmittanceRMu(r, muS);\n"
    "    vec3 depth = betaR * opticalDepth(HR, r, muS) + betaMEx * opticalDepth(HM, r, muS);\n"
    "    gl_FragColor = vec4(exp(-depth), 0.0);\n" // Eq (5)
    "}")
);