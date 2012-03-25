
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

#include "common.h"


namespace osgHimmel
{

const std::string glsl_version_150
(
    PRAGMA_ONCE(version,

    "#version 150 compatibility")
);


const std::string glsl_geometry_ext
(
    PRAGMA_ONCE(GL_EXT_geometry_shader4,

    "#extension GL_EXT_geometry_shader4 : enable")
);


const std::string glsl_quadTransform
(
    PRAGMA_ONCE(quadTransform,

    "void quadTransform()\n"
    "{\n"
    "    gl_Position = gl_Vertex;\n"
    "}")
);


const std::string glsl_quadRetrieveRay
(
    PRAGMA_ONCE(quadRetrieveRay,

    "vec4 quadRetrieveRay()\n"
    "{\n"
    "    return gl_ProjectionMatrixInverse * gl_Vertex * gl_ModelViewMatrix;\n"
    "}")
);


const std::string glsl_cmn_uniform
(
    PRAGMA_ONCE(cmn,
    
    "uniform vec4 cmn;")
);


// Returns unit intersection distance with the ground. Use for 
// e.g. horizon related discarding of elements

const std::string glsl_horizon 
(
    glsl_cmn_uniform +

    PRAGMA_ONCE(belowHorizon,

    "float tAtm(vec3 ray)\n"
    "{\n"
    "    vec3 x = vec3(0.0, 0.0, cmn[1] + cmn[0]);\n"
    "    vec3 v = normalize(ray);\n"
    "\n"
    "    float r = length(x);\n"
    "    float mu = dot(x, v) / r;\n"
    "\n"
    "    return r * mu - sqrt(r * r * (mu * mu - 1.0) + cmn[1] * cmn[1]);\n"
    "}\n"
    "\n"
    "bool belowHorizon(vec3 ray)\n"
    "{\n"
    "   return tAtm(ray) < 0.0;\n"
    "}")
);

} // namespace osgHimmel