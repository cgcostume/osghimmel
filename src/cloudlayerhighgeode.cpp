
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

#include "noise.h"
#include "himmelquad.h"

#include "shaderfragment/common.h"
#include "shaderfragment/bruneton_common.h"
#include "shaderfragment/pseudo_rand.h"
#include "shaderfragment/dither.h"
#include "shaderfragment/cloudlayer.h"
#include "shaderfragment/noise.h"

#include <osg/Texture2D>
#include <osg/Geode>
#include <osg/Depth>
#include <osg/BlendFunc>

#include <assert.h>


namespace osgHimmel
{

CloudLayerHighGeode::CloudLayerHighGeode()
:   osg::Group()

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   m_hquad(new HimmelQuad())

,   u_perm(NULL)
//,   u_perlin(NULL)
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


void CloudLayerHighGeode::update(const Himmel &)
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




void CloudLayerHighGeode::setupUniforms(osg::StateSet* stateSet)
{
    u_perm = new osg::Uniform("perm", 0);
    stateSet->addUniform(u_perm);

    //u_perlin = new osg::Uniform("perlin", 1);
    //stateSet->addUniform(u_perlin);
}


void CloudLayerHighGeode::setupTextures(osg::StateSet* stateSet)
{
    unsigned int s(256);
    Noise n(s);

    unsigned char *perm = new unsigned char[s * s * 4];
    n.generatePermutationMap(perm);

    osg::ref_ptr<osg::Image> permImage = new osg::Image();
    permImage->setImage(s, s, 1
        , GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE
        , perm, osg::Image::USE_NEW_DELETE);

//    osgDB::writeImageFile(*permImage, "D:/_perm.png");

    osg::ref_ptr<osg::Texture2D> t1 = new osg::Texture2D(permImage);

    t1->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
    t1->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
    t1->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    t1->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

    int unit;
    u_perm->get(unit);

    stateSet->setTextureAttributeAndModes(unit, t1, osg::StateAttribute::ON);

/*
    s = 256;
    float si = 1.f / static_cast<float>(s);

    unsigned char *perlin = new unsigned char[(s + 1) * (s + 1)];

    for(int x = 0; x < s; ++x)
        for(int y = 0; y < s; ++y)
        {
            unsigned int i = (y * s) + x;
            perlin[i] = n.noise2(x * 0.05f, y * 0.05f) * 127.5 + 127.5;
        }
         
    osg::ref_ptr<osg::Image> perlinImage = new osg::Image();
    perlinImage->setImage(s, s, 1
        , GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE
        , perlin, osg::Image::USE_NEW_DELETE);

    osgDB::writeImageFile(*perlinImage, "D:/_perlin.png");

    osg::ref_ptr<osg::Texture2D> t2 = new osg::Texture2D(perlinImage);

    t2->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
    t2->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
    t2->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
    t2->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);

    u_perlin->get(unit);

    stateSet->setTextureAttributeAndModes(unit, t2, osg::StateAttribute::ON);*/

}




const std::string CloudLayerHighGeode::getVertexShaderSource()
{
    return glsl_version_150

    +   glsl_quadRetrieveRay
    +   glsl_quadTransform
    
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


const std::string CloudLayerHighGeode::getFragmentShaderSource()
{
    return glsl_version_150

    +   glsl_cmn_uniform
    +   glsl_horizon

    +   glsl_cloud_layer_intersection

    +   Noise::fadeGlslSource()
    +   Noise::noise2GlslSource(256u)

    +   PRAGMA_ONCE(main,

        "uniform sampler2D perm;\n"
        "uniform float altitude = 8.0;\n" // TODO: make uniform.....!
        "\n"
        "in vec4 m_ray;\n"
        "\n"
        "uniform sampler2D perlin;\n"

        "\n"
        "void main()\n"
        "{\n"
        //"    vec3 d = normalize(m_ray.xyz);\n"
        //"\n"
        //"    if(belowHorizon(d))\n"
        //"        discard;\n"
        //"\n"
        //"    float t = getLayerIntersectionOrDiscard(d, altitude);\n"
        //"    vec2 st = t * d.xy * vec2(0.7, 1.0);\n"
        
        "    float n = 0;\n"
        "\n"
        "    int octave = 1;\n"
        "    int i = int(log(0.25 * 256.0 / float(octave)) / log(2.0));\n"
        "\n"
        "    for(int j = 0; j < i; ++j)\n"
        "        n += noise2(gl_FragCoord.xy * pow(2.0, float(octave + j)) / 256) / pow(2.0, float(j));\n"
        "\n"
        "    gl_FragColor = vec4(vec3(n) * 0.5 + 0.5, 1);\n"
        "}");
}




#ifdef OSGHIMMEL_EXPOSE_SHADERS

osg::Shader *CloudLayerHighGeode::getVertexShader()
{
    return m_vShader;
}
osg::Shader *CloudLayerHighGeode::getGeometryShader()
{
    return NULL;
}
osg::Shader *CloudLayerHighGeode::getFragmentShader()
{
    return m_fShader;
}

#endif // OSGHIMMEL_EXPOSE_SHADERS

} // namespace osgHimmel