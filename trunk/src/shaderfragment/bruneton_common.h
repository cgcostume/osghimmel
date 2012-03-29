
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
#ifndef __GLSL_BRUNETON_COMMON_H__
#define __GLSL_BRUNETON_COMMON_H__

#include <string>

namespace osgHimmel
{

const std::string glsl_bruneton_v_default();

//const std::string glsl_bruneton_g_default();

// TEXTURE SIZE
    
const std::string glsl_bruneton_const_TSize();
const std::string glsl_bruneton_const_ISize();
const std::string glsl_bruneton_const_RSize();

// NUMERICAL INTEGRATION PARAMETERS

const std::string glsl_bruneton_const_Samples();
const std::string glsl_bruneton_const_PI();

// PHYSICAL MODEL PARAMETERS

const std::string glsl_bruneton_const_avgReflectance();
const std::string glsl_bruneton_const_R();
const std::string glsl_bruneton_const_M();

// PARAMETERIZATION FUNCTIONS

const std::string glsl_bruneton_texture4D();           // requires: RES_MU, RES_MU_S, RES_R, RES_NU, cmn
const std::string glsl_bruneton_muMuSNu();             // requires: RES_MU, RES_MU_S, RES_NU, cmn
const std::string glsl_bruneton_transmittanceUV();     // requires: cmn
const std::string glsl_bruneton_transmittanceRMu();    // requires: TRANSMITTANCE_H, TRANSMITTANCE_W, cmn
const std::string glsl_bruneton_irradianceUV();        // requires: cmn
const std::string glsl_bruneton_irradianceRMuS();      // requires: SKY_H, SKY_W, cmn

// UTILITY FUNCTIONS

const std::string glsl_bruneton_transmittance();       // requires: transmittanceSampler, transmittanceUV
const std::string glsl_bruneton_transmittanceWithShadow(); // requires: cmn, transmittance()
const std::string glsl_bruneton_limit();               // requires: RL, cmn
const std::string glsl_bruneton_hdr();                 // requires: -
const std::string glsl_bruneton_opticalDepth();        // requires: cmn
const std::string glsl_bruneton_analyticTransmittance(); // requires: opticalDepth(), HR, betaR, HM, betaMEx
const std::string glsl_bruneton_irradiance();          // requires: irradianceUV()
const std::string glsl_bruneton_phaseFunctionR();      // requires: PI
const std::string glsl_bruneton_phaseFunctionM();      // requires: PI, mieG
const std::string glsl_bruneton_mie();                 // requires: betaR

} // namespace osgHimmel
 
#endif // __GLSL_BRUNETON_COMMON_H__