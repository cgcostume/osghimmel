
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

#include "milkywaygeode.h"

#include "proceduralhimmel.h"
#include "shadermodifier.h"
#include "himmelquad.h"
#include "abstractastronomy.h"

#include <osg/Geometry>
#include <osg/TextureCubeMap>
#include <osg/Texture2D>
#include <osgDB/ReadFile>


MilkywayGeode::MilkywayGeode(const ProceduralHimmel &himmel)
:   osg::Geode()
,   m_himmel(himmel)

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   m_hquad(new HimmelQuad())

,   u_R(NULL)
,   u_color(NULL)
,   u_milkywayCube(NULL)
{
    setName("Milkyway");

    osg::StateSet* stateSet = getOrCreateStateSet();

    setupNode(stateSet);
    setupUniforms(stateSet);
    setupShader(stateSet);
    setupTextures(stateSet);


    addDrawable(m_hquad);
};


MilkywayGeode::~MilkywayGeode()
{
};


void MilkywayGeode::update()
{
    u_R->set(m_himmel.astro()->equToLocalHorizonMatrix());
}


void MilkywayGeode::setupUniforms(osg::StateSet* stateSet)
{
    u_R = new osg::Uniform("R", osg::Matrix::identity());
    stateSet->addUniform(u_R);

    u_color = new osg::Uniform("color", osg::Vec4(defaultColor(), defaultIntensity()));
    stateSet->addUniform(u_color);

    u_milkywayCube = new osg::Uniform("milkywayCube", 0);
    stateSet->addUniform(u_milkywayCube);
}


void MilkywayGeode::createAndAddDrawable()
{

}


void MilkywayGeode::setupNode(osg::StateSet* stateSet)
{
    createAndAddDrawable();
}


void MilkywayGeode::setupShader(osg::StateSet* stateSet)
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


void MilkywayGeode::setupTextures(osg::StateSet* stateSet)
{   
    osg::ref_ptr<osg::TextureCubeMap> tcm(new osg::TextureCubeMap);

    tcm->setUnRefImageDataAfterApply(true);

    tcm->setInternalFormat(GL_RGBA);

    tcm->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tcm->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    tcm->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

    tcm->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    tcm->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

    tcm->setImage(osg::TextureCubeMap::POSITIVE_X, osgDB::readImageFile("resources/milkyway_px.png"));
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_X, osgDB::readImageFile("resources/milkyway_nx.png"));
    tcm->setImage(osg::TextureCubeMap::POSITIVE_Y, osgDB::readImageFile("resources/milkyway_py.png"));
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_Y, osgDB::readImageFile("resources/milkyway_ny.png"));
    tcm->setImage(osg::TextureCubeMap::POSITIVE_Z, osgDB::readImageFile("resources/milkyway_pz.png"));
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_Z, osgDB::readImageFile("resources/milkyway_nz.png"));

    stateSet->setTextureAttributeAndModes(0, tcm, osg::StateAttribute::ON);

    u_milkywayCube->set(0);
}


const osg::Vec3 MilkywayGeode::setColor(const osg::Vec3 &color)
{
    osg::Vec4 temp;
    u_color->get(temp);

    temp[0] = color[0];
    temp[1] = color[1];
    temp[2] = color[2];

    u_color->set(temp);

    return getColor();
}

const osg::Vec3 MilkywayGeode::getColor() const
{
    osg::Vec4 earthShine;
    u_color->get(earthShine);

    return osg::Vec3(earthShine[0], earthShine[1], earthShine[2]);
}

const osg::Vec3 MilkywayGeode::defaultColor()
{
    return osg::Vec3(0.84, 0.9, 1.0);
}


const float MilkywayGeode::setIntensity(const float intensity)
{
    osg::Vec4 color;
    u_color->get(color);

    color[3] = intensity;
    u_color->set(color);

    return getIntensity();
}

const float MilkywayGeode::getIntensity() const
{
    osg::Vec4 color;
    u_color->get(color);

    return color[3];
}

const float MilkywayGeode::defaultIntensity()
{
    return 0.33f;
}



// VertexShader

#include "shaderfragment/version.vsf"
#include "shaderfragment/quadretrieveray.vsf"
#include "shaderfragment/quadtransform.vsf"

const std::string MilkywayGeode::getVertexShaderSource()
{
    return glsl_v_version_150

    +   glsl_v_quadRetrieveRay
    +   glsl_v_quadTransform
    +
        "out vec4 m_ray;\n"
        "uniform mat4 R;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    m_ray = R * quadRetrieveRay();\n"
        "    quadTransform();\n"
        "}\n\n";
}


// FragmentShader

#include "shaderfragment/version.fsf"

const std::string MilkywayGeode::getFragmentShaderSource()
{
    return glsl_f_version_150

    +
        "uniform vec4 color;\n"
        "uniform samplerCube milkywayCube;\n"
        "\n"

        "in vec4 m_ray;\n"
        "\n"

        "void main(void)\n"
        "{\n"
        "    vec3 stu = normalize(m_ray.xyz);\n"
        "    vec4 fc = textureCube(milkywayCube, stu);\n"
        "\n"
        "    gl_FragColor = vec4(fc.rgb * color.w * color.rgb, 1.0);\n"
        "}\n\n";
}
