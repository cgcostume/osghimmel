
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

DubeCloudLayerGeode::DubeCloudLayerGeode()
:   osg::Group()

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   m_hquad(new HimmelQuad())
,   m_preNoise(NULL)

,   u_q(NULL)
,   u_preNoise(NULL)
,   u_time(NULL)
{
    setName("DubeCloudLayer");

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


osg::Group* DubeCloudLayerGeode::createPreRenderedNoise(
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

    const std::string fsrc = glsl_version_150()
        +   
            "uniform sampler3D noise0;"
            "uniform sampler3D noise1;"
            "uniform sampler3D noise2;"
            "uniform sampler3D noise3;"

            "uniform float time;"

            "void main()"
            "{"
            "   vec2 uv = gl_FragCoord.xy / 2048;"
            "   float n = 0;"
            
            //"   float t = time * 3600.0 * 0.25;"
            "   float t = time * 3600.0 * 0.25;"
            
            "   float speed = 0.1;"
            "   vec2  m = t * vec2(1.0, 0.0) * speed;" 
            
            "   n += 1.00000 * texture3D(noise0, vec3(uv     + m * 0.18, t * 0.01)).r;"
            "   n += 0.50000 * texture3D(noise1, vec3(uv     + m * 0.16, t * 0.02)).r;"
            "   n += 0.25000 * texture3D(noise2, vec3(uv     + m * 0.14, t * 0.04)).r;"
            "   n += 0.12500 * texture3D(noise3, vec3(uv     + m * 0.12, t * 0.08)).r;"
            "   n += 0.06750 * texture3D(noise3, vec3(uv * 2 + m * 0.10, t * 0.16)).r;"
            "   n += 0.03125 * texture3D(noise3, vec3(uv * 4 + m * 0.08, t * 0.32)).r;"
            "   n += 0.03125 * texture3D(noise3, vec3(uv * 8 + m * 0.08, t * 0.32)).r;"
            "   n *= 0.68;" // inverse sum 1/i^2 with i = 1 to 6
            
         //  "   float cover = 0.2;"
         //   "   float sharpness = 0.96;"

         //   "   n = max(n - cover, 0);"
	        //"   n = 1 - pow(sharpness, n * 255);"

            "   gl_FragColor = vec4(n);"
            "}";

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


osg::ref_ptr<osg::Image> DubeCloudLayerGeode::createNoiseSlice(
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


osg::Texture3D *DubeCloudLayerGeode::createNoiseArray(
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


void DubeCloudLayerGeode::update(const Himmel &himmel)
{
    // TODO: starmap and planets also require this ... - find better place
    const float fov = himmel.getCameraFovHint();
    const float height = himmel.getViewSizeHeightHint();

    //u_q->set(static_cast<float>(tan(_rad(fov / 2)) / (height * 0.5)));
    u_q->set(static_cast<float>(sqrt(2.0) * 2.0 * tan(_rad(fov * 0.5)) / height));

    u_time->set(static_cast<float>(himmel.getTime()->getf()));
}


void DubeCloudLayerGeode::setupNode(osg::StateSet* stateSet)
{
    osg::Depth* depth = new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0);    
    stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);
    
    osg::BlendFunc *blend  = new osg::BlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
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

    u_preNoise = new osg::Uniform("noise", 0);
    stateSet->addUniform(u_preNoise);

    u_time = new osg::Uniform("time", 0.f);
    stateSet->addUniform(u_time);

    u_noise0 = new osg::Uniform(osg::Uniform::SAMPLER_3D, "noise0");
    u_noise1 = new osg::Uniform(osg::Uniform::SAMPLER_3D, "noise1");
    u_noise2 = new osg::Uniform(osg::Uniform::SAMPLER_3D, "noise2");
    u_noise3 = new osg::Uniform(osg::Uniform::SAMPLER_3D, "noise3");
}


void DubeCloudLayerGeode::setupTextures(osg::StateSet* stateSet)
{
    srand(time(NULL));

    m_preNoise = new osg::Texture2D;

    osg::Group *preNoise(createPreRenderedNoise(2048, m_preNoise));
    osg::StateSet *pnStateSet(preNoise->getOrCreateStateSet());

    // precompute tilable permutations
    // generate textures 128, 256, 512, 1024 with rank 8, 16, 32, 64

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

    /*

    

#ifndef __version__
#define __version__

#version 150 compatibility

#endif // __version__


#ifndef __cmn__
#define __cmn__

uniform vec4 cmn;

#endif // __cmn__


#ifndef __cmn__
#define __cmn__

uniform vec4 cmn;

#endif // __cmn__


#ifndef __belowHorizon__
#define __belowHorizon__

float tAtm(vec3 ray)
{
    float r = cmn[1] + cmn[0];
    vec3 x = vec3(0.0, 0.0, r);

    vec3 v = normalize(ray);

    float mu = dot(x, v) / r;

    return r * mu - sqrt(r * r * (mu * mu - 1.0) + cmn[1] * cmn[1]);
}

bool belowHorizon(vec3 ray)
{
    if(ray.z > 0.0)
        return false;

   return tAtm(ray) < 0.0;
}

#endif // __belowHorizon__


#ifndef __layerIntersectionOrDiscard__
#define __layerIntersectionOrDiscard__

float layerIntersectionOrDiscard(
    const vec3 d
,   const float altitude)
{
    vec3  o = vec3(0.0, 0.0, cmn[1] + cmn[0]);
    float r = cmn[1] + altitude;

    if(o.z > r) 
        discard;

    float a = dot(d, d);
    float b = 2 * dot(d, o);
    float c = dot(o, o) - r * r;

    float B = b * b - 4 * a * c;
    if(B < 0)
        discard;

    B = sqrt(B);

    return (-b + B) * 0.5 / a;
}

#endif // __layerIntersectionOrDiscard__


#ifndef __main__
#define __main__


float layerIntersection(
    const vec3 d
,	const vec3 o
,   const float altitude)
{
    float r = cmn[1] + altitude;

    if(o.z > r) 
        discard;

    float a = dot(d, d);
    float b = 2 * dot(d, o);
    float c = dot(o, o) - r * r;

    float B2 = b * b - 4 * a * c;
	float B = sqrt(B2);

	float q = (-b + B) * 0.5;

	return q / a;
}

uniform sampler2D preNoise;
uniform float altitude = 1.0;

uniform vec3 sunr;

in vec4 m_ray;


float fetch(vec2 uv)
{
	float f = texture2D(preNoise, uv).r;
	
	float cover = 0.4;
    float sharpness = 0.98;

    f = max(f - cover, 0);
	f = 1 - pow(sharpness, f * 256);

	return f;
}


void main()
{
	vec3 eye = normalize(m_ray.xyz);

	if(belowHorizon(eye))
		discard;

	vec3 sun = eye;

	// position of observer
	vec3  o0  = vec3(0, 0, cmn[1] + cmn[0]);
	float t0  = layerIntersection(eye, o0, altitude - 0.5);

	float s = 1 - t0 * 4e-2;

	if(s < 0.01)
		discard;

	// intersection in eye direction on altitude
	vec3 stu0 = o0 + t0 * eye;

	// position of that intersection again
	vec3  o1  = o0; //stu0;
	float t1  = layerIntersection(sun, o1, altitude + 0.5);


	// last intersection of sun ray within atmosphere
	vec3 stu1 = o1 + t1 * sun;


	vec2 uv0 = atan(stu0.xy * 0.08);
	vec2 uv1 = atan(stu1.xy * 0.08);

	float f = fetch((uv1 - uv0) * 0.5 + uv0) * 0.5;

	if(f < 0.0)
		discard;


	int steps = 64;
	float Wuv = 1.f / steps;

	vec2 Duv = (uv1 - uv0) * Wuv;
	float Dstu = (length(stu1) - length(stu0)) * Wuv;

	float scattering = 0;

	vec2 uv;
	float t;
	float h;

	for(int i = 0; i < steps; ++i)
	{
		uv = uv0 + i * Duv;

		// current density on ray
		t = fetch(uv) * 0.5;

		// current height on ray
		h = i * Dstu * 0.99;

		scattering += step(0.5 - t, h) * step(h, 0.5 + t) * Wuv;
	}

	scattering = 1 / exp(scattering * 8);
	
	float f2 = pow(f, 1.2);

	gl_FragColor = vec4(vec3(scattering * f2), f2) * s;



}

#endif // __main__



    */
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