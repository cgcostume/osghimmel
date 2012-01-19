
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

#include "atmospheregeode.h"

#include "proceduralhimmel.h"
#include "shadermodifier.h"
#include "himmelquad.h"
#include "abstractastronomy.h"


AtmosphereGeode::AtmosphereGeode(const ProceduralHimmel &himmel)
:   osg::Geode()
,   m_himmel(himmel)

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   m_hquad(new HimmelQuad())

,   u_sun(NULL)
,   u_ditheringMultiplier(NULL)
{
    setName("Atmosphere");

    osg::StateSet* stateSet = getOrCreateStateSet();

    setupNode(stateSet);
    setupUniforms(stateSet);
    setupShader(stateSet);
    setupTextures(stateSet);


    addDrawable(m_hquad);
};


AtmosphereGeode::~AtmosphereGeode()
{
};


void AtmosphereGeode::update()
{
    osg::Vec3 sunv = m_himmel.astro()->getSunPosition();
    u_sun->set(osg::Vec4(sunv, m_himmel.astro()->getAngularSunRadius()/* * m_scale*/));
}


void AtmosphereGeode::setupUniforms(osg::StateSet* stateSet)
{
    u_sun = new osg::Uniform("sun", osg::Vec4(1.0, 0.0, 0.0, 1.0)); // [3] = apparent angular radius (not diameter!)
    stateSet->addUniform(u_sun);

    u_ditheringMultiplier = new osg::Uniform("ditheringMultiplier", defaultDitheringMultiplier());
    stateSet->addUniform(u_ditheringMultiplier);
}


void AtmosphereGeode::setupNode(osg::StateSet* stateSet)
{
}


void AtmosphereGeode::setupShader(osg::StateSet* stateSet)
{
    m_vShader->setShaderSource(getVertexShaderSource());
    m_fShader->setShaderSource(getFragmentShaderSource());

    m_program->addShader(m_vShader);
    m_program->addShader(m_fShader);

    stateSet->setAttributeAndModes(m_program, osg::StateAttribute::ON);

#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
    if(m_himmel.shaderModifier())
    {
        m_himmel.shaderModifier()->registerShader(getName(), m_fShader);
        m_himmel.shaderModifier()->registerShader(getName(), m_vShader);
    }
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER
}


void AtmosphereGeode::setupTextures(osg::StateSet* stateSet)
{

}


const float AtmosphereGeode::setDitheringMultiplier(const float multiplier)
{
    u_ditheringMultiplier->set(multiplier);
    return getDitheringMultiplier();
}

const float AtmosphereGeode::getDitheringMultiplier() const
{
    float multiplier;
    u_ditheringMultiplier->get(multiplier);

    return multiplier;
}

const float AtmosphereGeode::defaultDitheringMultiplier()
{
    return 4.0;
}




// VertexShader

#include "shaderfragment/version.vsf"
#include "shaderfragment/quadretrieveray.vsf"
#include "shaderfragment/quadtransform.vsf"

const std::string AtmosphereGeode::getVertexShaderSource()
{
    return glsl_v_version_150

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
#include "shaderfragment/pseudo_rand.fsf"
#include "shaderfragment/dither.fsf"

const std::string AtmosphereGeode::getFragmentShaderSource()
{
    return glsl_f_version_150
    
    +
        "in vec4 m_ray;\n"
        "\n"
        "uniform int osg_FrameNumber;\n"    // required by pseudo_rand
        "\n"

    +   glsl_f_pseudo_rand
    +   glsl_f_dither
    +
        // Color Retrieval

        "uniform vec4 sun;\n"
        //"uniform vec3 moon;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    vec3 stu = normalize(m_ray.xyz);\n"
        "\n"
        "    float s = 1.0 / length(normalize(sun.xyz) - stu)  * 0.08;\n"
        //"    float m = 1.0 / length(normalize(moon) - stu) * 0.08;\n"
        "\n"
	    "    vec3 su = vec3(0.6, .5, 0.4) * s;\n"
	    //"    vec3 mo = vec3(0.6, .6, 0.5) * clamp(m, 0.0, 2.0) * 0.66;\n"
        //"\n"
        //"    vec3 h = vec3(pow(1.0 - abs(stu.z), 2) * 0.8);\n"
        //"\n"
        //"    gl_FragColor = vec4(mo + su + h, 1.0) + dither();\n"
        "    gl_FragColor = vec4(su, 1.0) + dither();\n"
        "}\n\n";
}