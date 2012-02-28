
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
#ifndef __GLSL_BRUNETON_INSCATTER_HPP__
#define __GLSL_BRUNETON_INSCATTER_HPP__

#include "bruneton_common.hpp"
#include "common.hpp"

namespace 
{
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

    // computes single scattering (line 3 in algorithm 4.1)

    static const std::string glsl_bruneton_f_inscatter1 // requires: r, dhdH
    (
        glsl_cmn_uniform
    +   glsl_bruneton_const_R
    +   glsl_bruneton_const_M
    +   glsl_bruneton_const_Samples
    +   glsl_bruneton_const_RSize

    +   glsl_bruneton_limit
    +   glsl_bruneton_transmittanceUV
    +   glsl_bruneton_transmittance    
    +   glsl_bruneton_muMuSNu

    +
        "uniform float r;\n"
        "uniform vec4 dhdH;\n"
        "\n"
        "void integrand(float r, float mu, float muS, float nu, float t, out vec3 ray, out vec3 mie) {\n"
        "    ray = vec3(0.0);\n"
        "    mie = vec3(0.0);\n"
        "    float ri = sqrt(r * r + t * t + 2.0 * r * mu * t);\n"
        "    float muSi = (nu * t + muS * r) / ri;\n"
        "    ri = max(cmn[1], ri);\n"
        "    if (muSi >= -sqrt(1.0 - cmn[1] * cmn[1] / (ri * ri))) {\n"
        "        vec3 ti = transmittance(r, mu, t) * transmittance(ri, muSi);\n"
        "        ray = exp(-(ri - cmn[1]) / HR) * ti;\n"
        "        mie = exp(-(ri - cmn[1]) / HM) * ti;\n"
        "    }\n"
        "}\n"
        "\n"
        "void inscatter(float r, float mu, float muS, float nu, out vec3 ray, out vec3 mie) {\n"
        "    ray = vec3(0.0);\n"
        "    mie = vec3(0.0);\n"
        "    float dx = limit(r, mu) / float(INSCATTER_INTEGRAL_SAMPLES);\n"
        "    float xi = 0.0;\n"
        "    vec3 rayi;\n"
        "    vec3 miei;\n"
        "    integrand(r, mu, muS, nu, 0.0, rayi, miei);\n"
        "    for (int i = 1; i <= INSCATTER_INTEGRAL_SAMPLES; ++i) {\n"
        "        float xj = float(i) * dx;\n"
        "        vec3 rayj;\n"
        "        vec3 miej;\n"
        "        integrand(r, mu, muS, nu, xj, rayj, miej);\n"
        "        ray += (rayi + rayj) / 2.0 * dx;\n"
        "        mie += (miei + miej) / 2.0 * dx;\n"
        "        xi = xj;\n"
        "        rayi = rayj;\n"
        "        miei = miej;\n"
        "    }\n"
        "    ray *= betaR;\n"
        "    mie *= betaMSca;\n"
        "}\n"
        "\n"
        "void main() {\n"
        "    vec3 ray;\n"
        "    vec3 mie;\n"
        "    float mu, muS, nu;\n"
        "    getMuMuSNu(r, dhdH, mu, muS, nu);\n"
        "    inscatter(r, mu, muS, nu, ray, mie);\n"
            // store separately Rayleigh and Mie contributions, WITHOUT the phase function factor
            // (cf 'Angular precision')
        "    gl_FragData[0].rgb = ray;\n"
        "    gl_FragData[1].rgb = mie;\n"
        "}\n\n"
    );
    
    
    // computes higher order scattering (line 9 in algorithm 4.1)
  
    static const std::string glsl_bruneton_f_inscatterN // requires: r, dhdH, deltaJSampler, transmittanceSampler
    (
        glsl_bruneton_const_Samples
    +   glsl_cmn_uniform
    +   glsl_bruneton_const_RSize

    +   glsl_bruneton_limit
    +   glsl_bruneton_texture4D
    +   glsl_bruneton_muMuSNu
    +   glsl_bruneton_transmittanceUV
    +   glsl_bruneton_transmittance

    +   
        "uniform float r;\n"
        "uniform vec4 dhdH;\n"
        "\n"
        "uniform sampler3D deltaJSampler;\n"
        "\n"
        "vec3 integrand(float r, float mu, float muS, float nu, float t) {\n"
        "    float ri = sqrt(r * r + t * t + 2.0 * r * mu * t);\n"
        "    float mui = (r * mu + t) / ri;\n"
        "    float muSi = (nu * t + muS * r) / ri;\n"
        "    return texture4D(deltaJSampler, ri, mui, muSi, nu).rgb * transmittance(r, mu, t);\n"
        "}\n"
        "\n"
        "vec3 inscatter(float r, float mu, float muS, float nu) {\n"
        "    vec3 raymie = vec3(0.0);\n"
        "    float dx = limit(r, mu) / float(INSCATTER_INTEGRAL_SAMPLES);\n"
        "    float xi = 0.0;\n"
        "    vec3 raymiei = integrand(r, mu, muS, nu, 0.0);\n"
        "    for (int i = 1; i <= INSCATTER_INTEGRAL_SAMPLES; ++i) {\n"
        "        float xj = float(i) * dx;\n"
        "        vec3 raymiej = integrand(r, mu, muS, nu, xj);\n"
        "        raymie += (raymiei + raymiej) / 2.0 * dx;\n"
        "        xi = xj;\n"
        "        raymiei = raymiej;\n"
        "    }\n"
        "    return raymie;\n"
        "}\n"
        "\n"
        "void main() {\n"
        "    float mu, muS, nu;\n"
        "    getMuMuSNu(r, dhdH, mu, muS, nu);\n"
        "    gl_FragColor.rgb = inscatter(r, mu, muS, nu);\n"
        "}\n\n"
    );


    // computes deltaJ (line 7 in algorithm 4.1)
    
    static const std::string glsl_bruneton_f_inscatterS // requires: r, dhdH, deltaESampler, deltaSRSampler, deltaSMSampler, transmittanceSampler, first
    (
        glsl_bruneton_const_Samples
    +   glsl_cmn_uniform
    +   glsl_bruneton_const_avgReflectance
    +   glsl_bruneton_const_PI
    +   glsl_bruneton_const_R
    +   glsl_bruneton_const_M
    +   glsl_bruneton_const_RSize

    +   glsl_bruneton_muMuSNu
    +   glsl_bruneton_transmittanceUV
    +   glsl_bruneton_transmittance
    +   glsl_bruneton_phaseFunctionM
    +   glsl_bruneton_phaseFunctionR
    +   glsl_bruneton_irradianceUV
    +   glsl_bruneton_irradiance
    +   glsl_bruneton_texture4D

    +
        "uniform float r;\n"
        "uniform vec4 dhdH;\n"
        "\n"
        "uniform sampler2D deltaESampler;\n"
        "uniform sampler3D deltaSRSampler;\n"
        "uniform sampler3D deltaSMSampler;\n"
        "uniform float first;\n"
        "\n"
        "const float dphi = PI / float(INSCATTER_SPHERICAL_INTEGRAL_SAMPLES);\n"
        "const float dtheta = PI / float(INSCATTER_SPHERICAL_INTEGRAL_SAMPLES);\n"
        "\n"
        "void inscatter(float r, float mu, float muS, float nu, out vec3 raymie) {\n"
        "    r = clamp(r, cmn[1], cmn[2]);\n"
        "    mu = clamp(mu, -1.0, 1.0);\n"
        "    muS = clamp(muS, -1.0, 1.0);\n"
        "    float var = sqrt(1.0 - mu * mu) * sqrt(1.0 - muS * muS);\n"
        "    nu = clamp(nu, muS * mu - var, muS * mu + var);\n"
        "\n"
        "    float cthetamin = -sqrt(1.0 - (cmn[1] / r) * (cmn[1] / r));\n"
        "\n"
        "    vec3 v = vec3(sqrt(1.0 - mu * mu), 0.0, mu);\n"
        "    float sx = v.x == 0.0 ? 0.0 : (nu - muS * mu) / v.x;\n"
        "    vec3 s = vec3(sx, sqrt(max(0.0, 1.0 - sx * sx - muS * muS)), muS);\n"
        "\n"
        "    raymie = vec3(0.0);\n"
        "\n"
            // integral over 4.PI around x with two nested loops over w directions (theta,phi) -- Eq (7)
        "    for (int itheta = 0; itheta < INSCATTER_SPHERICAL_INTEGRAL_SAMPLES; ++itheta) {\n"
        "        float theta = (float(itheta) + 0.5) * dtheta;\n"
        "        float ctheta = cos(theta);\n"
        "\n"
        "        float greflectance = 0.0;\n"
        "        float dground = 0.0;\n"
        "        vec3 gtransp = vec3(0.0);\n"
        "        if (ctheta < cthetamin) {\n" // if ground visible in direction w
                    // compute transparency gtransp between x and ground
        "            greflectance = AVERAGE_GROUND_REFLECTANCE / PI;\n"
        "            dground = -r * ctheta - sqrt(r * r * (ctheta * ctheta - 1.0) + cmn[1] * cmn[1]);\n"
        "            gtransp = transmittance(cmn[1], -(r * ctheta + dground) / cmn[1], dground);\n"
        "        }\n"
        "\n"
        "        for (int iphi = 0; iphi < 2 * INSCATTER_SPHERICAL_INTEGRAL_SAMPLES; ++iphi) {\n"
        "            float phi = (float(iphi) + 0.5) * dphi;\n"
        "            float dw = dtheta * dphi * sin(theta);\n"
        "            vec3 w = vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), ctheta);\n"
        "\n"
        "            float nu1 = dot(s, w);\n"
        "            float nu2 = dot(v, w);\n"
        "            float pr2 = phaseFunctionR(nu2);\n"
        "            float pm2 = phaseFunctionM(nu2);\n"
        "\n"
                    // compute irradiance received at ground in direction w (if ground visible) =deltaE
        "            vec3 gnormal = (vec3(0.0, 0.0, r) + dground * w) / cmn[1];\n"
        "            vec3 girradiance = irradiance(deltaESampler, cmn[1], dot(gnormal, s));\n"
        "\n"
        "            vec3 raymie1; // light arriving at x from direction w\n"
        "\n"
                    // first term = light reflected from the ground and attenuated before reaching x, =T.alpha/PI.deltaE
        "            raymie1 = greflectance * girradiance * gtransp;\n"
        "\n"
                    // second term = inscattered light, =deltaS
        "            if (first == 1.0) {\n"
                        // first iteration is special because Rayleigh and Mie were stored separately,
                        // without the phase functions factors; they must be reintroduced here
        "                float pr1 = phaseFunctionR(nu1);\n"
        "                float pm1 = phaseFunctionM(nu1);\n"
        "                vec3 ray1 = texture4D(deltaSRSampler, r, w.z, muS, nu1).rgb;\n"
        "                vec3 mie1 = texture4D(deltaSMSampler, r, w.z, muS, nu1).rgb;\n"
        "                raymie1 += ray1 * pr1 + mie1 * pm1;\n"
        "            } else {\n"
        "                raymie1 += texture4D(deltaSRSampler, r, w.z, muS, nu1).rgb;\n"
        "            }\n"
        "\n"
                    // light coming from direction w and scattered in direction v
                    // = light arriving at x from direction w (raymie1) * SUM(scattering coefficient * phaseFunction)
                    // see Eq (7)
        "            raymie += raymie1 * (betaR * exp(-(r - cmn[1]) / HR) * pr2 + betaMSca * exp(-(r - cmn[1]) / HM) * pm2) * dw;\n"
        "        }\n"
        "    }\n"
        "\n"
            // output raymie = J[T.alpha / PI.deltaE + deltaS] (line 7 in algorithm 4.1)
        "}\n"
        "\n"
        "void main() {\n"
        "    vec3 raymie;\n"
        "    float mu, muS, nu;\n"
        "    getMuMuSNu(r, dhdH, mu, muS, nu);\n"
        "    inscatter(r, mu, muS, nu, raymie);\n"
        "    gl_FragColor.rgb = raymie;\n"
        "}\n\n"
    );




    // copies deltaS into S (line 5 in algorithm 4.1)

    static const std::string glsl_bruneton_f_copyInscatter1 // requires: deltaSRSampler, deltaSMSampler, layer
    (
        glsl_bruneton_const_RSize

    +
        "uniform sampler3D deltaSRSampler;\n"
        "uniform sampler3D deltaSMSampler;\n"
        "\n"
        "uniform int layer;\n"
        "\n"
        "void main() {\n"
        "    vec3 uvw = vec3(gl_FragCoord.xy, float(layer) + 0.5) / vec3(ivec3(RES_MU_S * RES_NU, RES_MU, RES_R));\n"
        "    vec4 ray = texture3D(deltaSRSampler, uvw);\n"
        "    vec4 mie = texture3D(deltaSMSampler, uvw);\n"
        "    gl_FragColor = vec4(ray.rgb, mie.r);\n" // store only red component of single Mie scattering (cf. 'Angular precision')
        "}\n\n"
    );


    // adds deltaS into S (line 11 in algorithm 4.1)

    static const std::string glsl_bruneton_f_copyInscatterN // requires: r, dhdH, layer, deltaSRSampler, inscatterSampler
    (
        glsl_bruneton_const_RSize
    +   glsl_cmn_uniform
    +   glsl_bruneton_const_PI
    
    +   glsl_bruneton_muMuSNu
    +   glsl_bruneton_phaseFunctionR

    +
        "uniform float r;\n"
        "uniform vec4 dhdH;\n"
        "uniform int layer;\n"
        "\n"
        "uniform sampler3D deltaSRSampler;\n"
        "uniform sampler3D inscatterSampler;\n"
        "\n"
        "void main() {\n"
        "    float mu, muS, nu;\n"
        "    getMuMuSNu(r, dhdH, mu, muS, nu);\n"
        "    vec3 uvw = vec3(gl_FragCoord.xy, float(layer) + 0.5) / vec3(ivec3(RES_MU_S * RES_NU, RES_MU, RES_R));\n"
        "    gl_FragColor  = texture3D(inscatterSampler, uvw);\n"
        "    gl_FragColor += vec4(texture3D(deltaSRSampler, uvw).rgb / phaseFunctionR(nu), 0.0);\n"
        "}\n\n"
    );
}

#endif // __GLSL_BRUNETON_INSCATTER_HPP__