
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


#include "cloudlayerhighgeode.h"

#include "himmelquad.h"

#include <osg/Texture2D>
#include <osg/Geode>
#include <osg/Depth>
#include <osg/BlendFunc>

#include <assert.h>




namespace
{
    // From // JAVA REFERENCE IMPLEMENTATION OF IMPROVED NOISE - COPYRIGHT 2002 KEN PERLIN. (http://mrl.nyu.edu/~perlin/noise/)
    // and (Improving Noise - Perlin - 2002) - http://mrl.nyu.edu/~perlin/paper445.pdf

    const static unsigned char perm[] = 
    { 
        151,160,137,91,90,15,
        131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
        190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
        88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
        77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
        102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
        135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
        5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
        223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
        129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
        251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
        49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
    };

    const static unsigned char grad[16][3] =
    {
            { 1, 1, 0}, {-1, 1, 0}, { 1,-1, 0}, {-1,-1, 0}
        ,   { 1, 0, 1}, {-1, 0, 1}, { 1, 0,-1}, {-1, 0,-1}
        ,   { 0, 1, 1}, { 0,-1, 1}, { 0, 1,-1}, { 0,-1,-1}
        ,   { 1, 1, 0}, {-1, 1, 0}, { 0,-1, 0}, { 0,-1,-1}
    };

}


void generate(
    const int width
,   const int height
,   unsigned char *dest)
{
    const unsigned int size = width * height;

    if(size < 1) 
        return;

    for(int i = 0; i < width; ++i)
        for(int j = 0; j < height; ++j)
        {
            const unsigned char p = /*rand() * */ perm[(perm[i & 255] + j) & 255];  // hash

            const int o = 4 * (i * width + j);

            dest[o + 0] = grad[p & 15][0] * 64 + 64;
            dest[o + 1] = grad[p & 15][1] * 64 + 64;
            dest[o + 2] = grad[p & 15][2] * 64 + 64;
            dest[o + 3] = p;
        }
}






CloudLayerHighGeode::CloudLayerHighGeode()
:   osg::Group()

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   m_hquad(new HimmelQuad())

,   u_perm(NULL)
{
    setName("CloudLayerHigh");

    osg::StateSet* stateSet = getOrCreateStateSet();

    setupNode(stateSet);
    setupUniforms(stateSet);
    setupShader(stateSet);
    setupTextures(stateSet);

    osg::Geode *geode = new osg::Geode;
    geode->addDrawable(m_hquad);
    addChild(geode);
};


CloudLayerHighGeode::~CloudLayerHighGeode()
{
};


void CloudLayerHighGeode::update(const Himmel &himmel)
{
}


void CloudLayerHighGeode::setupNode(osg::StateSet* stateSet)
{
    osg::Depth* depth = new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0);    
    stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

    osg::BlendFunc *blend  = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE);
    stateSet->setAttributeAndModes(blend, osg::StateAttribute::ON);
}


void CloudLayerHighGeode::setupShader(osg::StateSet* stateSet)
{
    m_vShader->setShaderSource(getVertexShaderSource());
    m_fShader->setShaderSource(getFragmentShaderSource());

    m_program->addShader(m_vShader);
    m_program->addShader(m_fShader);

    stateSet->setAttributeAndModes(m_program, osg::StateAttribute::ON);
}


#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER

osg::Shader *CloudLayerHighGeode::vertexShader()
{
    return m_vShader;
}
osg::Shader *CloudLayerHighGeode::geometryShader()
{
    return NULL;
}
osg::Shader *CloudLayerHighGeode::fragmentShader()
{
    return m_fShader;
}

#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER


void CloudLayerHighGeode::setupUniforms(osg::StateSet* stateSet)
{
    u_perm = new osg::Uniform("perm", 0);
    stateSet->addUniform(u_perm);
}


void CloudLayerHighGeode::setupTextures(osg::StateSet* stateSet)
{
    int w = 256;
    int h = 256;

    unsigned char *data = new unsigned char[w * h * 4];

    generate(w, h, data);

    osg::ref_ptr<osg::Image> permImage = new osg::Image();
    permImage->setImage(w, h, 1
        , GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE
        , data, osg::Image::USE_NEW_DELETE);

    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(permImage);

    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

    int unit;
    u_perm->get(unit);

    stateSet->setTextureAttributeAndModes(unit, texture, osg::StateAttribute::ON);
}




#include "shaderfragment/version.hpp"

// VertexShader

#include "shaderfragment/quadretrieveray.hpp"
#include "shaderfragment/quadtransform.hpp"
#include "shaderfragment/bruneton_common.hpp"

const std::string CloudLayerHighGeode::getVertexShaderSource()
{
    return glsl_version_150

    +   glsl_quadRetrieveRay
    +   glsl_quadTransform
    +
        "out vec4 m_ray;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    gl_TexCoord[0] = gl_Vertex * 0.5 + 0.5;\n"
        "\n"
        "    m_ray = quadRetrieveRay();\n"
        "    quadTransform();\n"
        "}\n\n";
}

#include "shaderfragment/pseudo_rand.hpp"
#include "shaderfragment/dither.hpp"

// FragmentShader

#include "shaderfragment/cloudlayer.hpp"
#include "shaderfragment/common.hpp"
#include "shaderfragment/noise.hpp"

const std::string CloudLayerHighGeode::getFragmentShaderSource()
{
    return glsl_version_150

    +   glsl_cmn_uniform
    +   glsl_horizon
    +   
        "uniform sampler2D perm;\n"

    +   glsl_cloud_layer_intersection
    +   glsl_fade
    +   glsl_noise2
    +
        "uniform float altitude = 8.0;\n" // TODO: make uniform.....!
        "\n"
        "in vec4 m_ray;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec3 d = normalize(m_ray.xyz);\n"
        "\n"
        "    if(belowHorizon(d))\n"
        "        discard;\n"
        "\n"
        "    float t = getLayerIntersectionOrDiscard(d, altitude);\n"
        "    vec2 st = t * d.xy * vec2(0.7, 1.0);\n"
        "\n"
        "    float n = 0;\n"
        "    float f = 0.08;\n"
        "\n"
        "    n += noise2(st *  0.125 * f) * (1 /  1.0);\n"
        "    n += noise2(st *  0.250 * f) * (1 /  2.0);\n"
        "    n += noise2(st *  0.500 * f) * (1 /  4.0);\n"
        "    n += noise2(st *  1.000 * f) * (1 /  8.0);\n"
        "    n += noise2(st *  2.000 * f) * (1 / 16.0);\n"
        "    //    n += noise2(st *  4.000 * f) * (1 / 32.0);\n"
        "        n += noise2(st *  8.000 * f) * (1 / 16.0);\n"
        "        n += noise2(st * 16.000 * f) * (1 / 16.0);\n"
        "        n += noise2(st * 32.000 * f) * (1 / 32.0);\n"
        "        n += noise2(st * 64.000 * f) * (1 / 32.0);\n"
        "\n"
        "    gl_FragColor = vec4(vec3(0.2), n);\n"
        "}\n\n";
}
