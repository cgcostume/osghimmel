
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
#ifndef __GLSL_BRUNETON_COMMON_HPP__
#define __GLSL_BRUNETON_COMMON_HPP__

#include <string>

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

    static const std::string glsl_bruneton_v_default
    (
        "void main() {\n"
        "    gl_Position = gl_Vertex;\n"
        "}\n\n"
    );


    //static const std::string glsl_bruneton_g_default
    //(
    //    "#version 150 compatibility\n\n"
    //    "#extension GL_EXT_geometry_shader4 : enable\n"
    //    "\n"
    //    "layout(triangles) in;\n"
    //    "layout(triangle_strip, max_vertices = 3) out;\n"
    //    "\n"
    //    "uniform int layer;\n"
    //    "\n"
    //    "void main() {\n"
    //    "    gl_Position = gl_in[0].gl_Position;\n"
    //    "    gl_Layer = layer;\n"
    //    "    EmitVertex();\n"
    //    "    gl_Position = gl_in[1].gl_Position;\n"
    //    "    gl_Layer = layer;\n"
    //    "    EmitVertex();\n"
    //    "    gl_Position = gl_in[2].gl_Position;\n"
    //    "    gl_Layer = layer;\n"
    //    "    EmitVertex();\n"
    //    "    EndPrimitive();\n"
    //    "}\n\n"
    //);


    // TEXTURE SIZE
    
    static const std::string glsl_bruneton_const_TSize
    (
        "const int TRANSMITTANCE_W = %TRANSMITTANCE_W%; \n"
        "const int TRANSMITTANCE_H = %TRANSMITTANCE_H%; \n\n"
    );


    static const std::string glsl_bruneton_const_ISize
    (
        "const int SKY_W = %SKY_W%; \n"
        "const int SKY_H = %SKY_H%; \n\n"
    );

    
    static const std::string glsl_bruneton_const_RSize
    (
        "const int RES_R    = %RES_R%; \n"
        "const int RES_MU   = %RES_MU%; \n"
        "const int RES_MU_S = %RES_MU_S%; \n"
        "const int RES_NU  =  %RES_NU%; \n\n"
    );


    // NUMERICAL INTEGRATION PARAMETERS

    static const std::string glsl_bruneton_const_Samples
    (
        "const int TRANSMITTANCE_INTEGRAL_SAMPLES       = %TRANSMITTANCE_INTEGRAL_SAMPLES%; \n"
        "const int INSCATTER_INTEGRAL_SAMPLES           = %INSCATTER_INTEGRAL_SAMPLES%; \n"
        "const int IRRADIANCE_INTEGRAL_SAMPLES          = %IRRADIANCE_INTEGRAL_SAMPLES%; \n"
        "const int INSCATTER_SPHERICAL_INTEGRAL_SAMPLES = %INSCATTER_SPHERICAL_INTEGRAL_SAMPLES%; \n\n"
    );


    static const std::string glsl_bruneton_const_PI
    (
        "const float M_PI = 3.1415926535897932384626433832795; \n\n"
    );


    // PHYSICAL MODEL PARAMETERS

    static const std::string glsl_bruneton_const_avgReflectance
    (
        "const float AVERAGE_GROUND_REFLECTANCE = %AVERAGE_GROUND_REFLECTANCE%; \n\n"
    );

    
    static const std::string glsl_bruneton_const_R
    (
        "const float HR   = %HR%; \n"
        "const vec3 betaR = %betaR%; \n\n"
    );

    static const std::string glsl_bruneton_const_M
    (
        "const float HM      = %HM%; \n"
        "const vec3 betaMSca = %betaMSca%; \n"
        "const vec3 betaMEx  = %betaMEx%; \n"
        "const float mieG    = %mieG%; \n\n"
    );


    // PARAMETERIZATION FUNCTIONS

    static const std::string glsl_bruneton_texture4D // requires: RES_MU, RES_MU_S, RES_R, RES_NU, cmn
    (
        "vec4 texture4D(sampler3D table, float r, float mu, float muS, float nu)\n"
        "{\n"
        "    float H = sqrt(cmn[2] * cmn[2] - cmn[1] * cmn[1]);\n"
        "    float rho = sqrt(r * r - cmn[1] * cmn[1]);\n"
        //"#ifdef INSCATTER_NON_LINEAR\n"
        "    float rmu = r * mu;\n"
        "    float delta = rmu * rmu - r * r + cmn[1] * cmn[1];\n"
        "    vec4 cst = rmu < 0.0 && delta > 0.0 ? vec4(1.0, 0.0, 0.0, 0.5 - 0.5 / float(RES_MU)) : vec4(-1.0, H * H, H, 0.5 + 0.5 / float(RES_MU));\n"
        "    float uR = 0.5 / float(RES_R) + rho / H * (1.0 - 1.0 / float(RES_R));\n"
        "    float uMu = cst.w + (rmu * cst.x + sqrt(delta + cst.y)) / (rho + cst.z) * (0.5 - 1.0 / float(RES_MU));\n"
        //    // paper formula
        //"    float uMuS = 0.5 / float(RES_MU_S) + max((1.0 - exp(-3.0 * muS - 0.6)) / (1.0 - exp(-3.6)), 0.0) * (1.0 - 1.0 / float(RES_MU_S));\n"
            // better formula
        "    float uMuS = 0.5 / float(RES_MU_S) + (atan(max(muS, -0.1975) * tan(1.26 * 1.1)) / 1.1 + (1.0 - 0.26)) * 0.5 * (1.0 - 1.0 / float(RES_MU_S));\n"
        //"#else\n"
        //"    float uR = 0.5 / float(RES_R) + rho / H * (1.0 - 1.0 / float(RES_R));\n"
        //"    float uMu = 0.5 / float(RES_MU) + (mu + 1.0) / 2.0 * (1.0 - 1.0 / float(RES_MU));\n"
        //"    float uMuS = 0.5 / float(RES_MU_S) + max(muS + 0.2, 0.0) / 1.2 * (1.0 - 1.0 / float(RES_MU_S));\n"
        //"#endif\n"
        "    float lerp = (nu + 1.0) / 2.0 * (float(RES_NU) - 1.0);\n"
        "    float uNu = floor(lerp);\n"
        "    lerp = lerp - uNu;\n"
        "    return texture3D(table, vec3((uNu + uMuS) / float(RES_NU), uMu, uR)) * (1.0 - lerp) +\n"
        "           texture3D(table, vec3((uNu + uMuS + 1.0) / float(RES_NU), uMu, uR)) * lerp;\n"
        "}\n\n"
    );


    static const std::string glsl_bruneton_muMuSNu // requires: RES_MU, RES_MU_S, RES_NU, cmn
    (
        "void getMuMuSNu(float r, vec4 dhdH, out float mu, out float muS, out float nu) {\n"
        "    float x = gl_FragCoord.x - 0.5;\n"
        "    float y = gl_FragCoord.y - 0.5;\n"
        //"#ifdef INSCATTER_NON_LINEAR\n"
        "    if (y < float(RES_MU) / 2.0) {\n"
        "        float d = 1.0 - y / (float(RES_MU) / 2.0 - 1.0);\n"
        "        d = min(max(dhdH.z, d * dhdH.w), dhdH.w * 0.999);\n"
        "        mu = (cmn[1] * cmn[1] - r * r - d * d) / (2.0 * r * d);\n"
        "        mu = min(mu, -sqrt(1.0 - (cmn[1] / r) * (cmn[1] / r)) - 0.001);\n"
        "    } else {\n"
        "        float d = (y - float(RES_MU) / 2.0) / (float(RES_MU) / 2.0 - 1.0);\n"
        "        d = min(max(dhdH.x, d * dhdH.y), dhdH.y * 0.999);\n"
        "        mu = (cmn[2] * cmn[2] - r * r - d * d) / (2.0 * r * d);\n"
        "    }\n"
        "    muS = mod(x, float(RES_MU_S)) / (float(RES_MU_S) - 1.0);\n"
        "    // paper formula\n"
        "    //muS = -(0.6 + log(1.0 - muS * (1.0 -  exp(-3.6)))) / 3.0;\n"
        "    // better formula\n"
        "    muS = tan((2.0 * muS - 1.0 + 0.26) * 1.1) / tan(1.26 * 1.1);\n"
        "    nu = -1.0 + floor(x / float(RES_MU_S)) / (float(RES_NU) - 1.0) * 2.0;\n"
        //"#else\n"
        //"    mu = -1.0 + 2.0 * y / (float(RES_MU) - 1.0);\n"
        //"    muS = mod(x, float(RES_MU_S)) / (float(RES_MU_S) - 1.0);\n"
        //"    muS = -0.2 + muS * 1.2;\n"
        //"    nu = -1.0 + floor(x / float(RES_MU_S)) / (float(RES_NU) - 1.0) * 2.0;\n"
        //"#endif\n"
        "}\n\n"
    );


    static const std::string glsl_bruneton_transmittanceUV // requires: cmn
    (
        "vec2 getTransmittanceUV(float r, float mu) {\n"
        "    float uR, uMu;\n"
        //"#ifdef TRANSMITTANCE_NON_LINEAR\n"
        "    uR = sqrt((r - cmn[1]) / (cmn[2] - cmn[1]));\n"
        "    uMu = atan((mu + 0.15) / (1.0 + 0.15) * tan(1.5)) / 1.5;\n"
        //"#else\n"
        //"    uR = (r - cmn[1]) / (cmn[2] - cmn[1]);\n"
        //"    uMu = (mu + 0.15) / (1.0 + 0.15);\n"
        //"#endif\n"
        "    return vec2(uMu, uR);\n"
        "}\n\n"
    );


    static const std::string glsl_bruneton_transmittanceRMu // requires: TRANSMITTANCE_H, TRANSMITTANCE_W, cmn
    (
        "void getTransmittanceRMu(out float r, out float muS) {\n"
        "    r = gl_FragCoord.y / float(TRANSMITTANCE_H);\n"
        "    muS = gl_FragCoord.x / float(TRANSMITTANCE_W);\n"
        //"#ifdef TRANSMITTANCE_NON_LINEAR\n"
        "    r = cmn[1] + (r * r) * (cmn[2] - cmn[1]);\n"
        "    muS = -0.15 + tan(1.5 * muS) / tan(1.5) * (1.0 + 0.15);\n"
        //"#else\n"
        //"    r = cmn[1] + r * (cmn[2] - cmn[1]);\n"
        //"    muS = -0.15 + muS * (1.0 + 0.15);\n"
        //"#endif\n"
        "}\n\n"
    );


    static const std::string glsl_bruneton_irradianceUV // requires: cmn
    (
        "vec2 getIrradianceUV(float r, float muS) {\n"
        "    float uR = (r - cmn[1]) / (cmn[2] - cmn[1]);\n"
        "    float uMuS = (muS + 0.2) / (1.0 + 0.2);\n"
        "    return vec2(uMuS, uR);\n"
        "}\n\n"
    );


    static const std::string glsl_bruneton_irradianceRMuS // requires: SKY_H, SKY_W, cmn
    (
        "void getIrradianceRMuS(out float r, out float muS) {\n"
        "    r = cmn[1] + (gl_FragCoord.y - 0.5) / (float(SKY_H) - 1.0) * (cmn[2] - cmn[1]);\n"
        "    muS = -0.2 + (gl_FragCoord.x - 0.5) / (float(SKY_W) - 1.0) * (1.0 + 0.2);\n"
        "}\n\n"
    );


    // UTILITY FUNCTIONS

    static const std::string glsl_bruneton_transmittance // requires: transmittanceSampler, transmittanceUV
    (
        "uniform sampler2D transmittanceSampler;\n\n"   

        // transmittance (=transparency) of atmosphere for infinite ray (r,mu)
        // (mu = cos(view zenith angle)), intersections with ground ignored
        "vec3 transmittance(float r, float mu) {\n"
        "    vec2 uv = getTransmittanceUV(r, mu);\n"
        "    return texture2D(transmittanceSampler, uv).rgb;\n"
        "}\n\n"

        // transmittance(=transparency) of atmosphere between x and x0
        // assume segment x,x0 not intersecting ground
        // r = ||x||, mu = cos(zenith angle of [x,x0) ray at x), v = unit direction vector of [x,x0) ray
        "vec3 transmittance(float r, float mu, vec3 v, vec3 x0) {\n"
        "    vec3 result;\n"
        "    float r1 = length(x0);\n"
        "    float mu1 = dot(x0, v) / r;\n"
        "    if (mu > 0.0) {\n"
        "        result = min(transmittance(r, mu) / transmittance(r1, mu1), 1.0);\n"
        "    } else {\n"
        "        result = min(transmittance(r1, -mu1) / transmittance(r, -mu), 1.0);\n"
        "    }\n"
        "    return result;\n"
        "}\n\n"

        // transmittance(=transparency) of atmosphere between x and x0
        // assume segment x,x0 not intersecting ground
        // d = distance between x and x0, mu = cos(zenith angle of [x,x0) ray at x)
        "vec3 transmittance(float r, float mu, float d) {\n"
        "    vec3 result;\n"
        "    float r1 = sqrt(r * r + d * d + 2.0 * r * mu * d);\n"
        "    float mu1 = (r * mu + d) / r1;\n"
        "    if (mu > 0.0) {\n"
        "        result = min(transmittance(r, mu) / transmittance(r1, mu1), 1.0);\n"
        "    } else {\n"
        "        result = min(transmittance(r1, -mu1) / transmittance(r, -mu), 1.0);\n"
        "    }\n"
        "    return result;\n"
        "}\n\n"
    );


    static const std::string glsl_bruneton_transmittanceWithShadow // requires: cmn, transmittance()
    (
        // transmittance(=transparency) of atmosphere for infinite ray (r,mu)
        // (mu = cos(view zenith angle)), or zero if ray intersects ground
        "vec3 transmittanceWithShadow(float r, float mu) {\n"
        "    return mu < -sqrt(1.0 - (cmn[1] / r) * (cmn[1] / r)) ? vec3(0.0) : transmittance(r, mu);\n"
        "}\n\n"
    );


    static const std::string glsl_bruneton_limit // requires: RL, cmn
    (
        // nearest intersection of ray r,mu with ground or top atmosphere boundary
        // mu=cos(ray zenith angle at ray origin)
        "float limit(float r, float mu) {\n"
        "    float RL = cmn[2] + 1.0;\n"
        "    float dout = -r * mu + sqrt(r * r * (mu * mu - 1.0) + RL * RL);\n"
        "    float delta2 = r * r * (mu * mu - 1.0) + cmn[1] * cmn[1];\n"
        "    if (delta2 >= 0.0) {\n"
        "        float din = -r * mu - sqrt(delta2);\n"
        "        if (din >= 0.0) {\n"
        "            dout = min(dout, din);\n"
        "        }\n"
        "    }\n"
        "    return dout;\n"
        "}\n\n"
    );


    static const std::string glsl_bruneton_hdr // requires: -
    (
        "vec3 HDR(vec3 L) {\n"
        "    L = L * exposure;\n"
        "    L.r = L.r < 1.413 ? pow(L.r * 0.38317, 1.0 / 2.2) : 1.0 - exp(-L.r);\n"
        "    L.g = L.g < 1.413 ? pow(L.g * 0.38317, 1.0 / 2.2) : 1.0 - exp(-L.g);\n"
        "    L.b = L.b < 1.413 ? pow(L.b * 0.38317, 1.0 / 2.2) : 1.0 - exp(-L.b);\n"
        "    return L;\n"
        "}\n\n"
    );


    static const std::string glsl_bruneton_opticalDepth // requires: cmn
    (
        // optical depth for ray (r,mu) of length d, using analytic formula
        // (mu = cos(view zenith angle)), intersections with ground ignored
        // H = height scale of exponential density function
        "float opticalDepth(float H, float r, float mu, float d) {\n"
        "    float a = sqrt((0.5/H)*r);\n"
        "    vec2 a01 = a*vec2(mu, mu + d / r);\n"
        "    vec2 a01s = sign(a01);\n"
        "    vec2 a01sq = a01*a01;\n"
        "    float x = a01s.y > a01s.x ? exp(a01sq.x) : 0.0;\n"
        "    vec2 y = a01s / (2.3193*abs(a01) + sqrt(1.52*a01sq + 4.0)) * vec2(1.0, exp(-d/H*(d/(2.0*r)+mu)));\n"
        "    return sqrt((6.2831*H)*r) * exp((cmn[1]-r)/H) * (x + dot(y, vec2(1.0, -1.0)));\n"
        "}\n\n"
    );


    static const std::string glsl_bruneton_analyticTransmittance // requires: opticalDepth(), HR, betaR, HM, betaMEx
    (
        // transmittance(=transparency) of atmosphere for ray (r,mu) of length d
        // (mu=cos(view zenith angle)), intersections with ground ignored
        // uses analytic formula instead of transmittance texture
        "vec3 analyticTransmittance(float r, float mu, float d) {\n"
        "    return exp(- betaR * opticalDepth(HR, r, mu, d) - betaMEx * opticalDepth(HM, r, mu, d));\n"
        "}\n\n"
    );


    static const std::string glsl_bruneton_irradiance // requires: irradianceUV()
    (
        "vec3 irradiance(sampler2D sampler, float r, float muS) {\n"
        "    vec2 uv = getIrradianceUV(r, muS);\n"
        "    return texture2D(sampler, uv).rgb;\n"
        "}\n\n"
    );


    static const std::string glsl_bruneton_phaseFunctionR // requires: M_PI
    (
        // Rayleigh phase function
        "float phaseFunctionR(float mu) {\n"
        "    return (3.0 / (16.0 * M_PI)) * (1.0 + mu * mu);\n"
        "}\n\n"
    );


    static const std::string glsl_bruneton_phaseFunctionM // requires: M_PI, mieG
    (
        // Mie phase function
        "float phaseFunctionM(float mu) {\n"
        "    return 1.5 * 1.0 / (4.0 * M_PI) * (1.0 - mieG*mieG) * pow(1.0 + (mieG*mieG) - 2.0*mieG*mu, -3.0/2.0) * (1.0 + mu * mu) / (2.0 + mieG*mieG);\n"
        "}\n\n"
    );


    static const std::string glsl_bruneton_mie // requires: betaR
    (
        // approximated single Mie scattering (cf. approximate Cm in paragraph 'Angular precision')
        "vec3 getMie(vec4 rayMie) {\n" // rayMie.rgb = C*, rayMie.w = Cm, r
        "    return rayMie.rgb * rayMie.w / max(rayMie.r, 1e-4) * (betaR.r / betaR);\n"
        "}\n\n"
    );
}

#endif // __GLSL_BRUNETON_COMMON_HPP__