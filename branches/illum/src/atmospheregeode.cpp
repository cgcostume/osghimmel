
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
#include "himmelquad.h"
#include "abstractastronomy.h"
#include "atmosphereprecompute.h"

#include "shaderfragment/common.h"
#include "shaderfragment/bruneton_common.h"
#include "shaderfragment/pseudo_rand.h"
#include "shaderfragment/dither.h"

#include <osg/Texture2D>
#include <osg/Texture3D>
#include <osg/Depth>
#include <osg/BlendFunc>
#include <osg/Geode>

#include <assert.h>


namespace osgHimmel
{

AtmosphereGeode::AtmosphereGeode()
:   osg::Geode()

,   m_precompute(NULL)

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   m_hquad(new HimmelQuad())

,   m_transmittance(NULL)
,   m_irradiance(NULL)
,   m_inscatter(NULL)

,   u_sunScale(NULL)
,   u_lheurebleue(NULL)
,   u_exposure(NULL)
{
    setName("Atmosphere");

    m_scale = defaultSunScale();

    osg::StateSet* stateSet = getOrCreateStateSet();

    m_precompute = new AtmospherePrecompute();

    setupNode(stateSet);
    setupUniforms(stateSet);
    setupShader(stateSet);
    setupTextures(stateSet);

    precompute();

    addDrawable(m_hquad);
};


AtmosphereGeode::~AtmosphereGeode()
{
    delete m_precompute;
};


void AtmosphereGeode::update(const Himmel &himmel)
{
    u_sunScale->set(himmel.astro()->getAngularSunRadius() * m_scale);

    precompute();
}


void AtmosphereGeode::setupNode(osg::StateSet* stateSet)
{
    osg::Depth* depth = new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0);    
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


void AtmosphereGeode::updateShader(osg::StateSet*)
{
    std::string fSource(getFragmentShaderSource());
    m_precompute->substituteMacros(fSource);

    m_fShader->setShaderSource(fSource);
}


void AtmosphereGeode::setupUniforms(osg::StateSet* stateSet)
{
    u_sunScale = new osg::Uniform("sunScale", 1.f); // apparent angular radius (not diameter!)
    stateSet->addUniform(u_sunScale);

    u_exposure = new osg::Uniform("exposure", defaultExposure());
    stateSet->addUniform(u_exposure);

    const osg::Vec3f hb(defaultLHeureBleueColor());

    u_lheurebleue = new osg::Uniform("lheurebleue", osg::Vec4f(hb[0], hb[1], hb[2], defaultLHeureBleueIntensity()));
    stateSet->addUniform(u_lheurebleue);
    
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


void AtmosphereGeode::precompute()
{   
    if(m_precompute->compute())
        updateShader(getOrCreateStateSet());
}


const float AtmosphereGeode::setSunScale(const float scale)
{
    float temp;
    u_sunScale->get(temp);

    temp = temp / m_scale * scale;
    u_sunScale->set(temp);

    m_scale = scale;

    return getSunScale();
}

const float AtmosphereGeode::getSunScale() const
{
    return m_scale;
}

const float AtmosphereGeode::defaultSunScale()
{
    return 2.f;
}


const float AtmosphereGeode::setExposure(const float exposure)
{
    u_exposure->set(exposure);
    return getExposure();
}

const float AtmosphereGeode::getExposure() const
{
    float exposure;
    u_exposure->get(exposure);
    return exposure;
}

const float AtmosphereGeode::defaultExposure()
{
    return 0.22f;
}


const osg::Vec3f AtmosphereGeode::setLHeureBleueColor(const osg::Vec3f &color)
{
    osg::Vec4f temp;
    u_lheurebleue->get(temp);

    temp[0] = color[0];
    temp[1] = color[1];
    temp[2] = color[2];

    u_lheurebleue->set(temp);

    return getLHeureBleueColor();
}

const osg::Vec3f AtmosphereGeode::getLHeureBleueColor() const
{
    osg::Vec4f temp;
    u_lheurebleue->get(temp);
    return osg::Vec3f(temp[0], temp[1], temp[2]);
}

const osg::Vec3f AtmosphereGeode::defaultLHeureBleueColor()
{
    return osg::Vec3f(0.08, 0.3, 1.0);
}


const float AtmosphereGeode::setLHeureBleueIntensity(const float intensity)
{
    osg::Vec4f temp;
    u_lheurebleue->get(temp);

    temp[3] = intensity;
    u_lheurebleue->set(temp);

    return getLHeureBleueIntensity();
}

const float AtmosphereGeode::getLHeureBleueIntensity() const
{
    osg::Vec4f temp;
    u_lheurebleue->get(temp);
    return temp[3];
}

const float AtmosphereGeode::defaultLHeureBleueIntensity()
{
    return 0.5f;
}

void AtmosphereGeode::setAverageGroundReflectance(const float reflectance)
{
    m_precompute->getModelConfig().avgGroundReflectance = reflectance;
    m_precompute->dirty();
}

void AtmosphereGeode::setThicknessRayleigh(const float thickness)
{
    m_precompute->getModelConfig().HR = thickness;
    m_precompute->dirty();
}

void AtmosphereGeode::setScatteringRayleigh(const osg::Vec3f &coefficients)
{
    m_precompute->getModelConfig().betaR = coefficients;
    m_precompute->dirty();
}

void AtmosphereGeode::setThicknessMie(const float thickness)
{
    m_precompute->getModelConfig().HM = thickness;
    m_precompute->dirty();
}

void AtmosphereGeode::setScatteringMie(const float coefficient)
{
    m_precompute->getModelConfig().betaMSca = osg::Vec3f(1, 1, 1) * coefficient;
    m_precompute->getModelConfig().betaMEx = m_precompute->getModelConfig().betaMSca / 0.9;
    m_precompute->dirty();
}

void AtmosphereGeode::setPhaseG(const float g)
{
    m_precompute->getModelConfig().mieG = g;
    m_precompute->dirty();
}




const std::string AtmosphereGeode::getVertexShaderSource()
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


const std::string AtmosphereGeode::getFragmentShaderSource()
{
    return glsl_version_150()

    +   glsl_cmn_uniform()
    +   
        "uniform vec3 sun;\n"
        "uniform vec3 sunr;\n"

    +   glsl_pseudo_rand()
    +   glsl_dither()

    +   glsl_bruneton_const_RSize()
    +   glsl_bruneton_const_R()
    +   glsl_bruneton_const_M()
    +   glsl_bruneton_const_PI()


    +   "in vec4 m_ray;\n"
        "\n"
        "const float ISun = 100.0;\n"
        "\n"
        //"uniform vec3 c = vec3(cmn[1] + 0.2, 0.0, 0.0);\n"
        "uniform float sunScale;\n"
        "uniform float exposure;\n"
        "\n"
        "uniform vec4 lheurebleue;\n" // rgb and w for instensity
        "\n"

        //"uniform sampler2D reflectanceSampler;\n" // ground reflectance texture
        "uniform sampler2D irradianceSampler;\n"    // precomputed skylight irradiance (E table)
        "uniform sampler3D inscatterSampler;\n"     // precomputed inscattered light (S table)
        "\n"

    +   glsl_bruneton_opticalDepth()
    +   glsl_bruneton_transmittanceUV()
    +   glsl_bruneton_transmittance()
    +   glsl_bruneton_transmittanceWithShadow()
    +   glsl_bruneton_analyticTransmittance()
    +   glsl_bruneton_irradianceUV()
    +   glsl_bruneton_irradiance()
    +   glsl_bruneton_texture4D()
    +   glsl_bruneton_phaseFunctionR()
    +   glsl_bruneton_phaseFunctionM()
    +   glsl_bruneton_mie()
    +   glsl_bruneton_hdr()

    +
        // inscattered light along ray x+tv, when sun in direction s (=S[L]-T(x,x0)S[L]|x0)
        "vec3 inscatter(inout vec3 x, inout float t, vec3 v, vec3 s, out float r, out float mu, out vec3 attenuation) {\n"
        "    vec3 result;\n"
        "    r = length(x);\n"
        "    mu = dot(x, v) / r;\n"
        "    float d = -r * mu - sqrt(r * r * (mu * mu - 1.0) + cmn[2] * cmn[2]);\n"
        "    if (d > 0.0) {\n" // if x in space and ray intersects atmosphere
                // move x to nearest intersection of ray with top atmosphere boundary
        "        x += d * v;\n"
        "        t -= d;\n"
        "        mu = (r * mu + d) / cmn[2];\n"
        "        r = cmn[2];\n"
        "    }\n"
        "    if (r <= cmn[2]) {\n" // if ray intersects atmosphere
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
        //"            attenuation = analyticTransmittance(r, mu, t);\n"
        //"#else\n"
        "            attenuation = transmittance(r, mu, v, x0);\n"
        //"#endif\n"
        "            if (r0 > cmn[1] + 0.01) {\n"
                        // computes S[L]-T(x,x0)S[L]|x0
        "                inscatter = max(inscatter - attenuation.rgbr * texture4D(inscatterSampler, r0, mu0, muS0, nu), 0.0);\n"
        //"#ifdef FIX\n"
        /*                // avoids imprecision problems near horizon by interpolating between two points above and below horizon
        "                const float EPS = 0.004;\n"
        "                float muHoriz = -sqrt(1.0 - (cmn[1] / r) * (cmn[1] / r));\n"
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
        */
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

        /*        
        "vec3 groundColor(vec3 x, float t, vec3 v, vec3 s, float r, float mu, vec3 attenuation)\n"
        "{\n"
        "    vec3 result;\n"
        "    if (t > 0.0) {\n" // if ray hits ground surface
        "        // ground reflectance at end of ray, x0\n"
        "        vec3 x0 = x + t * v;\n"
        "        float r0 = length(x0);\n"
        "        vec3 n = x0 / r0;\n"
        //"        vec2 coords = vec2(atan(n.y, n.x), acos(n.z)) * vec2(0.5, 1.0) / PI + vec2(0.5, 0.0);\n"
        //"        vec4 reflectance = texture2D(reflectanceSampler, coords) * vec4(0.2, 0.2, 0.2, 1.0);\n"
        "        vec4 reflectance = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "        if (r0 > cmn[1] + 0.01) {\n"
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
        "        vec3 groundColor = reflectance.rgb * (max(muS, 0.0) * sunLight + groundSkyLight) * ISun / PI;\n"
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
        */

        // direct sun light for ray x+tv, when sun in direction s (=L0)
        "vec3 sunColor(vec3 x, float t, vec3 v, vec3 s, float r, float mu) {\n"
        "    if (t > 0.0) {\n"
        "        return vec3(0.0);\n"
        "    } else {\n"
        "        vec3 transmittance = r <= cmn[2] ? transmittanceWithShadow(r, mu) : vec3(1.0);\n" // T(x, xo)
        "        float isun = step(cos(sunScale), dot(v, s)) * ISun;\n" // Lsun
        "        return transmittance * isun;\n" // Eq (9)
        "    }\n"
        "}\n"
        "\n"

        "void main() {\n"
        "    vec3 x = vec3(0.0, 0.0, cmn[1] + cmn[0]);\n"
        "    vec3 ray = normalize(m_ray.xyz);\n"
        "\n"
        "    float r = length(x);\n"
        "    float mu = dot(x, ray) / r;\n"
        "    float t = -r * mu - sqrt(r * r * (mu * mu - 1.0) + cmn[1] * cmn[1]);\n"
        //"\n"
        //"    vec3 g = x - vec3(0.0, 0.0, cmn[1] + 10.0);\n"
        //"    float a = ray.x * ray.x + ray.y * ray.y - ray.z * ray.z;\n"
        //"    float b = 2.0 * (g.x * ray.x + g.y * ray.y - g.z * ray.z);\n"
        //"    float c = g.x * g.x + g.y * g.y - g.z * g.z;\n"
        //"    float d = -(b + sqrt(b * b - 4.0 * a * c)) / (2.0 * a);\n"
        //"    bool cone = d > 0.0 && abs(x.z + d * ray.z - cmn[1]) <= 10.0;\n"
        //"\n"
        //"    if (t > 0.0) {\n"
        //"        if (cone && d < t) {\n"
        //"            t = d;\n"
        //"        }\n"
        //"    } else if (cone) {\n"
        //"        t = d;\n"
        //"    }\n"
        //"\n"
        "    vec3 attenuation;\n"
        "    vec3 inscatterColor = inscatter(x, t, ray, sunr, r, mu, attenuation);\n" // S[L]  - T(x, xs) S[l] | xs"
        //"    vec3 groundColor = groundColor(x, t, ray, sun, r, mu, attenuation);\n"  // R[L0] + R[L*]
        "    vec3 sunColor = sunColor(x, t, ray, sunr, r, mu);\n" // L0
        "\n"

            // l'heure bleue (blaue stunde des ozons)

            // gauss between -12° and +0° sun altitude (Civil & Nautical twilight) 
            // http://en.wikipedia.org/wiki/Twilight
        "    float hb = t > 0.0 ? 0.0 : exp(-pow(sunr.z, 2.0) * 166) + 0.03;\n"     // the +0.03 is for a slight blueish tint at night
        "    vec3 bluehour = lheurebleue.w * lheurebleue.rgb * (dot(ray, sunr) + 1.5) * hb;\n" // * mu (optional..)
        "\n"
        "    gl_FragColor = vec4(HDR(bluehour + sunColor /*+ groundColor*/ + inscatterColor), 1.0)\n"    // Eq (16)
        "        + dither(3, int(cmn[3]));\n"
        "}";
}




#ifdef OSGHIMMEL_EXPOSE_SHADERS

osg::Shader *AtmosphereGeode::getVertexShader()
{
    return m_vShader;
}
osg::Shader *AtmosphereGeode::getGeometryShader()
{
    return NULL;
}
osg::Shader *AtmosphereGeode::getFragmentShader()
{
    return m_fShader;
}

#endif // OSGHIMMEL_EXPOSE_SHADERS

} // namespace osgHimmel