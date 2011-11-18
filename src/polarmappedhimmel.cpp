
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

#include "polarmappedhimmel.h"
#include "horizonband.h"

#include <osg/Texture2D>

#include <assert.h>

PolarMappedHimmel::PolarMappedHimmel(
    const e_MappingMode &mappingMode
,   const bool withHorizonBand)
:   AbstractMappedHimmel()
,   m_mappingMode(mappingMode)
,   m_hBand(NULL)
,   m_withHBand(withHorizonBand)
{
    setName("PolarMappedHimmel");

    if(m_withHBand)
    {
        m_hBand = new HorizonBand();
        m_hBand->initialize(getOrCreateStateSet());
    }
};


PolarMappedHimmel::~PolarMappedHimmel()
{
};


HorizonBand *PolarMappedHimmel::hBand()
{
    return m_hBand;
}


osg::Texture2D* PolarMappedHimmel::getOrCreateTexture2D(const GLint textureUnit)
{
    // Retrieve an existing texture.

    const t_tex2DById::iterator existingTex2D(m_tex2DsById.find(textureUnit));
    if(existingTex2D != m_tex2DsById.end())
        return existingTex2D->second;


    // Create and configure new texture object.

    osg::ref_ptr<osg::Texture2D> newTex2D(new osg::Texture2D);

    newTex2D->setUnRefImageDataAfterApply(true);

    newTex2D->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    newTex2D->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

    newTex2D->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
    newTex2D->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

    m_tex2DsById[textureUnit] = newTex2D;

    // Assign some textures if there are none.

    if(m_tex2DsById.size() == 1)
        assignBackUnit(textureUnit);
    if(m_tex2DsById.size() == 2)
        assignSrcUnit(textureUnit);

    return newTex2D;
}


osg::StateAttribute *PolarMappedHimmel::getTextureAttribute(const GLint textureUnit) const
{
    const t_tex2DById::const_iterator tex2Di(m_tex2DsById.find(textureUnit));
    return tex2Di == m_tex2DsById.end() ? NULL : tex2Di->second;
}


// VertexShader

// -> specified in AbstractMappedHimmel


// FragmentShader

#include "shaderfragment/version.fsf"
#include "shaderfragment/blend_normal.fsf"
#include "shaderfragment/hband.fsf"

const std::string PolarMappedHimmel::getFragmentShaderSource()
{
    switch(getMappingMode())
    {
    case MM_Half:

        return glsl_f_version

        +   glsl_f_blendNormalExt

        +   (m_withHBand ? glsl_f_hband : "")
        +
            "in vec4 m_ray;\n"
            "\n"

            // From AbstractMappedHimmel

            "uniform float srcAlpha;\n"
            "\n"
            "uniform sampler2D back;\n"
            "uniform sampler2D src;\n"
            "\n"

            // Color Retrieval

            "const float c_2OverPi  = 0.6366197723675813430755350534901;\n"
            "const float c_1Over2Pi = 0.1591549430918953357688837633725;\n"
            "\n"
            "void main(void)\n"
            "{\n"
            "    vec3 stu = normalize(m_ray.xyz);\n"
            "\n"
            "    vec2 uv = vec2(atan(stu.x, stu.y) * c_1Over2Pi, asin(+stu.z) * c_2OverPi);\n"
            "\n"
            "    vec4 fc = mix(texture2D(back, uv), texture2D(src, uv), clamp(0.0, srcAlpha, 1.0));\n"
            "\n"
            "    gl_FragColor = " + (m_withHBand ? "hband(stu.z, fc)" : "fc") + ";\n"
            "}\n\n";


    case MM_Full:

        return glsl_f_version

//        +   glsl_f_blendNormalExt // using mix
        +
            "in vec4 m_ray;\n"
            "\n"

            // From AbstractMappedHimmel

            "uniform float srcAlpha;\n"
            "\n"
            "uniform sampler2D back;\n"
            "uniform sampler2D src;\n"
            "\n"

            // Color Retrieval

            "const float c_1OverPi  = 0.3183098861837906715377675267450;\n"
            "const float c_1Over2Pi = 0.1591549430918953357688837633725;\n"
            "\n"
            "void main(void)\n"
            "{\n"
            "    vec3 stu = normalize(m_ray.xyz);\n"
            "\n"
            "    vec2 uv = vec2(atan(stu.x, stu.y) * c_1Over2Pi, acos(-stu.z) * c_1OverPi);\n"
            "\n"
            "    gl_FragColor = mix(texture2D(back, uv), texture2D(src, uv), clamp(0.0, srcAlpha, 1.0));\n"
            "}\n\n";

    default:
        assert(false);
    }

    return "";
}
