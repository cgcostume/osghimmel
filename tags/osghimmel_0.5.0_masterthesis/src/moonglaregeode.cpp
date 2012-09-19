
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

#include "moonglaregeode.h"

#include "himmel.h"
#include "himmelquad.h"
#include "abstractastronomy.h"
#include "mathmacros.h"

#include "shaderfragment/common.h"

#include <osg/Geometry>
#include <osg/BlendFunc>
#include <osg/Depth>


namespace osgHimmel
{

MoonGlareGeode::MoonGlareGeode()
:   osg::Geode()

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   u_intensity(NULL)
,   u_scale(NULL)
,   u_phase(NULL)

,   m_hquad(new HimmelQuad())
{
    setName("MoonGlare");

    osg::StateSet* stateSet = getOrCreateStateSet();

    setupNode(stateSet);
    setupUniforms(stateSet);
    setupShader(stateSet);

    addDrawable(m_hquad);
};


MoonGlareGeode::~MoonGlareGeode()
{
};


void MoonGlareGeode::update(const Himmel &himmel)
{
    const osg::Vec3 moonv = himmel.astro()->getMoonPosition(false);
    const osg::Vec3 sunv  = himmel.astro()->getSunPosition(false);
    
    u_phase->set(acos(moonv * sunv));
}


void MoonGlareGeode::setupUniforms(osg::StateSet* stateSet)
{
    u_intensity = new osg::Uniform("intensity", defaultIntensity());
    stateSet->addUniform(u_intensity);

    u_scale = new osg::Uniform("scale", defaultScale());
    stateSet->addUniform(u_scale);

    u_phase = new osg::Uniform("phase", 0.f);
    stateSet->addUniform(u_phase);
}


void MoonGlareGeode::setupNode(osg::StateSet *stateSet)
{
    osg::Depth* depth = new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0);    
    stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

    osg::BlendFunc *blend  = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE);
    stateSet->setAttributeAndModes(blend, osg::StateAttribute::ON);
}


void MoonGlareGeode::setupShader(osg::StateSet* stateSet)
{
    m_vShader->setShaderSource(getVertexShaderSource());
    m_fShader->setShaderSource(getFragmentShaderSource());

    m_program->addShader(m_vShader);
    m_program->addShader(m_fShader);

    stateSet->setAttributeAndModes(m_program, osg::StateAttribute::ON);
}


const float MoonGlareGeode::setScale(const float scale)
{
    u_scale->set(scale);
    return getScale();
}

const float MoonGlareGeode::getScale() const
{
    float scale;
    u_scale->get(scale);

    return scale;
}

const float MoonGlareGeode::defaultScale()
{
    return 128.f;
}


const float MoonGlareGeode::setIntensity(const float intensity)
{
    u_intensity->set(intensity);
    return getIntensity();
}

const float MoonGlareGeode::getIntensity() const
{
    float intensity;
    u_intensity->get(intensity);

    return intensity;
}

const float MoonGlareGeode::defaultIntensity()
{
    return 0.5f;
}


const std::string MoonGlareGeode::getVertexShaderSource()
{
    return glsl_version_150() +

    +   PRAGMA_ONCE(main,

        // moon.xyz is expected to be normalized and moon.a the moons
        // angular diameter in rad.
        "uniform vec4 moon;\n"
        "uniform vec4 moonr;\n"
        "\n"
        "uniform float scale;\n"
        "\n"
        "const float SQRT2 = 1.41421356237;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    vec3 m = moonr.xyz;\n"
        "\n"
        "    vec3 u = normalize(cross(vec3(0, 0, 1), m));\n"
        "    vec3 v = normalize(cross(m, u));\n"
        "\n"
        "    float mScale = tan(moon.a) * SQRT2 * scale;\n"
        "\n"
        "    vec3 eye = m - normalize(gl_Vertex.x * u + gl_Vertex.y * v) * mScale;\n"
        "\n"
        "    gl_TexCoord[0] = gl_Vertex;\n"
        "    gl_Position = gl_ModelViewProjectionMatrix * vec4(eye, 1.0);\n"
        "}");
}


const std::string MoonGlareGeode::getFragmentShaderSource()
{
    return glsl_version_150()

    +   PRAGMA_ONCE(main,

        "uniform vec3 sun;\n"
        "\n"
        // moon.xyz is expected to be normalized and moon.a the moons
        // angular diameter in rad.
        "uniform vec4 moon;\n"
        "\n"
        "uniform float intensity;\n"
        "\n"
        "uniform vec4 sunShine;\n"
        "uniform vec3 earthShine;\n"
        "\n"
        "uniform float phase;\n"
        "\n"
        "const float radius = 0.98;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    float x = gl_TexCoord[0].x;\n"
        "    float y = gl_TexCoord[0].y;\n"
        "\n"
        "    float zz = radius * radius - x * x - y * y;\n"
        "    if(zz < 1.0 - radius)\n"
        "        discard;\n"
        "\n"
        "    float l = 1 - pow(length(vec2(x, y)) + 0.02, 0.25);\n"
            // Day-Twilight-Night-Intensity Mapping (Butterworth-Filter)
        "    float b = 1.0 / sqrt(1 + pow(sun.z + 1.05, 32));\n"
        "\n"
        "    gl_FragColor = vec4(l * (phase * sunShine.rgb) + l * earthShine , 1.0) * intensity * b;\n"
        "}");
}




#ifdef OSGHIMMEL_EXPOSE_SHADERS

osg::Shader *MoonGlareGeode::getVertexShader()
{
    return m_vShader;
}
osg::Shader *MoonGlareGeode::getGeometryShader()
{
    return NULL;
}
osg::Shader *MoonGlareGeode::getFragmentShader()
{
    return m_fShader;
}

#endif // OSGHIMMEL_EXPOSE_SHADERS

} // namespace osgHimmel