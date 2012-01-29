
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

#include "proceduralhimmel.h"
#include "shadermodifier.h"
#include "abstractastronomy.h"
#include "randommapgenerator.h"

#include "mathmacros.h"

#include "coords.h"
#include "earth.h"
#include "sideraltime.h"
#include "stars.h"

#include <osg/Geometry>
#include <osg/Point>
#include <osg/BlendFunc>
#include <osg/Image>
#include <osg/Texture1D>


namespace
{
    static const float TWO_TIMES_SQRT2(2.0 * sqrt(2.0));
}


StarsGeode::StarsGeode(const ProceduralHimmel &himmel)
:   osg::Geode()
,   m_himmel(himmel)

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_gShader(new osg::Shader(osg::Shader::GEOMETRY))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   u_quadWidth(NULL)
,   u_noise1(NULL)

,   u_color(NULL)
,   u_glareIntensity(NULL)
,   u_glareScale(NULL)
,   u_maxVMag(NULL)
,   u_scattering(NULL)
,   u_scintillation(NULL)
{
    setName("Stars");

    osg::StateSet* stateSet = getOrCreateStateSet();

    setupNode(stateSet);
    setupUniforms(stateSet);
    setupShader(stateSet);
    setupTextures(stateSet);
};


StarsGeode::~StarsGeode()
{
};


void StarsGeode::update()
{
    float fov = m_himmel.getCameraFovHint();
    float height = m_himmel.getViewSizeHeightHint();
    
    u_quadWidth->set(static_cast<float>(tan(_rad(fov) / height) * TWO_TIMES_SQRT2));

    t_aTime aTime(m_himmel.astro()->getATime());

    // It seems faster to update the array every frame than
    // to do the equ to hor conversion in shader.
    // Furthermore osg produces clipping errors with the fixed vertices set.

    const float o = earth_trueObliquity(jd(aTime));
    const float s = siderealTime(aTime);

    const float la = m_himmel.astro()->getLatitude();
    const float lo = m_himmel.astro()->getLongitude();

    osg::Vec4Array::iterator i = m_vAry->begin();
    const osg::Vec4Array::iterator iEnd = m_vAry->end();

    unsigned int c = 0;

    for(; i != iEnd; ++i, ++c)
    {
        t_equf equ;
        equ.right_ascension = _rightascd(m_bss[c].RA, 0, 0);
        equ.declination = m_bss[c].DE;

        osg::Vec3f vec = equ.toHorizontal(s, la, lo).toEuclidean();
        i->set(vec.x(), vec.y(), vec.z(), c);
    }
    m_g->setVertexArray(m_vAry);

    // TEMP

    u_sun->set(m_himmel.astro()->getSunPosition());
}


void StarsGeode::setupUniforms(osg::StateSet* stateSet)
{
    u_quadWidth = new osg::Uniform("quadWidth", 0.0f);
    stateSet->addUniform(u_quadWidth);

    u_noise1 = new osg::Uniform("noise1", 0);
    stateSet->addUniform(u_noise1);


    u_color = new osg::Uniform("color", osg::Vec4(1.0, 1.0, 1.0, 0.0));
    stateSet->addUniform(u_color);


    u_glareIntensity = new osg::Uniform("glareIntensity", 1.0f);
    stateSet->addUniform(u_glareIntensity);

    u_glareScale = new osg::Uniform("glareScale", 1.0f);
    stateSet->addUniform(u_glareScale);

    u_maxVMag = new osg::Uniform("maxVMag", defaultMaxVMag());
    stateSet->addUniform(u_maxVMag);

    u_scintillation = new osg::Uniform("scintillation", 1.0f);
    stateSet->addUniform(u_scintillation);

    u_scattering = new osg::Uniform("scattering", 1.0f);
    stateSet->addUniform(u_scattering);


    // TEMP - use correct function in cpu for that
    u_sun = new osg::Uniform("sun", osg::Vec3(1.0, 0.0, 0.0));
    stateSet->addUniform(u_sun);
}


void StarsGeode::createAndAddDrawable()
{
    m_g = new osg::Geometry;
    addDrawable(m_g);

    brightstars_readFromFile("resources/brightstars", m_bss);


    m_cAry = new osg::Vec4Array;
    m_vAry = new osg::Vec4Array(m_bss.size());

    for(int i = 0; i < m_bss.size(); ++i)
        m_cAry->push_back(osg::Vec4(m_bss[i].sRGB_R, m_bss[i].sRGB_G, m_bss[i].sRGB_B, m_bss[i].Vmag));

    m_g->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    m_g->setColorArray(m_cAry);
    m_g->setVertexArray(m_vAry);

    m_g->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, m_vAry->size()));

    // If things go wrong, fall back to big point rendering without geometry shader.
    m_g->getOrCreateStateSet()->setAttribute(new osg::Point(TWO_TIMES_SQRT2));
}


void StarsGeode::setupNode(osg::StateSet* stateSet)
{
    createAndAddDrawable();

    osg::BlendFunc *blend  = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE);
    stateSet->setAttributeAndModes(blend, osg::StateAttribute::ON);

    stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
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

#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
    if(m_himmel.shaderModifier())
    {
        m_himmel.shaderModifier()->registerShader(getName(), m_fShader);
        m_himmel.shaderModifier()->registerShader(getName(), m_gShader);
        m_himmel.shaderModifier()->registerShader(getName(), m_vShader);
    }
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER
}


void StarsGeode::setupTextures(osg::StateSet* stateSet)
{   
    const int noiseN = 256;

    unsigned char *noiseMap = new unsigned char[noiseN * 3];
    RandomMapGenerator::generate3(noiseN, 1, noiseMap);

    osg::ref_ptr<osg::Image> noiseImage = new osg::Image();
    noiseImage->setImage(noiseN, 1, 1
        , GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE
        , noiseMap, osg::Image::USE_NEW_DELETE);

    osg::ref_ptr<osg::Texture1D> noise = new osg::Texture1D(noiseImage);

    stateSet->setTextureAttributeAndModes(0, noise, osg::StateAttribute::ON);

    u_noise1->set(0);
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


const float StarsGeode::setMaxVMag(const float vMag)
{
    u_maxVMag->set(vMag);
    return getMaxVMag();
}

const float StarsGeode::getMaxVMag() const
{
    float vMag;
    u_maxVMag->get(vMag);

    return vMag;
}

const float StarsGeode::defaultMaxVMag() 
{
    return 6.0f;
}


const osg::Vec3 StarsGeode::setColor(const osg::Vec3 color)
{
    osg::Vec4 colorAndRatio;
    u_color->get(colorAndRatio);
    
    colorAndRatio._v[0] = color._v[0];
    colorAndRatio._v[1] = color._v[1];
    colorAndRatio._v[2] = color._v[2];

    u_color->set(colorAndRatio);
    return getColor();
}

const osg::Vec3 StarsGeode::getColor() const
{
    osg::Vec4 colorAndRatio;
    u_color->get(colorAndRatio);

    return osg::Vec3(colorAndRatio._v[0], colorAndRatio._v[1], colorAndRatio._v[2]);
}


const float StarsGeode::setColorRatio(const float ratio)
{
    osg::Vec4 colorAndRatio;
    u_color->get(colorAndRatio);
    
    colorAndRatio._v[3] = ratio;

    u_color->set(colorAndRatio);
    return getColorRatio();
}

const float StarsGeode::getColorRatio() const
{
    osg::Vec4 colorAndRatio;
    u_color->get(colorAndRatio);

    return colorAndRatio._v[3];
}


// VertexShader

const std::string StarsGeode::getVertexShaderSource()
{
    return 

        "#version 150 compatibility\n"
        "\n"
        "uniform vec4 color;\n" // rgb and alpha for mix
        "\n"
        "uniform float scintillation;\n"
        "uniform float scattering;\n"
        "\n"
        "uniform float quadWidth;\n"
        "uniform float maxVMag;\n"
        "\n"
        "uniform sampler1D noise1;\n"
        "\n"
        "uniform int osg_FrameNumber;\n"
        "\n"
        "out vec4 m_color;\n"
        "\n"
        "const float minB = pow(2.512, -6.5) * 0.1;\n"
        
        //  ("Efcient Rendering of Atmospheric Phenomena" - 2004 - Riley et al.)
        "const vec3 lambda = vec3(0.058, 0.135, 0.331);\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    float vMag = gl_Color.w;\n"
        "\n"
        "    float estB = pow(2.512, maxVMag - vMag);\n"
        "    float scaledB = minB * estB / quadWidth;\n"
        //"\n"
        "    float i = mod(osg_FrameNumber ^ int(gl_Vertex.w), 251);\n"
        "    vec3 s = texture(noise1, i / 256.0).rgb - 0.5;\n"
        "\n"
        // Approximation of relative air mass (path length relative to that at the zenith at sea level).
        // Also called optical path length: http://en.wikipedia.org/wiki/Air_mass_(astronomy).
        // y = x^5.37 -> y is 39 times higher for x = 0 than for x = 1 which correlates the relative 
        // air mass ratio from zenith to horizon.
        "    float w1 = pow(1.0 - gl_Vertex.b, 5.37);\n"
        "\n"
        "    vec3 c = mix(gl_Color.rgb, color.rgb, color.a)\n"
        "        - lambda * w1 * 4 * (scattering + s * scintillation);\n"
        "\n"
        "    m_color = vec4(c, scaledB - w1 * s * 0.1);\n"
        "\n"
        "    gl_Position = vec4(gl_Vertex.xyz, 1.0);\n"
        "}\n\n";


/* equ to hor conversion for vertex shader:

         ...

        float sinDE = gl_Vertex[0];
        float cosDE = gl_Vertex[1];
        float tanDE = gl_Vertex[2];
        float    RA = gl_Vertex[3];

        float H = equ2hor[0] - RA;
        float sinH = sin(H);
        float cosH = cos(H);

        float sinLa = equ2hor[1];
        float cosLa = equ2hor[2];

        float h = atan(sinH, cosH * sinLa - cosLa * tanDE);
        float A = asin(sinLa * sinDE + cosH * cosLa * cosDE);

        float cosA = cos(A);

        float x = sin(h) * cosA;
        float y = cos(h) * cosA;
        float z = sin(A);

        gl_Position = vec4(x, y, z, 1.0);

        ...
*/
}


// GeometryShader

const std::string StarsGeode::getGeometryShaderSource()
{
    return
        "#version 150 compatibility\n"
        "\n"
        "#extension GL_EXT_geometry_shader4 : enable\n"
        "\n"
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
        "    m_c = vec4(m_color[0].rgb, scaledB);\n"
        "\n"
        "    gl_TexCoord[0].z = (1.0 + sqrt(scaledB)) * max(1.0, glareScale);\n"
        "\n"

        "    float k = quadWidth * gl_TexCoord[0].z;\n"
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
        "}\n\n";
}


// FragmentShader

const std::string StarsGeode::getFragmentShaderSource()
{
    return 

        "#version 150 compatibility\n"
        "\n"
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
        "    float t = smoothstep(1.0, 0.0, l * s);\n"
        "    float g = smoothstep(1.0, 0.0, pow(l, 0.125)) * glareIntensity;\n"
        "\n"
        "    gl_FragColor = m_c * (t + g) * clamp(-asin(sun.z - 0.1) * 2, 0.0, 1.0);\n"
        "}\n\n";
}
