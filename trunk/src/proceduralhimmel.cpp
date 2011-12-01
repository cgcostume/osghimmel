
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

#include "proceduralhimmel.h"
#include "timef.h"
#include "himmelquad.h"


ProceduralHimmel::ProceduralHimmel()
:   AbstractHimmel()
{
    addChild(m_hquad);
};


ProceduralHimmel::~ProceduralHimmel()
{
};


// VertexShader

#include "shaderfragment/version.vsf"
#include "shaderfragment/quadretrieveray.vsf"
#include "shaderfragment/quadtransform.vsf"

const std::string ProceduralHimmel::getVertexShaderSource()
{
    return glsl_v_version

        +   glsl_v_quadRetrieveRay
        +   glsl_v_quadTransform
        +
        "out vec4 m_ray;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    m_ray = quadRetrieveRay();\n"
        "    quadTransform();\n"
        "}\n\n";
}


// FragmentShader

#include "shaderfragment/version.fsf"

const std::string ProceduralHimmel::getFragmentShaderSource()
{
    return glsl_f_version

    +
        "in vec4 m_ray;\n"
        "\n"

        "const vec3 t = vec3(1.0, 0.0, 0.5);\n"

        // Color Retrieval

        "void main(void)\n"
        "{\n"
        "    vec3 stu = normalize(m_ray.xyz);\n"
        "\n"
        "    float d = 1.0 / length(normalize(t) - stu) * 0.04;\n"
        "\n"
        "    float h = pow(1.0 - abs(stu.z), 2) * 0.8;\n"
        "\n"
        "    gl_FragColor = vec4(vec3(d + h), 1.0);\n"
        "}\n\n";
}

