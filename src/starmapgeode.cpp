
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

#include "starmapgeode.h"

#include "himmel.h"
#include "himmelquad.h"
#include "abstractastronomy.h"
#include "earth.h"
#include "mathmacros.h"

#include "shaderfragment/common.h"
#include "shaderfragment/scattering.h"

#include <osg/Geometry>
#include <osg/TextureCubeMap>
#include <osg/Texture2D>
#include <osgDB/ReadFile>


namespace osgHimmel
{

StarMapGeode::StarMapGeode(const char* cubeMapFilePath)
:   osg::Geode()

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   m_hquad(new HimmelQuad())

,   u_R(NULL)
,   u_q(NULL)

,   u_color(NULL) // [0,1,2] = color; [3] = intensity
,   u_deltaM(NULL)
,   u_scattering(NULL)
,   u_starmapCube(NULL)

{
    setName("StarMap");

    osg::StateSet* stateSet = getOrCreateStateSet();

    setupNode(stateSet);
    setupUniforms(stateSet);
    setupShader(stateSet);
    setupTextures(stateSet, cubeMapFilePath);

    addDrawable(m_hquad);
};


StarMapGeode::~StarMapGeode()
{
};


void StarMapGeode::update(const Himmel &himmel)
{
    // TODO: starmap and planets also require this ... - find better place
    const float fov = himmel.getCameraFovHint();
    const float height = himmel.getViewSizeHeightHint();

    //u_q->set(static_cast<float>(tan(_rad(fov / 2)) / (height * 0.5)));
    u_q->set(static_cast<float>(sqrt(2.0) * 2.0 * tan(_rad(fov * 0.5)) / height));

    u_R->set(himmel.astro()->getEquToHorTransform());
}


void StarMapGeode::setupNode(osg::StateSet*)
{
}


void StarMapGeode::setupUniforms(osg::StateSet* stateSet)
{
    u_R = new osg::Uniform("R", osg::Matrix::identity());
    stateSet->addUniform(u_R);

    u_q = new osg::Uniform("q", 0.0f);
    stateSet->addUniform(u_q);

    u_color = new osg::Uniform("color", osg::Vec4(defaultColor(), defaultColorRatio()));
    stateSet->addUniform(u_color);

    u_deltaM = new osg::Uniform("deltaM", 1.f);
    stateSet->addUniform(u_deltaM);

    setApparentMagnitude(defaultApparentMagnitude()); // This calls updateScaledB.

    u_scattering = new osg::Uniform("scattering", defaultScattering());
    stateSet->addUniform(u_scattering);


    u_starmapCube = new osg::Uniform("starmapCube", 0);
    stateSet->addUniform(u_starmapCube);
}


void StarMapGeode::setupShader(osg::StateSet* stateSet)
{
    m_vShader->setShaderSource(getVertexShaderSource());
    m_fShader->setShaderSource(getFragmentShaderSource());

    m_program->addShader(m_vShader);
    m_program->addShader(m_fShader);

    stateSet->setAttributeAndModes(m_program, osg::StateAttribute::ON);
}


void StarMapGeode::setupTextures(
    osg::StateSet* stateSet
,   const char* cubeMapFilePath)
{   
    osg::ref_ptr<osg::TextureCubeMap> tcm(new osg::TextureCubeMap);

    tcm->setUnRefImageDataAfterApply(true);

    tcm->setInternalFormat(GL_RGBA);

    tcm->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tcm->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    tcm->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

    tcm->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    tcm->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

    std::string px = cubeMapFilePath; px.replace(px.find("?"), 1, "_px");
    std::string nx = cubeMapFilePath; nx.replace(nx.find("?"), 1, "_nx");
    std::string py = cubeMapFilePath; py.replace(py.find("?"), 1, "_py");
    std::string ny = cubeMapFilePath; ny.replace(ny.find("?"), 1, "_ny");
    std::string pz = cubeMapFilePath; pz.replace(pz.find("?"), 1, "_pz");
    std::string nz = cubeMapFilePath; nz.replace(nz.find("?"), 1, "_nz");

    tcm->setImage(osg::TextureCubeMap::POSITIVE_X, osgDB::readImageFile(px));
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_X, osgDB::readImageFile(nx));
    tcm->setImage(osg::TextureCubeMap::POSITIVE_Y, osgDB::readImageFile(py));
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_Y, osgDB::readImageFile(ny));
    tcm->setImage(osg::TextureCubeMap::POSITIVE_Z, osgDB::readImageFile(pz));
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_Z, osgDB::readImageFile(nz));

    stateSet->setTextureAttributeAndModes(0, tcm, osg::StateAttribute::ON);

    u_starmapCube->set(0);
}


void StarMapGeode::updateScaledB()
{
    // Precompute brightness based on logarithmic scale. 
    // (Similar to starsgeode vertex shader.)

    const float deltaM = pow(2.512, m_apparentMagnitude - static_cast<double>(Earth::apparentMagnitudeLimit()));

    u_deltaM->set(deltaM);
}


const osg::Vec3 StarMapGeode::setColor(const osg::Vec3 &color)
{
    osg::Vec4 temp;
    u_color->get(temp);

    temp[0] = color[0];
    temp[1] = color[1];
    temp[2] = color[2];

    u_color->set(temp);

    return getColor();
}

const osg::Vec3 StarMapGeode::getColor() const
{
    osg::Vec4 color;
    u_color->get(color);

    return osg::Vec3(color[0], color[1], color[2]);
}

const osg::Vec3 StarMapGeode::defaultColor()
{
    return osg::Vec3(0.66, 0.78, 1.00);
}


const float StarMapGeode::setColorRatio(const float ratio)
{
    osg::Vec4 color;
    u_color->get(color);

    color[3] = ratio;
    u_color->set(color);

    return getColorRatio();
}

const float StarMapGeode::getColorRatio() const
{
    osg::Vec4 color;
    u_color->get(color);

    return color[3];
}

const float StarMapGeode::defaultColorRatio()
{
    return 0.33f;
}


const float StarMapGeode::setApparentMagnitude(const float vMag)
{
    m_apparentMagnitude = vMag;
    updateScaledB();

    return getApparentMagnitude();
}

const float StarMapGeode::getApparentMagnitude() const
{
    return m_apparentMagnitude;
}

const float StarMapGeode::defaultApparentMagnitude() 
{
    return 6.0f;
}


const float StarMapGeode::setScattering(const float scattering)
{
    u_scattering->set(scattering);
    return getScattering();
}

const float StarMapGeode::getScattering() const
{
    float scattering;
    u_scattering->get(scattering);

    return scattering;
}

const float StarMapGeode::defaultScattering()
{
    return 2.0f;
}




const std::string StarMapGeode::getVertexShaderSource()
{
    return glsl_version_150()

    +   glsl_quadRetrieveRay()
    +   glsl_quadTransform()
    
    +   PRAGMA_ONCE(main,

        "out vec4 m_eye;\n"
        "out vec4 m_ray;\n"
        "\n"
        "uniform mat4 R;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    m_eye = quadRetrieveRay();\n"
        "    m_ray = R * m_eye;\n"
        "    quadTransform();\n"
        "}");
}


const std::string StarMapGeode::getFragmentShaderSource()
{
    return glsl_version_150()

    +   glsl_cmn_uniform()
    +   glsl_horizon()
    +   glsl_scattering()

    +   PRAGMA_ONCE(main,

        "uniform vec3 sun;\n"
        "\n"
        "uniform vec4 color;\n"
        "uniform float q;\n"
        "\n"
        "uniform float deltaM;\n"
        "uniform float scattering;\n"
        "\n"
        "uniform samplerCube starmapCube;\n"
        "\n"
        "in vec4 m_eye;\n"
        "in vec4 m_ray;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    vec3 eye = normalize(m_eye.xyz);\n"
        "    vec3 stu = normalize(m_ray.xyz);\n"
        "\n"
        "    if(belowHorizon(eye))\n"
        "        discard;\n"
        "\n"
        "    vec4 fc = textureCube(starmapCube, stu);\n"
        "    fc *= 3e-2 / sqrt(q) * deltaM;\n"
        "\n"
        "    float omega = acos(eye.z * 0.9998);\n"
        "\n"
            // Day-Twilight-Night-Intensity Mapping (Butterworth-Filter)
        "    float b = 1.0 / sqrt(1 + pow(sun.z + 1.14, 32));\n"
        "\n"
        "    gl_FragColor = vec4(b * (fc.rgb - scatt(omega)), 1.0);\n"
        "}");
}




#ifdef OSGHIMMEL_EXPOSE_SHADERS

osg::Shader *StarMapGeode::getVertexShader()
{
    return m_vShader;
}
osg::Shader *StarMapGeode::getGeometryShader()
{
    return NULL;
}
osg::Shader *StarMapGeode::getFragmentShader()
{
    return m_fShader;
}

#endif // OSGHIMMEL_EXPOSE_SHADERS

} // namespace osgHimmel