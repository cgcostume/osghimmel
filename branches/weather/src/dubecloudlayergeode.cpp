
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


#include "dubecloudlayergeode.h"

#include "highcloudlayergeode.h"
#include "noise.h"
#include "himmel.h"
#include "mathmacros.h"
#include "himmelquad.h"
#include  "timef.h"

#include "shaderfragment/common.h"
#include "shaderfragment/bruneton_common.h"
#include "shaderfragment/pseudo_rand.h"
#include "shaderfragment/dither.h"
#include "shaderfragment/cloudlayer.h"
#include "shaderfragment/noise.h"

#include <osg/Texture2D>
#include <osg/Texture3D>
#include <osg/Geode>
#include <osg/Depth>
#include <osg/BlendFunc>

#include <osgDB/WriteFile>

#include <assert.h>
#include "strutils.h"


namespace osgHimmel
{

DubeCloudLayerGeode::DubeCloudLayerGeode(const int texSize)
:   osg::Group()

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   m_hquad(new HimmelQuad())
,   m_preNoise(NULL)
,   m_noiseSize(texSize)

,   u_q(NULL)
,   u_clouds(NULL)
,   u_noise(NULL)
,   u_time(NULL)

,   u_coverage(NULL)
,   u_sharpness(NULL)
,   u_change(NULL)
,   u_wind(NULL)
,   u_altitude(NULL)
,   u_scale(NULL)
,   u_thickness(NULL)
,   u_offset(NULL)
,   u_tcolor(NULL)
,   u_bcolor(NULL)
{
    setName("DubéCloudLayer");

    osg::StateSet* stateSet = getOrCreateStateSet();

    setupNode(stateSet);
    setupUniforms(stateSet);
    setupShader(stateSet);
    setupTextures(stateSet);

    osg::Geode *geode = new osg::Geode;
    geode->addDrawable(m_hquad);
    addChild(geode);
};


DubeCloudLayerGeode::~DubeCloudLayerGeode()
{
};


void DubeCloudLayerGeode::update(const Himmel &himmel)
{
    // TODO: starmap and planets also require this ... - find better place
    const float fov = himmel.getCameraFovHint();
    const float height = himmel.getViewSizeHeightHint();

    u_q->set(static_cast<float>(sqrt(2.0) * 2.0 * tan(_rad(fov * 0.5)) / height));

    u_time->set(static_cast<float>(himmel.getTime()->getf()));
}


void DubeCloudLayerGeode::setupNode(osg::StateSet* stateSet)
{
    osg::Depth* depth = new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0);    
    stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

    osg::BlendFunc *blend  = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    stateSet->setAttributeAndModes(blend, osg::StateAttribute::ON);
}
void DubeCloudLayerGeode::setupShader(osg::StateSet* stateSet)
{
    m_vShader->setShaderSource(getVertexShaderSource());
    m_fShader->setShaderSource(getFragmentShaderSource());

    m_program->addShader(m_vShader);
    m_program->addShader(m_fShader);

    stateSet->setAttributeAndModes(m_program, osg::StateAttribute::ON);
}


void DubeCloudLayerGeode::setupUniforms(osg::StateSet *stateSet)
{
    u_q = new osg::Uniform("q", 0.0f);
    stateSet->addUniform(u_q);

    u_clouds = new osg::Uniform(osg::Uniform::SAMPLER_2D, "clouds");
    stateSet->addUniform(u_clouds);

    u_noise = new osg::Uniform(osg::Uniform::SAMPLER_3D, "nnoise");
    stateSet->addUniform(u_noise);

    u_time = new osg::Uniform("time", 0.f);
    stateSet->addUniform(u_time);

    u_coverage = new osg::Uniform("coverage", 0.2f);
    stateSet->addUniform(u_coverage);

    u_sharpness = new osg::Uniform("sharpness", 0.3f);
    stateSet->addUniform(u_sharpness);

    u_change = new osg::Uniform("change", defaultChange());
    stateSet->addUniform(u_change);

    u_wind = new osg::Uniform("wind", osg::Vec2f(0.f, 0.f));
    stateSet->addUniform(u_wind);

    u_altitude = new osg::Uniform("altitude", defaultAltitude());
    stateSet->addUniform(u_altitude);

    u_scale = new osg::Uniform("scale", defaultScale());
    stateSet->addUniform(u_scale);

    u_thickness = new osg::Uniform("thickness", 3.f);
    stateSet->addUniform(u_thickness);

    u_offset = new osg::Uniform("offset", -.5f);
    stateSet->addUniform(u_offset);

    u_bcolor = new osg::Uniform("bcolor", osg::Vec3f(1.f, 1.f, 1.f));
    stateSet->addUniform(u_bcolor);

    u_tcolor = new osg::Uniform("tcolor", osg::Vec3f(1.f, 1.f, 1.f));
    stateSet->addUniform(u_tcolor);


    u_noise0 = new osg::Uniform(osg::Uniform::SAMPLER_3D, "noise0");
    u_noise1 = new osg::Uniform(osg::Uniform::SAMPLER_3D, "noise1");
    u_noise2 = new osg::Uniform(osg::Uniform::SAMPLER_3D, "noise2");
    u_noise3 = new osg::Uniform(osg::Uniform::SAMPLER_3D, "noise3");
}


void DubeCloudLayerGeode::setupTextures(osg::StateSet* stateSet)
{
    srand(time(NULL));

    m_preNoise = new osg::Texture2D;

    osg::Group *preNoise(HighCloudLayerGeode::createPreRenderedNoise(m_noiseSize, m_preNoise));
    osg::StateSet *pnStateSet(preNoise->getOrCreateStateSet());

    // precompute tilable permutations
    // generate textures 128, 256, 512, 1024 with rank 8, 16, 32, 64

    // TODO: the use of m_noise as an array insead of a texture array was due to problems
    // with osg and not enough time and vigor to fix this.. :D

    m_noise[0] = HighCloudLayerGeode::createNoiseArray(1 << 6, 3, 4);
    m_noise[1] = HighCloudLayerGeode::createNoiseArray(1 << 7, 4, 4);
    m_noise[2] = HighCloudLayerGeode::createNoiseArray(1 << 8, 5, 4);
    m_noise[3] = HighCloudLayerGeode::createNoiseArray(1 << 8, 6, 4);

    pnStateSet->addUniform(u_time);

    pnStateSet->addUniform(u_noise0);
    u_noise0->set(1);
    pnStateSet->addUniform(u_noise1);
    u_noise1->set(2);
    pnStateSet->addUniform(u_noise2);
    u_noise2->set(3);
    pnStateSet->addUniform(u_noise3);
    u_noise3->set(4);

    pnStateSet->setTextureAttributeAndModes(1, m_noise[0]);
    pnStateSet->setTextureAttributeAndModes(2, m_noise[1]);
    pnStateSet->setTextureAttributeAndModes(3, m_noise[2]);
    pnStateSet->setTextureAttributeAndModes(4, m_noise[3]);


    addChild(preNoise);

    u_clouds->set(0);
    stateSet->setTextureAttributeAndModes(0, m_preNoise, osg::StateAttribute::ON);
    u_noise->set(1);
    stateSet->setTextureAttributeAndModes(1, m_noise[3], osg::StateAttribute::ON);
}


const float DubeCloudLayerGeode::setAltitude(const float altitude)
{
    u_altitude->set(altitude);
    return getAltitude();
}

const float DubeCloudLayerGeode::getAltitude() const
{
    float altitude;
    u_altitude->get(altitude);

    return altitude;
}

const float DubeCloudLayerGeode::defaultAltitude()
{
    return 2.f;
}


const float DubeCloudLayerGeode::setSharpness(const float sharpness)
{
    u_sharpness->set(sharpness);
    return getSharpness();
}

const float DubeCloudLayerGeode::getSharpness() const
{
    float sharpness;
    u_sharpness->get(sharpness);

    return sharpness;
}


const float DubeCloudLayerGeode::setCoverage(const float coverage)
{
    u_coverage->set(coverage);
    return getCoverage();
}

const float DubeCloudLayerGeode::getCoverage() const
{
    float coverage;
    u_coverage->get(coverage);

    return coverage;
}


const osg::Vec2f DubeCloudLayerGeode::setScale(const osg::Vec2f &scale)
{
    u_scale->set(scale);
    return getScale();
}

const osg::Vec2f DubeCloudLayerGeode::getScale() const
{
    osg::Vec2f scale;
    u_scale->get(scale);

    return scale;
}

const osg::Vec2f DubeCloudLayerGeode::defaultScale()
{
    return osg::Vec2f(128.f, 128.f);
}


const float DubeCloudLayerGeode::setChange(const float change)
{
    u_change->set(change);
    return getChange();
}

const float DubeCloudLayerGeode::getChange() const
{
    float change;
    u_change->get(change);

    return change;
}

const float DubeCloudLayerGeode::defaultChange()
{
    return 0.1f;
}


const osg::Vec2f DubeCloudLayerGeode::setWind(const osg::Vec2f &wind)
{
    u_wind->set(wind);
    return getWind();
}

const osg::Vec2f DubeCloudLayerGeode::getWind() const
{
    osg::Vec2f wind;
    u_wind->get(wind);

    return wind;
}


const osg::Vec3f DubeCloudLayerGeode::setBottomColor(const osg::Vec3f &color)
{
    u_bcolor->set(color);
    return getBottomColor();
}

const osg::Vec3f DubeCloudLayerGeode::getBottomColor() const
{
    osg::Vec3f color;
    u_bcolor->get(color);

    return color;
}


const osg::Vec3f DubeCloudLayerGeode::setTopColor(const osg::Vec3f &color)
{
    u_tcolor->set(color);
    return getTopColor();
}

const osg::Vec3f DubeCloudLayerGeode::getTopColor() const
{
    osg::Vec3f color;
    u_tcolor->get(color);

    return color;
}


const float DubeCloudLayerGeode::setThickness(const float thickness)
{
    u_thickness->set(thickness);
    return getThickness();
}

const float DubeCloudLayerGeode::getThickness() const
{
    float thickness;
    u_thickness->get(thickness);

    return thickness;
}


const float DubeCloudLayerGeode::setOffset(const float offset)
{
    u_offset->set(offset);
    return getOffset();
}

const float DubeCloudLayerGeode::getOffset() const
{
    float offset;
    u_offset->get(offset);

    return offset;
}


const std::string DubeCloudLayerGeode::getVertexShaderSource()
{
    return glsl_version_150()

    +   glsl_quadRetrieveRay()
    +   glsl_quadTransform()
    
    +   PRAGMA_ONCE(main,
    
        "out vec4 m_ray;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    gl_TexCoord[0] = gl_Vertex * 0.5 + 0.5;\n"
        "\n"
        "    m_ray = quadRetrieveRay();\n"
        "    quadTransform();\n"
        "}");
}


const std::string DubeCloudLayerGeode::getFragmentShaderSource()
{
//    const std::string n = Noise::noise2GlslSource(32u);

    return glsl_version_150()

    +   glsl_cmn_uniform()
    +   glsl_horizon()

    +   glsl_cloud_layer_intersection()

//    +   Noise::fadeGlslSource().c_str()
//    +   n.c_str()

    +   PRAGMA_ONCE(main,

        "uniform sampler2D clouds;\n"
        "uniform sampler3D nnoise;\n"
        "\n"
        "uniform float altitude;\n"
        "uniform float offset;\n"
        "uniform float thickness;\n"
        "\n"
        "uniform vec2 scale;\n"
        "\n"
        "uniform vec3 tcolor;\n"
        "uniform vec3 bcolor;\n"
        "\n"
        "const float sSteps = 128;\n"
        "const float sRange =  8;\n"
        "\n"
        "const int dSteps = 32;\n"
        "const float dRange = 1.9; \n"
        "\n"

        "uniform vec3 sunr;\n"
        "\n"
        "in vec4 m_ray;\n"
        "\n"

        "float T(vec2 uv)\n"
        "{\n"
        "    return texture2D(clouds, uv * scale).r;\n"
        "}\n\n"

        "float T(vec3 stu)\n"
        "{\n"
        "    float m = 2.0 * (1.0 + stu.z);\n"
        "    vec2 uv = vec2(stu.x / m + 0.5, stu.y / m + 0.5);\n"
        "\n"
        "    return T(uv);\n"
        "}\n\n"

        "float density(\n"
        "    in vec3 stu0\n"
        ",   in vec3 sun\n"
        ",   in float aa0)\n"
        "{\n"
        "    vec3 stu1 = stu0 + sun * dRange * thickness;\n"
        "\n"
        "    float iSteps = 1.0 / (dSteps - 1);\n"
        "    float iRange = dRange * thickness * iSteps;\n"
        "\n"
        "    vec3 Dstu = (stu1 - stu0) * iSteps;\n"
        "    vec3 stu  = stu0;\n"
        "\n"
        "    float d = 0.0;\n"
        "    float a1 = thickness + offset;\n"
        "\n"
        "    for(int i = 0; i < dSteps; ++i)\n"
        "    {\n"
        "        float t = T(stu);\n"
        "\n"
        "        float a = aa0 + i * iRange;\n"
        "\n"
        "        if(a > t * offset\n"
        "        && a < t * a1)\n"
        "            d += iSteps;\n"
        "\n"
        "        stu += Dstu;\n"
        "    }\n"
        "\n"
        "    return d;\n"
        "}\n\n"

        "float scatter(\n"
        "    in vec3 eye\n"
        ",   in vec3 sun\n"
        ",   out float d)\n"
        "{\n"
        "    vec3 o0 = vec3(0, 0, cmn[1] + cmn[0]);\n"
        "\n"
        "    // check if intersects with lower cloud sphere    \n"
        "    float t0, t1;\n"
        "\n"
        "    float a1 = thickness + offset;\n"
        "\n"
        "    if(!layerIntersection(eye, o0, altitude + offset, t0))\n"
        "        return 0.f;\n"
        "\n"
        "    if(!layerIntersection(eye, o0, altitude + a1, t1))\n"
        "        return 0.f;\n"
        "\n"
        "    // start t0 and end t1 through atm\n"
        "\n"
        "    d = 0.0;\n"
        "\n"
        "    vec3 stu0 = o0 + t0 * eye;\n"
        "    vec3 stu1 = o0 + t1 * eye;\n"
        "\n"
        "    float iSteps = 1.0 / (sSteps - 1);\n"
        "\n"
        "    vec3 Dstu = (stu1 - stu0) / (sSteps - 1);\n"
        "    vec3 stu  = stu0;\n"
        "\n"
        "    float s = 0.0;\n"
        "    float Da = thickness * iSteps;\n"
        "\n"
        "    for(int i = 0; i < sSteps; ++i)\n"
        "    {\n"
        "        float t = T(stu);\n"
        "\n"
        "        float a = offset + i * Da;\n"
        "\n"
        "        if(a > t * offset\n"
        "        && a < t * a1)\n"
        "        {\n"
        "            s += density(stu, sun, a);\n"
        "            ++d;\n"
        "        }\n"
        "        if(d >= sRange)\n"
        "            break;\n"
        "\n"
        "        stu += Dstu;\n"
        "    }\n"
        "    s /= sRange;\n"
        "    d /= sRange;\n"
        "\n"
        "    return s;\n"
        "}\n\n"

        "void main()\n"
        "{\n"
        "    vec3 eye = normalize(m_ray.xyz);\n"
        "\n"
        "    if(belowHorizon(eye))\n"
        "        discard;\n"
        "\n"
        "    vec3 sun = normalize(sunr);\n"
        "\n"
        "    float t;\n"
        "    vec3 o = vec3(0, 0, cmn[1] + cmn[0]);\n"
        "    layerIntersection(eye, o, altitude, t);\n"
        "\n"
        "    float d;\n"
        "    float s = scatter(eye, sun, d);\n"
        "    float f = 1 - s;\n"
        "\n"
        "    d *= (1.0 - pow(t, 0.8) * 12e-3);\n"
        "\n"
        "    gl_FragColor = vec4(mix(tcolor, bcolor, s) * f, d);\n"
        "}");
}




#ifdef OSGHIMMEL_EXPOSE_SHADERS

osg::Shader *DubeCloudLayerGeode::getVertexShader()
{
    return m_vShader;
}
osg::Shader *DubeCloudLayerGeode::getGeometryShader()
{
    return NULL;
}
osg::Shader *DubeCloudLayerGeode::getFragmentShader()
{
    return m_fShader;
}

#endif // OSGHIMMEL_EXPOSE_SHADERS

} // namespace osgHimmel