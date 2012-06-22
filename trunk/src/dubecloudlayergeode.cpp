
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
:   HighCloudLayerGeode(2048)
{
    setName("DubeCloudLayer");
};


DubeCloudLayerGeode::~DubeCloudLayerGeode()
{
};


void DubeCloudLayerGeode::update(const Himmel &himmel)
{
    HighCloudLayerGeode::update(himmel);
}


    osg::BlendFunc *blend  = new osg::BlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);

void DubeCloudLayerGeode::setupUniforms(osg::StateSet *stateSet)
{
    HighCloudLayerGeode::setupUniforms(stateSet);

    //u_q = new osg::Uniform("q", 0.0f);
    //stateSet->addUniform(u_q);
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