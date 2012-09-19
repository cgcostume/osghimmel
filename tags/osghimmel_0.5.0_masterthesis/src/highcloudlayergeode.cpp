
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


#include "highcloudlayergeode.h"

#include "noise.h"
#include "himmel.h"
#include "mathmacros.h"
#include "himmelquad.h"
#include "timef.h"

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

#include <assert.h>
#include "strutils.h"


namespace osgHimmel
{

HighCloudLayerGeode::HighCloudLayerGeode(const int texSize)
:   osg::Group()

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   m_hquad(new HimmelQuad())
,   m_preNoise(NULL)
,   m_noiseSize(texSize)

,   u_q(NULL)
,   u_preNoise(NULL)
,   u_time(NULL)

,   u_coverage(NULL)
,   u_sharpness(NULL)
,   u_change(NULL)
,   u_wind(NULL)
,   u_altitude(NULL)
,   u_scale(NULL)
,   u_color(NULL)
{
    setName("HighCloudLayer");

    osg::StateSet* stateSet = getOrCreateStateSet();

    setupNode(stateSet);
    setupUniforms(stateSet);
    setupShader(stateSet);
    setupTextures(stateSet);

    osg::Geode *geode = new osg::Geode;
    geode->addDrawable(m_hquad);
    addChild(geode);
};


HighCloudLayerGeode::~HighCloudLayerGeode()
{
};


osg::Group* HighCloudLayerGeode::createPreRenderedNoise(
    const unsigned texSize
,   osg::Texture2D *texture)
{
    assert(texture);


    // setup texture

    texture->setTextureSize(texSize, texSize);
    texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
    texture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);

    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

    texture->setInternalFormat(GL_LUMINANCE16F_ARB);
    texture->setSourceFormat(GL_LUMINANCE);
    texture->setSourceType(GL_FLOAT);


    // setup geode

    HimmelQuad *hquad = new HimmelQuad();
    osg::Geode* geode = new osg::Geode;
    geode->addDrawable(hquad);


    // setup camera

    osg::Camera* camera = new osg::Camera;

    camera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
    camera->setProjectionMatrixAsFrustum(-1, 1,-1, 1, 0.01, 2.00);

    camera->setViewport(0, 0, texSize, texSize);
    camera->setRenderOrder(osg::Camera::PRE_RENDER);

    camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    camera->attach(osg::Camera::COLOR_BUFFER, texture);

    camera->addChild(geode);


    // setup shaders

    std::string fsrc = glsl_version_150()

    +   PRAGMA_ONCE(main,

        "uniform sampler3D noise0;\n"
        "uniform sampler3D noise1;\n"
        "uniform sampler3D noise2;\n"
        "uniform sampler3D noise3;\n"
        "\n"
        "uniform float time;\n"
        "\n"
        "uniform float coverage;\n"
        "uniform float sharpness;\n"
        "\n"
        "uniform float change;\n"
        "uniform vec2  wind;\n"
        "\n"

        "void main()\n"
        "{\n"
        "   vec2 uv = gl_FragCoord.xy / %NOISE_SIZE%;\n"
        "   float n = 0;\n"
        "\n"
        "   float t = time * 3600.0;\n"
        "\n"
        "   vec2  m = t * wind;\n"
        "   t *= change;\n"
        "\n"
        "   n += 1.00000 * texture3D(noise0, vec3(uv     + m * 0.18, t * 0.01)).r;\n"
        "   n += 0.50000 * texture3D(noise1, vec3(uv     + m * 0.16, t * 0.02)).r;\n"
        "   n += 0.25000 * texture3D(noise2, vec3(uv     + m * 0.14, t * 0.04)).r;\n"
        "   n += 0.12500 * texture3D(noise3, vec3(uv     + m * 0.12, t * 0.08)).r;\n"
        "   n += 0.06750 * texture3D(noise3, vec3(uv * 2 + m * 0.10, t * 0.16)).r;\n"
        //"   n += 0.06125 * texture3D(noise3, vec3(uv * 4 + m * 0.08, t * 0.32)).r;\n"
//        "   n += 0.06125 * texture3D(noise2, vec3(uv * 8 + m * 0.06, t * 0.64)).r;\n"
        "   n *= 0.76;\n"
        "\n"
        "   n = n - 1 + coverage;\n"
        "   n /= coverage;\n"
        "   n = max(0.0, n);\n"
        "   n = pow(n, 1.0 - sharpness);\n"
        "\n"
        "   gl_FragColor = vec4(n);\n"
        "}");

    replace(fsrc, "%NOISE_SIZE%", static_cast<int>(texSize));

    const std::string vsrc = glsl_version_150()

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

    osg::Program *program = new osg::Program;

    program->addShader(new osg::Shader(osg::Shader::VERTEX, vsrc));
    program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fsrc));


    // setup group

    osg::Group *group = new osg::Group;
    group->getOrCreateStateSet()->setAttributeAndModes(program);

    group->addChild(camera);

    return group;
}


// TODO: move to noise class/area

osg::ref_ptr<osg::Image> HighCloudLayerGeode::createNoiseSlice(
    const unsigned int texSize
,   const unsigned int octave)
{
    const unsigned int size2 = texSize * texSize;
    const float oneOverTexSize = 1.f / static_cast<float>(texSize);

    Noise n(1 << (octave + 2), _randf(0.f, 1.f), _randf(0.f, 1.f));

    float *noise = new float[size2];
    unsigned int o;

    for(unsigned int s = 0; s < texSize; ++s)
    for(unsigned int t = 0; t < texSize; ++t)
    {
        o = t * texSize + s;
        noise[o] = n.noise2(s * oneOverTexSize, t * oneOverTexSize, octave) * 0.5 + 0.5;
    }

    osg::ref_ptr<osg::Image> image = new osg::Image();
    image->setImage(texSize, texSize, 1
        , GL_LUMINANCE16F_ARB, GL_LUMINANCE, GL_FLOAT
        , reinterpret_cast<unsigned char*>(noise), osg::Image::USE_NEW_DELETE);

    return image;
}


osg::Texture3D *HighCloudLayerGeode::createNoiseArray(
    const unsigned int texSize
,   const unsigned int octave
,   const unsigned int slices)
{
    osg::ref_ptr<osg::Image> image = new osg::Image();
    image->allocateImage(texSize, texSize, slices, GL_LUMINANCE, GL_FLOAT);

    for(unsigned int s = 0; s < slices; ++s)
        image->copySubImage(0, 0, s, createNoiseSlice(texSize, octave)); 

    osg::Texture3D *texture = new osg::Texture3D(image);

    texture->setUnRefImageDataAfterApply(true);

    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    texture->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT);

    return texture;
}


void HighCloudLayerGeode::update(const Himmel &himmel)
{
    // TODO: starmap and planets also require this ... - find better place
    const float fov = himmel.getCameraFovHint();
    const float height = himmel.getViewSizeHeightHint();

    u_q->set(static_cast<float>(sqrt(2.0) * 2.0 * tan(_rad(fov * 0.5)) / height));

    u_time->set(static_cast<float>(himmel.getTime()->getf()));
}


void HighCloudLayerGeode::setupNode(osg::StateSet* stateSet)
{
    osg::Depth* depth = new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0);    
    stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);
    
    osg::BlendFunc *blend  = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    stateSet->setAttributeAndModes(blend, osg::StateAttribute::ON);
}


void HighCloudLayerGeode::setupShader(osg::StateSet* stateSet)
{
    m_vShader->setShaderSource(getVertexShaderSource());
    m_fShader->setShaderSource(getFragmentShaderSource());

    m_program->addShader(m_vShader);
    m_program->addShader(m_fShader);

    stateSet->setAttributeAndModes(m_program, osg::StateAttribute::ON);
}


void HighCloudLayerGeode::setupUniforms(osg::StateSet *stateSet)
{
    u_q = new osg::Uniform("q", 0.0f);
    stateSet->addUniform(u_q);

    u_preNoise = new osg::Uniform("noise", 0);
    stateSet->addUniform(u_preNoise);

    u_time = new osg::Uniform("time", 0.f);
    stateSet->addUniform(u_time);

    u_coverage = new osg::Uniform("coverage", 0.2f);
    stateSet->addUniform(u_coverage);

    u_sharpness = new osg::Uniform("sharpness", 0.5f);
    stateSet->addUniform(u_sharpness);

    u_change = new osg::Uniform("change", defaultChange());
    stateSet->addUniform(u_change);

    u_wind = new osg::Uniform("wind", osg::Vec2(0.f, 0.f));
    stateSet->addUniform(u_wind);

    u_color = new osg::Uniform("color", osg::Vec3(1.f, 1.f, 1.f));
    stateSet->addUniform(u_color);

    u_altitude = new osg::Uniform("altitude", defaultAltitude());
    stateSet->addUniform(u_altitude);

    u_scale = new osg::Uniform("scale", defaultScale());
    stateSet->addUniform(u_scale);


    u_noise0 = new osg::Uniform(osg::Uniform::SAMPLER_3D, "noise0");
    u_noise1 = new osg::Uniform(osg::Uniform::SAMPLER_3D, "noise1");
    u_noise2 = new osg::Uniform(osg::Uniform::SAMPLER_3D, "noise2");
    u_noise3 = new osg::Uniform(osg::Uniform::SAMPLER_3D, "noise3");
}


void HighCloudLayerGeode::setupTextures(osg::StateSet* stateSet)
{
    srand(time(NULL));

    m_preNoise = new osg::Texture2D;

    osg::Group *preNoise(createPreRenderedNoise(m_noiseSize, m_preNoise));
    osg::StateSet *pnStateSet(preNoise->getOrCreateStateSet());

    // precompute tilable permutations
    // generate textures 128, 256, 512, 1024 with rank 8, 16, 32, 64

    // TODO: the use of m_noise as an array insead of a texture array was due to problems
    // with osg and not enough time and vigor to fix this.. :D

    m_noise[0] = createNoiseArray(1 << 6, 3, 4);
    m_noise[1] = createNoiseArray(1 << 7, 4, 4);
    m_noise[2] = createNoiseArray(1 << 8, 5, 4);
    m_noise[3] = createNoiseArray(1 << 8, 6, 4);

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

    u_preNoise->set(0);
    stateSet->setTextureAttributeAndModes(0, m_preNoise, osg::StateAttribute::ON);
}


const float HighCloudLayerGeode::setAltitude(const float altitude)
{
    u_altitude->set(altitude);
    return getAltitude();
}

const float HighCloudLayerGeode::getAltitude() const
{
    float altitude;
    u_altitude->get(altitude);

    return altitude;
}

const float HighCloudLayerGeode::defaultAltitude()
{
    return 8.f;
}


const float HighCloudLayerGeode::setSharpness(const float sharpness)
{
    u_sharpness->set(sharpness);
    return getSharpness();
}

const float HighCloudLayerGeode::getSharpness() const
{
    float sharpness;
    u_sharpness->get(sharpness);

    return sharpness;
}


const float HighCloudLayerGeode::setCoverage(const float coverage)
{
    u_coverage->set(coverage);
    return getCoverage();
}

const float HighCloudLayerGeode::getCoverage() const
{
    float coverage;
    u_coverage->get(coverage);

    return coverage;
}


const osg::Vec2 HighCloudLayerGeode::setScale(const osg::Vec2 &scale)
{
    u_scale->set(scale);
    return getScale();
}

const osg::Vec2 HighCloudLayerGeode::getScale() const
{
    osg::Vec2 scale;
    u_scale->get(scale);

    return scale;
}

const osg::Vec2 HighCloudLayerGeode::defaultScale()
{
    return osg::Vec2(32.f, 32.f);
}


const float HighCloudLayerGeode::setChange(const float change)
{
    u_change->set(change);
    return getChange();
}

const float HighCloudLayerGeode::getChange() const
{
    float change;
    u_change->get(change);

    return change;
}

const float HighCloudLayerGeode::defaultChange()
{
    return 0.1f;
}


const osg::Vec2 HighCloudLayerGeode::setWind(const osg::Vec2 &wind)
{
    u_wind->set(wind);
    return getWind();
}

const osg::Vec2 HighCloudLayerGeode::getWind() const
{
    osg::Vec2 wind;
    u_wind->get(wind);

    return wind;
}


const osg::Vec3 HighCloudLayerGeode::setColor(const osg::Vec3 &color)
{
    u_color->set(color);
    return getColor();
}

const osg::Vec3 HighCloudLayerGeode::getColor() const
{
    osg::Vec3 color;
    u_color->get(color);

    return color;
}


const std::string HighCloudLayerGeode::getVertexShaderSource()
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


const std::string HighCloudLayerGeode::getFragmentShaderSource()
{
    return glsl_version_150()

    +   glsl_cmn_uniform()
    +   glsl_horizon()

    +   glsl_cloud_layer_intersection()

    +   PRAGMA_ONCE(main,

        "uniform sampler2D preNoise;\n"
        "\n"
        "uniform float altitude;\n"
        "uniform vec2 scale;\n"
        "uniform vec3 color;\n"
        "\n"
        "uniform vec3 sunr;\n"
        "\n"
        "in vec4 m_ray;\n"
        "\n"


        "float T(vec2 uv)\n"
        "{\n"
        "    return texture2D(preNoise, uv * scale).r;\n"
        "}\n\n"


        "float T(vec3 stu)\n"
        "{\n"
        "    float m = 2.0 * (1.0 + stu.z);\n"
        "    vec2 uv = vec2(stu.x / m + 0.5, stu.y / m + 0.5);\n"
        "\n"
	    "    return T(uv);\n"
        "}\n\n"


        "void main()\n"
        "{\n"
        "    vec3 eye = normalize(m_ray.xyz);\n"
        "\n"
        "    if(belowHorizon(eye))\n"
        "       discard;\n"
        "\n"
        "    vec3 sun = normalize(sunr);\n"
        "\n"
        "    float t;\n"
        "    vec3 o = vec3(0, 0, cmn[1] + cmn[0]);\n"
        "    layerIntersection(eye, o, altitude, t);\n"
        "    float n = T(o + t * eye);\n"
        "\n"
        "    gl_FragColor = vec4(color, n);\n"
        "}");
}




#ifdef OSGHIMMEL_EXPOSE_SHADERS

osg::Shader *HighCloudLayerGeode::getVertexShader()
{
    return m_vShader;
}
osg::Shader *HighCloudLayerGeode::getGeometryShader()
{
    return NULL;
}
osg::Shader *HighCloudLayerGeode::getFragmentShader()
{
    return m_fShader;
}

#endif // OSGHIMMEL_EXPOSE_SHADERS

} // namespace osgHimmel