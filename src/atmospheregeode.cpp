
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

#include "himmel.h"
#include "shadermodifier.h"
#include "himmelquad.h"
#include "abstractastronomy.h"
#include "atmosphereprecompute.h"

#include <osg/Texture2D>
#include <osg/Texture3D>
#include <osg/Depth>
#include <osg/BlendFunc>

#include <osg/Geode>

#include <assert.h>


AtmosphereGeode::AtmosphereGeode()
:   osg::Group()

,   m_precompute(NULL)

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   m_hquad(new HimmelQuad())

,   m_transmittance(NULL)
,   m_irradiance(NULL)
,   m_inscatter(NULL)

,   u_sun(NULL)
,   u_altitude(NULL)
{
    setName("Atmosphere");

    m_scale = defaultSunScale();

    osg::StateSet* stateSet = getOrCreateStateSet();

    m_precompute = new AtmospherePrecompute();
    m_precompute->compute();

    setupNode(stateSet);
    setupUniforms(stateSet);
    setupShader(stateSet);
    setupTextures(stateSet);

//    precompute();

    osg::Geode *geode = new osg::Geode;
    geode->addDrawable(m_hquad);
    addChild(geode);
};


AtmosphereGeode::~AtmosphereGeode()
{
    delete m_precompute;
};


void AtmosphereGeode::update(const Himmel &himmel)
{
    osg::Vec3 sunv = himmel.astro()->getSunPosition();
    u_sun->set(osg::Vec4(sunv, himmel.astro()->getAngularSunRadius() * m_scale));

//    precompute();
}


void AtmosphereGeode::setupNode(osg::StateSet* stateSet)
{
    osg::Depth* depth = new osg::Depth(osg::Depth::ALWAYS);    
    stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

    osg::BlendFunc *blend  = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE);
    stateSet->setAttributeAndModes(blend, osg::StateAttribute::ON);
}


void AtmosphereGeode::setupShader(osg::StateSet* stateSet)
{
    m_vShader->setShaderSource(getVertexShaderSource());

    updateShader(stateSet);

    m_program->addShader(m_vShader);
    m_program->addShader(m_fShader);

    stateSet->setAttributeAndModes(m_program, osg::StateAttribute::ON);
}


void AtmosphereGeode::updateShader(osg::StateSet* stateSet)
{
    std::string fSource(getFragmentShaderSource());
    m_precompute->substituteMacros(fSource);

    m_fShader->setShaderSource(fSource);
}


#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER

osg::Shader *AtmosphereGeode::vertexShader()
{
    return m_vShader;
}
osg::Shader *AtmosphereGeode::geometryShader()
{
    return NULL;
}
osg::Shader *AtmosphereGeode::fragmentShader()
{
    return m_fShader;
}

#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER


void AtmosphereGeode::setupUniforms(osg::StateSet* stateSet)
{
    u_sun = new osg::Uniform("sun", osg::Vec4(1.0, 0.0, 0.0, 1.0)); // [3] = apparent angular radius (not diameter!)
    stateSet->addUniform(u_sun);

    u_altitude = new osg::Uniform("altitude", defaultAltitude());
    stateSet->addUniform(u_altitude);
}


void AtmosphereGeode::setupTextures(osg::StateSet* stateSet)
{
    assert(m_precompute);

    m_transmittance = m_precompute->getTransmittanceTexture();
    stateSet->setTextureAttributeAndModes(0, m_transmittance);

    m_irradiance = m_precompute->getIrradianceTexture();
    stateSet->setTextureAttributeAndModes(1, m_irradiance);

    m_inscatter = m_precompute->getInscatterTexture();
    stateSet->setTextureAttributeAndModes(2, m_inscatter);

    stateSet->addUniform(new osg::Uniform("transmittanceSampler", 0));
    stateSet->addUniform(new osg::Uniform("irradianceSampler", 1));
    stateSet->addUniform(new osg::Uniform("inscatterSampler", 2));
}


//void AtmosphereGeode::precompute()
//{
//    m_precompute->compute();
//    updateShader(getOrCreateStateSet());
//}


const float AtmosphereGeode::setSunScale(const float scale)
{
    osg::Vec4 temp;
    u_sun->get(temp);

    temp._v[3] = temp._v[3] / m_scale * scale;
    u_sun->set(temp);

    m_scale = scale;

    return getSunScale();
}

const float AtmosphereGeode::getSunScale() const
{
    return m_scale;
}

const float AtmosphereGeode::defaultSunScale()
{
    return 4.f;
}


const float AtmosphereGeode::setAltitude(const float altitude)
{
    u_altitude->set(altitude);
    return getAltitude();
}

const float AtmosphereGeode::getAltitude() const
{
    float altitude;
    u_altitude->get(altitude);
    return altitude;
}

const float AtmosphereGeode::defaultAltitude()
{
    return 0.2f;
}


#include "shaderfragment/version.hpp"

// VertexShader

#include "shaderfragment/quadretrieveray.hpp"
#include "shaderfragment/quadtransform.hpp"
#include "shaderfragment/bruneton_common.hpp"

const std::string AtmosphereGeode::getVertexShaderSource()
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

// FragmentShader

const std::string AtmosphereGeode::getFragmentShaderSource()
{
    return glsl_version_150
  
    +   glsl_bruneton_const_RgRtRL
    +   glsl_bruneton_const_RSize
    +   glsl_bruneton_const_R
    +   glsl_bruneton_const_M
    +   glsl_bruneton_const_PI


    +   "in vec4 m_ray;\n"
        "\n"
        "const float ISun = 100.0;\n"
        "\n"
        //"uniform vec3 c = vec3(Rg + 0.2, 0.0, 0.0);\n"
        "uniform float altitude;\n"
        "uniform vec4 sun;\n"
        "uniform float exposure = 0.4;\n"
        "\n"

        //"uniform sampler2D reflectanceSampler;\n" // ground reflectance texture
        "uniform sampler2D irradianceSampler;\n"    // precomputed skylight irradiance (E table)
        "uniform sampler3D inscatterSampler;\n"     // precomputed inscattered light (S table)
        "\n"

    +   glsl_bruneton_opticalDepth
    +   glsl_bruneton_transmittanceUV
    +   glsl_bruneton_transmittance
    +   glsl_bruneton_transmittanceWithShadow
    +   glsl_bruneton_analyticTransmittance
    +   glsl_bruneton_irradianceUV
    +   glsl_bruneton_irradiance
    +   glsl_bruneton_texture4D
    +   glsl_bruneton_phaseFunctionR
    +   glsl_bruneton_phaseFunctionM
    +   glsl_bruneton_mie
    +   glsl_bruneton_hdr

    +
        // inscattered light along ray x+tv, when sun in direction s (=S[L]-T(x,x0)S[L]|x0)
        "vec3 inscatter(inout vec3 x, inout float t, vec3 v, vec3 s, out float r, out float mu, out vec3 attenuation) {\n"
        "    vec3 result;\n"
        "    r = length(x);\n"
        "    mu = dot(x, v) / r;\n"
        "    float d = -r * mu - sqrt(r * r * (mu * mu - 1.0) + Rt * Rt);\n"
        "    if (d > 0.0) {\n" // if x in space and ray intersects atmosphere
                // move x to nearest intersection of ray with top atmosphere boundary
        "        x += d * v;\n"
        "        t -= d;\n"
        "        mu = (r * mu + d) / Rt;\n"
        "        r = Rt;\n"
        "    }\n"
        "    if (r <= Rt) {\n" // if ray intersects atmosphere
        "        float nu = dot(v, s);\n"
        "        float muS = dot(x, s) / r;\n"
        "        float phaseR = phaseFunctionR(nu);\n"
        "        float phaseM = phaseFunctionM(nu);\n"
        "        vec4 inscatter = max(texture4D(inscatterSampler, r, mu, muS, nu), 0.0);\n"
        "        if (t > 0.0) {\n"
        "            vec3 x0 = x + t * v;\n"
        "            float r0 = length(x0);\n"
        "            float rMu0 = dot(x0, v);\n"
        "            float mu0 = rMu0 / r0;\n"
        "            float muS0 = dot(x0, s) / r0;\n"
        //"#ifdef FIX\n"
                    // avoids imprecision problems in transmittance computations based on textures
        "            attenuation = analyticTransmittance(r, mu, t);\n"
        //"#else\n"
        //"            attenuation = transmittance(r, mu, v, x0);
        //"#endif\n"
        "            if (r0 > Rg + 0.01) {\n"
                        // computes S[L]-T(x,x0)S[L]|x0
        "                inscatter = max(inscatter - attenuation.rgbr * texture4D(inscatterSampler, r0, mu0, muS0, nu), 0.0);\n"
        //"#ifdef FIX\n"
                        // avoids imprecision problems near horizon by interpolating between two points above and below horizon
        "                const float EPS = 0.004;\n"
        "                float muHoriz = -sqrt(1.0 - (Rg / r) * (Rg / r));\n"
        "                if (abs(mu - muHoriz) < EPS) {\n"
        "                    float a = ((mu - muHoriz) + EPS) / (2.0 * EPS);\n"
        "\n"
        "                    mu = muHoriz - EPS;\n"
        "                    r0 = sqrt(r * r + t * t + 2.0 * r * t * mu);\n"
        "                    mu0 = (r * mu + t) / r0;\n"
        "                    vec4 inScatter0 = texture4D(inscatterSampler, r, mu, muS, nu);\n"
        "                    vec4 inScatter1 = texture4D(inscatterSampler, r0, mu0, muS0, nu);\n"
        "                    vec4 inScatterA = max(inScatter0 - attenuation.rgbr * inScatter1, 0.0);\n"
        "\n"
        "                    mu = muHoriz + EPS;\n"
        "                    r0 = sqrt(r * r + t * t + 2.0 * r * t * mu);\n"
        "                    mu0 = (r * mu + t) / r0;\n"
        "                    inScatter0 = texture4D(inscatterSampler, r, mu, muS, nu);\n"
        "                    inScatter1 = texture4D(inscatterSampler, r0, mu0, muS0, nu);\n"
        "                    vec4 inScatterB = max(inScatter0 - attenuation.rgbr * inScatter1, 0.0);\n"
        "\n"
        "                    inscatter = mix(inScatterA, inScatterB, a);\n"
        "                }\n"
        //"#endif\n"
        "            }\n"
        "        }\n"
        //"#ifdef FIX\n"
                // avoids imprecision problems in Mie scattering when sun is below horizon
        "        inscatter.w *= smoothstep(0.00, 0.02, muS);\n"
        //"#endif\n"
        "        result = max(inscatter.rgb * phaseR + getMie(inscatter) * phaseM, 0.0);\n"
        "    } else {\n" // x in space and ray looking in space
        "        result = vec3(0.0);\n"
        "    }\n"
        "    return result * ISun;\n"
        "}\n"
        "\n"

        // ground radiance at end of ray x+tv, when sun in direction s
        // attenuated bewteen ground and viewer (=R[L0]+R[L*])

        "vec3 groundColor(vec3 x, float t, vec3 v, vec3 s, float r, float mu, vec3 attenuation)\n"
        "{\n"
        "    vec3 result;\n"
        "    if (t > 0.0) {\n" // if ray hits ground surface
        "        // ground reflectance at end of ray, x0\n"
        "        vec3 x0 = x + t * v;\n"
        "        float r0 = length(x0);\n"
        "        vec3 n = x0 / r0;\n"
        //"        vec2 coords = vec2(atan(n.y, n.x), acos(n.z)) * vec2(0.5, 1.0) / M_PI + vec2(0.5, 0.0);\n"
        //"        vec4 reflectance = texture2D(reflectanceSampler, coords) * vec4(0.2, 0.2, 0.2, 1.0);\n"
        "        vec4 reflectance = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "        if (r0 > Rg + 0.01) {\n"
        "            reflectance = vec4(0.4, 0.4, 0.4, 0.0);\n"
        "        }\n"
        "\n"
                // direct sun light (radiance) reaching x0
        "        float muS = dot(n, s);\n"
        "        vec3 sunLight = transmittanceWithShadow(r0, muS);\n"
        "\n"
                // precomputed sky light (irradiance) (=E[L*]) at x0
        "        vec3 groundSkyLight = irradiance(irradianceSampler, r0, muS);\n"
        "\n"
                // light reflected at x0 (=(R[L0] + R[L*]) / T(x, x0))
        "        vec3 groundColor = reflectance.rgb * (max(muS, 0.0) * sunLight + groundSkyLight) * ISun / M_PI;\n"
        "\n"
        //"        // water specular color due to sunLight\n"
        //"        if (reflectance.w > 0.0) {\n"
        //"            vec3 h = normalize(s - v);\n"
        //"            float fresnel = 0.02 + 0.98 * pow(1.0 - dot(-v, h), 5.0);\n"
        //"            float waterBrdf = fresnel * pow(max(dot(h, n), 0.0), 150.0);\n"
        //"            groundColor += reflectance.w * max(waterBrdf, 0.0) * sunLight * ISun;\n"
        //"        }\n"
        //"\n"
        "        result = attenuation * groundColor;\n" // = R[L0] + R[L*]
        "    } else { // ray looking at the sky\n"
        "        result = vec3(0.0);\n"
        "    }\n"
        "    return result;\n"
        "}\n"
        "\n"

        // direct sun light for ray x+tv, when sun in direction s (=L0)
        "vec3 sunColor(vec3 x, float t, vec3 v, vec3 s, float r, float mu) {\n"
        "    if (t > 0.0) {\n"
        "        return vec3(0.0);\n"
        "    } else {\n"
        "        vec3 transmittance = r <= Rt ? transmittanceWithShadow(r, mu) : vec3(1.0);\n" // T(x, xo)
        "        float isun = step(cos(sun.a), dot(v, s)) * ISun;\n" // Lsun
        "        return transmittance * isun;\n" // Eq (9)
        "    }\n"
        "}\n"
        "\n"

        "void main() {\n"
        "    vec3 x = vec3(Rg + altitude, 0.0, 0.0);\n"
        "    vec3 v = normalize(m_ray.zyx);\n"
        "    vec3 s = normalize(sun.zyx);\n"
        "\n"
        "    float r = length(x);\n"
        "    float mu = dot(x, v) / r;\n"
        "    float t = -r * mu - sqrt(r * r * (mu * mu - 1.0) + Rg * Rg);\n"
        "\n"
        "    vec3 g = x - vec3(0.0, 0.0, Rg + 10.0);\n"
        "    float a = v.x * v.x + v.y * v.y - v.z * v.z;\n"
        "    float b = 2.0 * (g.x * v.x + g.y * v.y - g.z * v.z);\n"
        "    float c = g.x * g.x + g.y * g.y - g.z * g.z;\n"
        "    float d = -(b + sqrt(b * b - 4.0 * a * c)) / (2.0 * a);\n"
        "    bool cone = d > 0.0 && abs(x.z + d * v.z - Rg) <= 10.0;\n"
        "\n"
        "    if (t > 0.0) {\n"
        "        if (cone && d < t) {\n"
        "            t = d;\n"
        "        }\n"
        "    } else if (cone) {\n"
        "        t = d;\n"
        "    }\n"
        "\n"
        "    vec3 attenuation;\n"
        "    vec3 inscatterColor = inscatter(x, t, v, s, r, mu, attenuation);\n" // S[L]  - T(x, xs) S[l] | xs"
        "    vec3 groundColor = groundColor(x, t, v, s, r, mu, attenuation);\n"  // R[L0] + R[L*]
        "    vec3 sunColor = sunColor(x, t, v, s, r, mu);\n" // L0
        "    gl_FragColor = vec4(HDR(sunColor + groundColor + inscatterColor), 1.0);\n" // Eq (16)
        "}\n";
}