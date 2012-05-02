
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

#include "starsgeode.h"

#include "himmel.h"
#include "abstractastronomy.h"
#include "randommapgenerator.h"
#include "mathmacros.h"
#include "coords.h"
#include "earth.h"
#include "stars.h"
#include "strutils.h"

#include "shaderfragment/common.h"

#include <osg/Geometry>
#include <osg/Point>
#include <osg/BlendFunc>
#include <osg/Image>
#include <osg/Texture1D>
#include <osg/Depth>


namespace
{
    const float TWO_TIMES_SQRT2(2.0 * sqrt(2.0));
}


namespace osgHimmel
{

StarsGeode::StarsGeode(const char* brightStarsFilePath)
:   osg::Geode()

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_gShader(new osg::Shader(osg::Shader::GEOMETRY))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   u_R(NULL)
,   u_quadWidth(NULL)
,   u_noise1(NULL)

,   u_color(NULL)
,   u_glareIntensity(NULL)
,   u_glareScale(NULL)
,   u_apparentMagnitude(NULL)
,   u_scattering(NULL)
,   u_scintillation(NULL)
{
    setName("Stars");

    osg::StateSet* stateSet = getOrCreateStateSet();

    setupNode(stateSet, brightStarsFilePath);
    setupUniforms(stateSet);
    setupShader(stateSet);
    setupTextures(stateSet);
};


StarsGeode::~StarsGeode()
{
};


void StarsGeode::update(const Himmel &himmel)
{
    const float fov = himmel.getCameraFovHint();
    const float height = himmel.getViewSizeHeightHint();

    u_quadWidth->set(static_cast<float>(tan(_rad(fov / 2)) / (height * 0.5)));

    u_R->set(himmel.astro()->equToLocalHorizonMatrix());
}


void StarsGeode::setupUniforms(osg::StateSet* stateSet)
{
    u_R = new osg::Uniform("R", osg::Matrix::identity());
    stateSet->addUniform(u_R);

    u_quadWidth = new osg::Uniform("quadWidth", 0.0f);
    stateSet->addUniform(u_quadWidth);

    u_noise1 = new osg::Uniform("noise1", 0);
    stateSet->addUniform(u_noise1);


    u_color = new osg::Uniform("color", osg::Vec4(defaultColor(), defaultColorRatio()));
    stateSet->addUniform(u_color);


    u_glareIntensity = new osg::Uniform("glareIntensity", 1.0f);
    stateSet->addUniform(u_glareIntensity);

    u_glareScale = new osg::Uniform("glareScale", defaultGlareScale());
    stateSet->addUniform(u_glareScale);

    u_apparentMagnitude = new osg::Uniform("apparentMagnitude", defaultApparentMagnitude());
    stateSet->addUniform(u_apparentMagnitude);

    u_scintillation = new osg::Uniform("scintillation", defaultScintillation());
    stateSet->addUniform(u_scintillation);

    u_scattering = new osg::Uniform("scattering", defaultScattering());
    stateSet->addUniform(u_scattering);
}


void StarsGeode::createAndAddDrawable(const char* brightStarsFilePath)
{
    BrightStars bs(brightStarsFilePath);
    const BrightStars::s_BrightStar *stars = bs.stars();

    osg::ref_ptr<osg::Vec4Array> cAry = new osg::Vec4Array(bs.numStars());
    osg::ref_ptr<osg::Vec4Array> vAry = new osg::Vec4Array(bs.numStars());

    for(unsigned int i = 0; i < bs.numStars(); ++i)
    {
        t_equf equ;
        equ.right_ascension = _rightascd(stars[i].RA, 0, 0);
        equ.declination = stars[i].DE;

        osg::Vec3f vec = equ.toEuclidean();
        (*vAry)[i] = osg::Vec4(vec.x(), vec.y(), vec.z(), i);

        (*cAry)[i] = osg::Vec4(stars[i].sRGB_R, stars[i].sRGB_G, stars[i].sRGB_B, stars[i].Vmag);
    }
      
    osg::ref_ptr<osg::Geometry> g = new osg::Geometry;
    addDrawable(g);

    g->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    g->setColorArray(cAry);
    g->setVertexArray(vAry);

    g->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vAry->size()));

    // If things go wrong, fall back to big point rendering without geometry shader.
    g->getOrCreateStateSet()->setAttribute(new osg::Point(TWO_TIMES_SQRT2));
}


void StarsGeode::setupNode(
    osg::StateSet* stateSet
,   const char *brightStarsFilePath)
{
    createAndAddDrawable(brightStarsFilePath);

    osg::Depth* depth = new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0);    
    stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

    osg::BlendFunc *blend  = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE);
    stateSet->setAttributeAndModes(blend, osg::StateAttribute::ON);
}


void StarsGeode::setupShader(osg::StateSet* stateSet)
{
    m_vShader->setShaderSource(getVertexShaderSource());
    m_gShader->setShaderSource(getGeometryShaderSource());
    m_fShader->setShaderSource(getFragmentShaderSource());

    m_program->addShader(m_vShader);
    m_program->addShader(m_gShader);
    m_program->addShader(m_fShader);

    stateSet->setAttributeAndModes(m_program, osg::StateAttribute::ON);
}


void StarsGeode::setupTextures(osg::StateSet* stateSet)
{   
    const int noiseN = 256;

    unsigned char *noiseMap = new unsigned char[noiseN];
    RandomMapGenerator::generate1(noiseN, 1, noiseMap);

    osg::ref_ptr<osg::Image> noiseImage = new osg::Image();
    noiseImage->setImage(noiseN, 1, 1
        , GL_LUMINANCE8, GL_LUMINANCE, GL_UNSIGNED_BYTE
        , noiseMap, osg::Image::USE_NEW_DELETE);

    osg::ref_ptr<osg::Texture1D> noise = new osg::Texture1D(noiseImage);

    int unit;
    u_noise1->get(unit);

    stateSet->setTextureAttributeAndModes(unit, noise, osg::StateAttribute::ON);
}


const float StarsGeode::setGlareIntensity(const float intensity)
{
    u_glareIntensity->set(intensity);
    return getGlareIntensity();
}

const float StarsGeode::getGlareIntensity() const
{
    float intensity;
    u_glareIntensity->get(intensity);

    return intensity;
}


const float StarsGeode::setGlareScale(const float scale)
{
    u_glareScale->set(scale);
    return getGlareScale();
}

const float StarsGeode::getGlareScale() const
{
    float scale;
    u_glareScale->get(scale);

    return scale;
}

const float StarsGeode::defaultGlareScale()
{
    return 64.0f;
}


const float StarsGeode::setScintillation(const float scintillation)
{
    u_scintillation->set(scintillation);
    return getScintillation();
}

const float StarsGeode::getScintillation() const
{
    float scintillation;
    u_scintillation->get(scintillation);

    return scintillation;
}

const float StarsGeode::defaultScintillation()
{
    return 1.0f;
}


const float StarsGeode::setScattering(const float scattering)
{
    u_scattering->set(scattering);
    return getScattering();
}

const float StarsGeode::getScattering() const
{
    float scattering;
    u_scattering->get(scattering);

    return scattering;
}

const float StarsGeode::defaultScattering()
{
    return 2.0f;
}


const float StarsGeode::setApparentMagnitude(const float vMag)
{
    u_apparentMagnitude->set(vMag);
    return getApparentMagnitude();
}

const float StarsGeode::getApparentMagnitude() const
{
    float vMag;
    u_apparentMagnitude->get(vMag);

    return vMag;
}

const float StarsGeode::defaultApparentMagnitude() 
{
    return 5.0f;
}


const osg::Vec3 StarsGeode::setColor(const osg::Vec3 color)
{
    osg::Vec4 colorAndRatio;
    u_color->get(colorAndRatio);
    
    colorAndRatio[0] = color[0];
    colorAndRatio[1] = color[1];
    colorAndRatio[2] = color[2];

    u_color->set(colorAndRatio);
    return getColor();
}

const osg::Vec3 StarsGeode::getColor() const
{
    osg::Vec4 colorAndRatio;
    u_color->get(colorAndRatio);

    return osg::Vec3(colorAndRatio[0], colorAndRatio[1], colorAndRatio[2]);
}

const osg::Vec3 StarsGeode::defaultColor()
{
    return osg::Vec3(0.66, 0.78, 1.00);
}


const float StarsGeode::setColorRatio(const float ratio)
{
    osg::Vec4 colorAndRatio;
    u_color->get(colorAndRatio);
    
    colorAndRatio[3] = ratio;

    u_color->set(colorAndRatio);
    return getColorRatio();
}

const float StarsGeode::getColorRatio() const
{
    osg::Vec4 colorAndRatio;
    u_color->get(colorAndRatio);

    return colorAndRatio[3];
}

const float StarsGeode::defaultColorRatio()
{
    return 0.5f;
}




const std::string StarsGeode::getVertexShaderSource()
{
    char apparentMagLimit[8];

    sprintf_s(apparentMagLimit, 8, "%.2f", static_cast<float>(Earth::apparentMagnitudeLimit()));

    return glsl_version_150()

    +   glsl_horizon()
    
    +   PRAGMA_ONCE(main,

        "uniform vec3 sun;\n"
        "\n"
        "uniform mat4 R;\n" // rgb and alpha for mix
        "uniform vec4 color;\n" // rgb and alpha for mix
        "\n"
        "uniform float scintillation;\n"
        "uniform float scattering;\n"
        "\n"
        "uniform float quadWidth;\n"
        "uniform float apparentMagnitude;\n"
        "\n"
        "uniform sampler1D noise1;\n"
        "\n"

        "out vec4 m_color;\n"
        "\n"

        "const float minB = pow(2.512, -" + std::string(apparentMagLimit) + ") * 4e-5;\n"
        "\n"
        // ("Efcient Rendering of Atmospheric Phenomena" - 2004 - Riley et al.)
        // This is used only for the ratio, not for exact physical scale.
        "const vec3 lambda = normalize(vec3(0.58, 1.35, 3.31));\n"
        "\n"

        "void main(void)\n"
        "{\n"
        "    vec4 v = gl_Vertex * R;\n"
        "    gl_Position = v;\n"
        "\n"
        "    if(belowHorizon(v.xyz))\n" // "discard" stars below horizon
        "    {\n"
        "        m_color = vec4(0.0);\n"
        "        return;\n"
        "    }\n"
        "\n"
        "    float vMag = gl_Color.w;\n"
        "\n"
        "    float estB = pow(2.512, apparentMagnitude - vMag);\n"
        "    float scaledB = estB * minB / (quadWidth * quadWidth);\n"
        "\n"
        "    float i = mod(int(cmn[3]) ^ int(gl_Vertex.w), 251);\n"
        "    float s = pow(texture(noise1, i / 256.0).r , 8) / (scaledB * 0.05);\n"
        "\n"
        // Approximation of relative air mass (path length relative to that at the zenith at sea level).
        // Also called optical path length: http://en.wikipedia.org/wiki/Air_mass_(astronomy).
        // y = x^5.37 -> y is 39 times higher for x = 0 than for x = 1 which correlates the relative 
        // air mass ratio from zenith to horizon.
        "    float w1 = pow(1.0 - v.z, 5.37) * scattering;\n"
        "    float w2 = min(1.0, (1.0 - v.z) * s * scintillation);\n"
        "\n"
        "    vec3 c = mix(gl_Color.rgb, color.rgb, color.a)\n"
        "        - lambda * (w1 * 0.25 + w2);\n"
        "\n"
            // Day-Twilight-Night-Intensity Mapping (Butterworth-Filter)
        "    float b = 1.0 / sqrt(1 + pow(sun.z + 1.3, 16));\n"
        "\n"
        "    m_color = vec4(c, scaledB - w1) * b;\n"
        "}");
}


const std::string StarsGeode::getGeometryShaderSource()
{
    return glsl_version_150() +

        glsl_geometry_ext()
    
    +   PRAGMA_ONCE(main, 

        "layout (points) in;\n"
        "layout (triangle_Strip, max_vertices = 4) out;\n"
        "\n"
        "uniform float quadWidth;\n"
        "uniform float glareScale;\n"
        "\n"
        "in vec4 m_color[];\n"
        "out vec4 m_c;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec3 p = normalize(gl_in[0].gl_Position.xyz);\n"
        "\n"
        "    vec3 u = cross(p, vec3(1));\n"
        "    vec3 v = cross(u, p);\n"
        "\n"
        "    float scaledB = m_color[0].w;\n"
        "\n"
        // Ignore stars with less than 1% Brightness.
        "    if(scaledB < 0.01)\n"
        "        return;\n"
        "\n"
        "    m_c = vec4(m_color[0].rgb, scaledB);\n"
        "\n"
        "    gl_TexCoord[0].z = sqrt(scaledB) * max(1.0, glareScale);\n"
        "    float k = sqrt(2) * quadWidth * gl_TexCoord[0].z;\n"
        "\n"
        "    gl_Position = gl_ModelViewProjectionMatrix * vec4(p - normalize(-u -v) * k, 1.0);\n"
        "    gl_TexCoord[0].xy = vec2(-1.0, -1.0);\n"
        "    EmitVertex();\n"
        "\n"
        "    gl_Position = gl_ModelViewProjectionMatrix * vec4(p - normalize(-u +v) * k, 1.0);\n"
        "    gl_TexCoord[0].xy = vec2(-1.0,  1.0);\n"
        "    EmitVertex();\n"
        "\n"
        "    gl_Position = gl_ModelViewProjectionMatrix  * vec4(p - normalize(+u -v) * k, 1.0);\n"
        "    gl_TexCoord[0].xy = vec2( 1.0, -1.0);\n"
        "    EmitVertex();\n"
        "\n"
        "    gl_Position = gl_ModelViewProjectionMatrix * vec4(p - normalize(+u +v) * k, 1.0);\n"
        "    gl_TexCoord[0].xy = vec2( 1.0,  1.0);\n"
        "    EmitVertex();\n"
        "}");
}


const std::string StarsGeode::getFragmentShaderSource()
{
    return glsl_version_150()
        
    +   PRAGMA_ONCE(main,

        "uniform float quadWidth;\n"
        "uniform float glareIntensity;\n"
        "\n"
        "uniform vec3 sun;\n"
        "\n"
        "in vec4 m_c;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    float x = gl_TexCoord[0].x;\n"
        "    float y = gl_TexCoord[0].y;\n"
        "\n"
        "    float radius = 0.98;\n"
        "    float zz = (radius * radius - x * x - y * y);\n"
        "\n"
        "    if(zz < 0)\n"
        "        discard;\n"
        "\n"
        "    float s =  gl_TexCoord[0].z;\n"
        "\n"
        "    float l = length(vec2(x, y));\n"
        "\n"
        "    float t = smoothstep(1.0, 0.0, l * s * 0.5);\n"
        "    float g = smoothstep(1.0, 0.0, pow(l, 0.0675)) * glareIntensity;\n"
        "\n"
        "    gl_FragColor = m_c * (t + g);\n"
        "}");
}




#ifdef OSGHIMMEL_EXPOSE_SHADERS

osg::Shader *StarsGeode::getVertexShader()
{
    return m_vShader;
}
osg::Shader *StarsGeode::getGeometryShader()
{
    return m_gShader;
}
osg::Shader *StarsGeode::getFragmentShader()
{
    return m_fShader;
}

#endif // OSGHIMMEL_EXPOSE_SHADERS

} // namespace osgHimmel