
// Copyright (c) 2011-2012, Daniel M�ller <dm@g4t3.de>
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

#include "moongeode.h"

#include "proceduralhimmel.h"
#include "shadermodifier.h"
#include "himmelquad.h"
#include "abstractastronomy.h"

#include <osg/Geometry>
#include <osg/BlendFunc>
#include <osg/Depth>

#include <osg/TextureCubeMap>
#include <osg/Texture2D>

#include <osgDB/ReadFile>


// Transforms a quad to the moons position into the canopy. Then 
// generates a circle with sphere normals (and normals from cube map)
// representing a perfect sphere in space.
// Applies lighting from sun - moon phase is always correct and no 
// separate calculation is required. Correct Moon rotation is currently
// faked (face towards earth is incorrect due to missing librations etc).

MoonGeode::MoonGeode(const ProceduralHimmel &himmel)
:   osg::Geode()
,   m_himmel(himmel)

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   m_hquad(new HimmelQuad())

,   u_moon(NULL)
,   u_sun(NULL)
,   u_moonCube(NULL)
,   u_rotation(NULL)
,   u_sunShine(NULL)    // [0,1,2] = color; [3] = intensity
,   u_earthShine(NULL)  // [0,1,2] = color; [3] = intensity
{
    setName("Moon");

    m_scale = defaultScale();

    osg::StateSet* stateSet = getOrCreateStateSet();

    setupNode(stateSet);
    setupUniforms(stateSet);
    setupShader(stateSet);
    setupTextures(stateSet);


    addDrawable(m_hquad);
};


MoonGeode::~MoonGeode()
{
};


void MoonGeode::update()
{
    osg::Vec3 moonv = m_himmel.astro()->getMoonPosition();
    u_moon->set(osg::Vec4(moonv, m_himmel.astro()->getAngularMoonRadius() * m_scale));

    u_sun->set(m_himmel.astro()->getSunPosition());

    // HACK: just orient the moon towards the earth. 
    // TODO: apply optical libration and real phase of the moon.
    u_rotation->set(osg::Matrix::rotate(osg::Vec3(0.0, 1.0, 0.0), moonv));
}


void MoonGeode::setupUniforms(osg::StateSet* stateSet)
{
    u_sun = new osg::Uniform("sun", osg::Vec3(1.0, 0.0, 0.0)); 
    stateSet->addUniform(u_sun);

    u_moon = new osg::Uniform("moon", osg::Vec4(0.0, 0.0, 1.0, 1.0)); // [3] = apparent angular radius (not diameter!)
    stateSet->addUniform(u_moon);

    u_moonCube = new osg::Uniform("moonCube", 0);
    stateSet->addUniform(u_moonCube);

    u_rotation = new osg::Uniform("rotation", osg::Matrixd());
    stateSet->addUniform(u_rotation);

    u_sunShine = new osg::Uniform("sunShine"
        , osg::Vec4(defaultSunShineColor(), defaultSunShineIntensity()));
    stateSet->addUniform(u_sunShine);

    u_earthShine = new osg::Uniform("earthShine"
        , osg::Vec4(defaultEarthShineColor(), defaultEarthShineIntensity()));
    stateSet->addUniform(u_earthShine);
}


void MoonGeode::setupNode(osg::StateSet* stateSet)
{
    // This prevents objects rendered afterwards to appear inside the moon.

    osg::Depth* depth = new osg::Depth(osg::Depth::LESS);    
    stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);
    
    osg::BlendFunc *blend  = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE);
    stateSet->setAttributeAndModes(blend, osg::StateAttribute::ON);
}


void MoonGeode::setupShader(osg::StateSet* stateSet)
{
    m_vShader->setShaderSource(getVertexShaderSource());
    m_fShader->setShaderSource(getFragmentShaderSource());

    m_program->addShader(m_vShader);
    m_program->addShader(m_fShader);

    stateSet->setAttributeAndModes(m_program, osg::StateAttribute::ON);

#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
    if(m_himmel.shaderModifier())
    {
        m_himmel.shaderModifier()->registerShader(getName(), m_fShader);
        m_himmel.shaderModifier()->registerShader(getName(), m_vShader);
    }
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER
}


void MoonGeode::setupTextures(osg::StateSet* stateSet)
{
    osg::ref_ptr<osg::TextureCubeMap> tcm(new osg::TextureCubeMap);

    tcm->setUnRefImageDataAfterApply(true);

    tcm->setInternalFormat(GL_RGBA);

    tcm->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tcm->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    tcm->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

    tcm->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    tcm->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

    tcm->setImage(osg::TextureCubeMap::POSITIVE_X, osgDB::readImageFile("resources/moon_px.png"));
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_X, osgDB::readImageFile("resources/moon_nx.png"));
    tcm->setImage(osg::TextureCubeMap::POSITIVE_Y, osgDB::readImageFile("resources/moon_py.png"));
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_Y, osgDB::readImageFile("resources/moon_ny.png"));
    tcm->setImage(osg::TextureCubeMap::POSITIVE_Z, osgDB::readImageFile("resources/moon_pz.png"));
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_Z, osgDB::readImageFile("resources/moon_nz.png"));

    stateSet->setTextureAttributeAndModes(0, tcm, osg::StateAttribute::ON);

    u_moonCube->set(0);
}



const float MoonGeode::setScale(const float scale)
{
    m_scale = scale;
    return getScale();
}

const float MoonGeode::getScale() const
{
    return m_scale;
}

const float MoonGeode::defaultScale()
{
    return 4.f;
}


const osg::Vec3 MoonGeode::setSunShineColor(const osg::Vec3 &color)
{
    osg::Vec4 sunShine;
    u_sunShine->get(sunShine);

    sunShine[0] = color[0];
    sunShine[1] = color[1];
    sunShine[2] = color[2];

    u_sunShine->set(sunShine);

    return getSunShineColor();
}

const osg::Vec3 MoonGeode::getSunShineColor() const
{
    osg::Vec4 sunShine;
    u_sunShine->get(sunShine);

    return osg::Vec3(sunShine[0], sunShine[1], sunShine[2]);
}

const osg::Vec3 MoonGeode::defaultSunShineColor()
{
    return osg::Vec3(1.0, 0.96, 0.80);
}


const float MoonGeode::setSunShineIntensity(const float intensity)
{
    osg::Vec4 sunShine;
    u_sunShine->get(sunShine);

    sunShine[3] = intensity;
    u_sunShine->set(sunShine);

    return getSunShineIntensity();
}

const float MoonGeode::getSunShineIntensity() const
{
    osg::Vec4 sunShine;
    u_sunShine->get(sunShine);

    return sunShine[3];
}

const float MoonGeode::defaultSunShineIntensity()
{
    return 32.f;
}


const osg::Vec3 MoonGeode::setEarthShineColor(const osg::Vec3 &color)
{
    osg::Vec4 earthShine;
    u_earthShine->get(earthShine);

    earthShine[0] = color[0];
    earthShine[1] = color[1];
    earthShine[2] = color[2];

    u_earthShine->set(earthShine);

    return getEarthShineColor();
}

const osg::Vec3 MoonGeode::getEarthShineColor() const
{
    osg::Vec4 earthShine;
    u_earthShine->get(earthShine);

    return osg::Vec3(earthShine[0], earthShine[1], earthShine[2]);
}

const osg::Vec3 MoonGeode::defaultEarthShineColor()
{
    return osg::Vec3(1.0, 1.02, 1.04);
}


const float MoonGeode::setEarthShineIntensity(const float intensity)
{
    osg::Vec4 earthShine;
    u_earthShine->get(earthShine);

    earthShine[3] = intensity;
    u_earthShine->set(earthShine);

    return getEarthShineIntensity();
}

const float MoonGeode::getEarthShineIntensity() const
{
    osg::Vec4 earthShine;
    u_earthShine->get(earthShine);

    return earthShine[3];
}

const float MoonGeode::defaultEarthShineIntensity()
{
    return 1.f;
}




// VertexShader

#include "shaderfragment/version.vsf"

const std::string MoonGeode::getVertexShaderSource()
{
    return glsl_v_version_150

    +   
        "uniform vec4 moon; // moon.xyz is expected to be normalized and\n"
        "                   // moon.a the moons angular radius in rad.\n"

        "out mat4 m_tangent;\n"
        "out vec3 m_eye;\n"
        "\n"
        "const float SQRT2 = 1.41421356237;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    vec3 m = moon.xyz;\n"
        "\n"

        //  tangent space of the unitsphere at m.

        "    vec3 u = normalize(cross(m, vec3(1)));\n"
        "    vec3 v = normalize(cross(u, m));\n"
        "\n"
        "    m_tangent = mat4(vec4(u, 0.0), vec4(v, 0.0), vec4(m, 0.0), vec4(vec3(0.0), 1.0));\n"
        "\n"

        "    float mScale = tan(moon.a) * SQRT2;\n"
        "\n"
        "    m_eye = m - normalize(gl_Vertex.x * u + gl_Vertex.y * v) * mScale;\n"
        "\n"
        "    gl_TexCoord[0] = gl_Vertex;\n"
        "    gl_Position = gl_ModelViewProjectionMatrix * vec4(m_eye, 1.0);\n"
        "}\n\n";
}


// FragmentShader

#include "shaderfragment/version.fsf"

const std::string MoonGeode::getFragmentShaderSource()
{
    return glsl_f_version_150
    
    +   
        "uniform vec4 moon; // moon.xyz is expected to be normalized and\n"
        "                   // moon.a the moons angular diameter in rad.\n"
        "uniform vec3 sun;  // Expected to be normalized.\n"
        "\n"
        "uniform samplerCube moonCube;\n"
        "\n"
        "uniform mat4 rotation;\n"
        "\n"
        "uniform vec4 sunShine;   // rgb as color and w as intensity.\n"
        "uniform vec4 earthShine; // rgb as color and w as intensity.\n"
        "\n"

        "const float radius = 0.98;\n"
        "\n"
        "in vec3 m_eye;\n"
        "in mat4 m_tangent;\n"
        "\n"

        "const float PI               = 3.1415926535897932;\n"
        "const float TWO_OVER_THREEPI = 0.2122065907891938;\n"
        "\n"

        "void main(void)\n"
        "{\n"
        "    float x = gl_TexCoord[0].x;\n"
        "    float y = gl_TexCoord[0].y;\n"
        "\n"
        "    float zz = radius * radius - x * x - y * y;\n"
        "\n"
        "    if(zz < 1.0 - radius)\n"
        "        discard;\n"
        "\n"
        "    float z = sqrt(zz);\n"
        "\n"

        // Moon Tanget Space
        "    vec3 mn = (m_tangent * vec4(x, y, z, 1.0)).xyz;\n"
        "    vec3 mt = mn.zyx;\n"
        "    vec3 mb = mn.xzy;\n"
        "\n"

        // Texture Lookup direction -> "FrontFacing".
        "    vec3 q = (vec4(mn.x, mn.y, mn.z, 1.0) * rotation).xyz;\n"
        "\n"
        "    vec4 c  = textureCube(moonCube, vec3(-q.x, q.y, -q.z));\n"
        "    vec3 cn = (c.xyz) * 2.0 - 1.0;\n"
        "    vec3 n = vec3(dot(cn, mt), dot(cn, mb), dot(cn, mn));\n"
        "\n"

        "    vec3 m = moon.xyz;\n"
        "    vec3 s = sun;\n"
        "    vec3 e = normalize(m_eye.xyz);\n"


        // Hapke-Lommel-Seeliger approximation of the moons reflectance function.

        "    float cos_p = clamp(dot(e, s), 0.0, 1.0);\n"
        "    float p     = acos(cos_p);\n"
        "    float tan_p = tan(p);\n"
        "\n"
        "    float dot_ne = dot(n, e);\n"
        "    float dot_nl = dot(n, s);\n"
        "\n"
        "    float g = 0.6; // surface densitiy parameter which determines the sharpness of the peak at the full Moon\n"
        "    float t = 0.1; // small amount of forward scattering\n"
        "\n"

        // Retrodirective.
        "    float R = 2.0 - tan_p / (2.0 * g) \n"
        "        * (1.0 - exp(-g / tan_p))     \n"
        "        * (3.0 - exp(-g / tan_p));    \n"
        "\n"

        // Scattering.
        "    float S = (sin(p) + (PI - p) * cos_p) / PI \n"
        "        + t * (1.0 - cos_p) * (1.0 - cos_p);\n"
        "\n"

        // BRDF
        "    float F = TWO_OVER_THREEPI * R * S * 1.0 / (1.0 + (-dot_ne) / dot_nl);\n"
        "\n"

        "    if(dot_nl > 0.0)\n"
        "        F = 0.0;\n"
        "\n"
        

        // ("Multiple Light Scattering" - 1980 - Van de Hulst) and 
        // ("A Physically-Based Night Sky Model" - 2001 - Wann Jensen et. al.) -> the 0.19 is the earth full intensity
        "    float op2 = (PI - acos(dot(-m, s))) * 0.5; // opposite phase over 2\n"
        "    float Eem = 0.19 * 0.5 * (1.0 - sin(op2) * tan(op2) * log(1.0 / tan(op2 * 0.5)));\n"
        "\n"

        // My approximation with non-perceivable difference.
        //"    float op2 = dot(-m, s);
        //"    float Eem = 0.1 * op2 * op2;
        //"\n"

        "    gl_FragDepth = 0.99999996;\n"
        "    gl_FragColor = vec4(c.a * (earthShine.w * earthShine.rgb * Eem + sunShine.w * sunShine.rgb * F), 1.0);\n"
        "}\n\n";


        // Debug.

        //    gl_FragColor = vec4(x * 0.5 + 0.5, y * 0.5 + 0.5, 0.0, 0.0);
        //    gl_FragColor = vec4(mn * 0.5 + 0.5, 1.0);
        //    gl_FragColor = vec4(n * 0.5 + 0.5, 1.0);
        //    gl_FragColor = vec4(vec3(cd), 1.0);
        //    gl_FragColor = vec4(d * vec3(1.06, 1.06, 0.98), 1.0);
}