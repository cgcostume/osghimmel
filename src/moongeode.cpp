
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

#include "moongeode.h"

#include "himmel.h"
#include "himmelquad.h"
#include "abstractastronomy.h"
#include "mathmacros.h"
#include "interpolate.h"

#include "shaderfragment/common.h"
#include "shaderfragment/scattering.h"

#include <osg/Geometry>
#include <osg/TextureCubeMap>
#include <osg/Texture2D>
#include <osg/Image>
#include <osg/Texture1D>
#include <osgDB/ReadFile>
#include <osg/BlendFunc>


#include <assert.h>


namespace osgHimmel
{

// Transforms a quad to the moons position into the canopy. Then 
// generates a circle with sphere normals (and normals from cube map)
// representing a perfect sphere in space.
// Applies lighting from sun - moon phase is always correct and no 
// separate calculation is required. Correct Moon rotation is currently
// faked (face towards earth is incorrect due to missing librations etc).

MoonGeode::MoonGeode(const char* cubeMapFilePath)
:   osg::Geode()

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   m_hquad(new HimmelQuad())

,   u_moon(NULL)
,   u_moonr(NULL)
,   u_moonCube(NULL)

,   u_R(NULL)
,   u_q(NULL)

,   u_sunShine(NULL)    // [0,1,2] = color; [3] = intensity
,   u_earthShine(NULL)  // [0,1,2] = color;

,   u_eclCoeffs(NULL) // rgb coefficients for eclipse coloring
,   u_eclParams(NULL) // e0, e1, e2, and brightness scalar

,   m_earthShineColor(defaultEarthShineColor())
,   m_earthShineScale(defaultEarthShineIntensity())
{
    setName("Moon");

    m_scale = defaultScale();

    osg::StateSet* stateSet = getOrCreateStateSet();

    setupNode(stateSet);
    setupUniforms(stateSet);
    setupShader(stateSet);
    setupTextures(stateSet, cubeMapFilePath);

    addDrawable(m_hquad);
};


MoonGeode::~MoonGeode()
{
};


void MoonGeode::update(const Himmel &himmel)
{
    const osg::Vec3 moonv = himmel.astro()->getMoonPosition(false);
    const osg::Vec3 sunv = himmel.astro()->getSunPosition(false);

    const float moons = tan(himmel.astro()->getAngularMoonRadius() * m_scale);

    u_moon->set(osg::Vec4(moonv, moons));

    const osg::Vec3 moonrv = himmel.astro()->getMoonPosition(true);
    u_moonr->set(osg::Vec4(moonrv, moonv[3]));

    u_R->set(himmel.astro()->getMoonOrientation());

    u_earthShine->set(m_earthShineColor 
        * himmel.astro()->getEarthShineIntensity() * m_earthShineScale);


    // TODO: starmap and planets  and stars also require / use this ... - find better place 
    // issue 11..
    const float fov = himmel.getCameraFovHint();
    const float height = himmel.getViewSizeHeightHint();
 
    u_q->set(static_cast<float>(tan(_rad(fov * 0.5)) / (height * 0.5)));


    // approximate umbra and penumbra size in moon radii

    float e0 = 0, e1 = 0, e2 = 0;

    float B = 0.f; // 0 hints that eclipse is not happening

    // This allows skipping of the 1d eclipse texture look-up 
    // as well as all other eclipse related calcs.

    if(acos(sunv * moonv) > _PI_2)
    {

        const float dm  = himmel.astro()->getMoonDistance();
        const float ds  = himmel.astro()->getSunDistance();

        const float ids = 1.f / ds;

        // scale for the normalized earth-moon system
        e0  = himmel.astro()->getMoonRadius() / dm;
        e1  = 3.6676 - (397.0001 * dm) * ids;
        e2  = 3.6676 + (404.3354 * dm) * ids;

        // shortest, normalized distance between earth and moon-sun line
        const float Dm = (moonv ^ sunv).length() / e0;

        if(Dm < (e2 + 1.f))
        {
            // transform the distance from 0 to e2 + 1 into [0,1] range for simpler brightness term
            const float t = Dm < (e1 + 1) ? Dm / (2.f + 2.f * e1) : 0.5 + (Dm - e1 - 1.f) / (2.f * (e2 - e1));

            // brightness scalar of the eclipse phenomena as function of the distance
            B = 1.0 + 29 * (1.0 - _smoothstep_ext(t, 0.2, 0.44));
        }        
    }

    // encode in uniform
    u_eclParams->set(osg::Vec4(e0, e1, e2, B));
}


void MoonGeode::addUniformsToVariousStateSate(osg::StateSet* stateSet)
{
    if(!stateSet)
        return;

    assert(u_moon);
    stateSet->addUniform(u_moon);
    assert(u_moonr);
    stateSet->addUniform(u_moonr);
    assert(u_sunShine);
    stateSet->addUniform(u_sunShine);
    assert(u_earthShine);
    stateSet->addUniform(u_earthShine); 
}


void MoonGeode::setupUniforms(osg::StateSet* stateSet)
{
    u_moon = new osg::Uniform("moon", osg::Vec4(0.f, 0.f, 1.f, 1.f)); // [3] = apparent angular radius (not diameter!)
    stateSet->addUniform(u_moon);

    u_moonr = new osg::Uniform("moonr", osg::Vec4(0.f, 0.f, 1.f, 1.f)); // [3] = apparent angular radius (not diameter!)
    stateSet->addUniform(u_moonr);

    u_moonCube = new osg::Uniform("moonCube", 0);
    stateSet->addUniform(u_moonCube);

    u_eclCoeffs = new osg::Uniform("eclCoeffs", 1);
    stateSet->addUniform(u_eclCoeffs);

    u_eclParams = new osg::Uniform("eclParams", osg::Vec4(0.f, 0.f, 0.f, -1.f));
    stateSet->addUniform(u_eclParams);

    u_q = new osg::Uniform("q", 0.0f);
    stateSet->addUniform(u_q);

    u_R = new osg::Uniform("R", osg::Matrixd());
    stateSet->addUniform(u_R);

    u_sunShine = new osg::Uniform("sunShine"
        , osg::Vec4(defaultSunShineColor(), defaultSunShineIntensity()));
    stateSet->addUniform(u_sunShine);

    u_earthShine = new osg::Uniform("earthShine"
        , osg::Vec3(osg::Vec3(0, 0, 0)));
    stateSet->addUniform(u_earthShine);
}


void MoonGeode::setupNode(osg::StateSet* stateSet)
{
    osg::BlendFunc *blend  = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    stateSet->setAttributeAndModes(blend, osg::StateAttribute::ON);
}


void MoonGeode::setupShader(osg::StateSet* stateSet)
{
    m_vShader->setShaderSource(getVertexShaderSource());
    m_fShader->setShaderSource(getFragmentShaderSource());

    m_program->addShader(m_vShader);
    m_program->addShader(m_fShader);

    stateSet->setAttributeAndModes(m_program, osg::StateAttribute::ON);
}

void MoonGeode::setupTextures(
    osg::StateSet* stateSet
,   const char* cubeMapFilePath)
{
    setupMoonTextureCube(stateSet, cubeMapFilePath);
    setupEclipseTexture(stateSet);
}


void MoonGeode::setupMoonTextureCube(
    osg::StateSet* stateSet
,   const char* cubeMapFilePath)
{
    osg::ref_ptr<osg::TextureCubeMap> tcm(new osg::TextureCubeMap);

    tcm->setUnRefImageDataAfterApply(true);

    tcm->setInternalFormat(GL_RGBA);

    tcm->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tcm->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    tcm->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

    tcm->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    tcm->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

    std::string px = cubeMapFilePath; px.replace(px.find("?"), 1, "_px");
    std::string nx = cubeMapFilePath; nx.replace(nx.find("?"), 1, "_nx");
    std::string py = cubeMapFilePath; py.replace(py.find("?"), 1, "_py");
    std::string ny = cubeMapFilePath; ny.replace(ny.find("?"), 1, "_ny");
    std::string pz = cubeMapFilePath; pz.replace(pz.find("?"), 1, "_pz");
    std::string nz = cubeMapFilePath; nz.replace(nz.find("?"), 1, "_nz");

    tcm->setImage(osg::TextureCubeMap::POSITIVE_X, osgDB::readImageFile(px));
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_X, osgDB::readImageFile(nx));
    tcm->setImage(osg::TextureCubeMap::POSITIVE_Y, osgDB::readImageFile(py));
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_Y, osgDB::readImageFile(ny));
    tcm->setImage(osg::TextureCubeMap::POSITIVE_Z, osgDB::readImageFile(pz));
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_Z, osgDB::readImageFile(nz));

    stateSet->setTextureAttributeAndModes(0, tcm, osg::StateAttribute::ON);

    u_moonCube->set(0);
}


void MoonGeode::setupEclipseTexture(osg::StateSet* stateSet)
{
    // generate lunar eclipse 2d-texture

    const int sizeS = 128;

    float *map = new float[sizeS * 3];

    const osg::Vec3 le0 = osg::Vec3(1.0, 1.0, 1.0) * 0.900f;
    const osg::Vec3 le1 = osg::Vec3(1.0, 1.0, 1.0) * 0.088f;
    const osg::Vec3 le2 = osg::Vec3(0.4, 0.7, 1.0) * 0.023f;
    const osg::Vec3 le3 = osg::Vec3(0.3, 0.5, 1.0) * 0.040f;

    const float s_u = 0.05;

    for(int s = 0; s < sizeS; ++s)
    {
        const float fs = static_cast<float>(s) / sizeS;
        const unsigned int i = s * 3;

        osg::Vec3 l = osg::Vec3(1, 1, 1);

        // remove the penumbral soft shadow from the moons coloring
        l -= le0 * (1.0 - _clamp(0.0, 1.0, 2 * fs - 1));
        // remove the umbral hard shadow from the moons coloring
        l -= le1 * (1 - _smoothstep_ext(fs, 0.5 * (1.0 - s_u), 0.5 * (1.0 + s_u)));
        // add reddish darkening towards umbral center from atmosphere scattering, linear scaling within the umbral distance of e1
        l -= le2 * (fs < 0.5 ? _clamp(0, 1, - 2.0 * fs + 1) : 0);
        // account for blue scattered light visible at the outer edge of the umbral shadow
        l += le3 * (_smoothstep_ext(fs, 0.5 * (1 - 4 * s_u), 0.5 * (1 + s_u))) * (1 - _smoothstep_ext(fs, 0.5, 1.0));

        map[i + 0] = l[0] < 0 ? 0 : l[0];
        map[i + 1] = l[1] < 0 ? 0 : l[1];
        map[i + 2] = l[2] < 0 ? 0 : l[2];
    }

    //

    osg::ref_ptr<osg::Image> image = new osg::Image();
    image->setImage(sizeS, 1, 1, GL_RGB32F_ARB, GL_RGB, GL_FLOAT, reinterpret_cast<unsigned char*>(map), osg::Image::USE_NEW_DELETE);

    osg::ref_ptr<osg::Texture1D> eclipse = new osg::Texture1D(image);

    eclipse->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    eclipse->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    eclipse->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);

    int unit;
    u_eclCoeffs->get(unit);

    stateSet->setTextureAttributeAndModes(unit, eclipse, osg::StateAttribute::ON);
}



const float MoonGeode::setScale(const float scale)
{
    osg::Vec4 temp;
    u_moon->get(temp);

    temp[3] = temp[3] / m_scale * scale;
    u_moon->set(temp);

    m_scale = scale;

    return getScale();
}

const float MoonGeode::getScale() const
{
    return m_scale;
}

const float MoonGeode::defaultScale()
{
    return 2.f;
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
    return osg::Vec3(0.923, 0.786, 0.636);
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
    return 128.f;
}


const osg::Vec3 MoonGeode::setEarthShineColor(const osg::Vec3 &color)
{
    m_earthShineColor = color;
    return m_earthShineColor;
}

const osg::Vec3 MoonGeode::getEarthShineColor() const
{
    return m_earthShineColor;
}

const osg::Vec3 MoonGeode::defaultEarthShineColor()
{
    //return osg::Vec3(0.92, 0.96, 1.00);
    return osg::Vec3(0.88, 0.96, 1.00);
}


const float MoonGeode::setEarthShineIntensity(const float intensity)
{
    m_earthShineScale = intensity;
    return m_earthShineScale;
}

const float MoonGeode::getEarthShineIntensity() const
{
    return m_earthShineScale;
}

const float MoonGeode::defaultEarthShineIntensity()
{
    return 1.0f;
}




const std::string MoonGeode::getVertexShaderSource()
{
    return glsl_version_150() +

    +   PRAGMA_ONCE(main,

        // moon.xyz is expected to be normalized and moon.a the moons
        // angular diameter in rad.
        "uniform vec4 moon;\n"
        "uniform vec4 moonr;\n"
        "\n"
        "out mat3 m_tangent;\n"
        "out vec3 m_eye;\n"
        "\n"
        //"const float SQRT2 = 1.41421356237;\n"
        //"\n"
        "void main(void)\n"
        "{\n"
        "    vec3 m = moonr.xyz;\n"
        "\n"
        //  tangent space of the unitsphere at m.
        "    vec3 u = normalize(cross(vec3(0, 0, 1), m));\n"
        "    vec3 v = normalize(cross(m, u));\n"
        "    m_tangent = mat3(u, v, m);\n"
        "\n"
        //"    float mScale = tan(moon.a) * SQRT2;\n"
        //"\n"
        //"    m_eye = m - normalize(gl_Vertex.x * u + gl_Vertex.y * v) * mScale;\n"
        "    m_eye = m - (gl_Vertex.x * u + gl_Vertex.y * v) * moon.a;\n"
        "\n"
        "    gl_TexCoord[0] = gl_Vertex;\n"
        "    gl_Position = gl_ModelViewProjectionMatrix * vec4(m_eye, 1.0);\n"
        "}");
}


const std::string MoonGeode::getFragmentShaderSource()
{
    return glsl_version_150()
    
    +   glsl_cmn_uniform()
    +   glsl_horizon()

    +   glsl_scattering()

    +   PRAGMA_ONCE(main,

        "uniform vec3 sun;\n"
        "\n"
        // moon.xyz is expected to be normalized and moon.a the moons
        // angular diameter in rad.
        "uniform vec4 moon;\n"
        "\n"
        "uniform samplerCube moonCube;\n"
        "\n"
        "uniform sampler1D eclCoeffs;\n"
        "uniform vec4 eclParams;\n"
        "\n"
        "uniform float q;\n"
        "uniform mat4 R;\n"
        "\n"
        "uniform vec4 sunShine;\n"   // rgb as color and w as intensity.
        "uniform vec3 earthShine;\n" // rgb as color with premultiplied intensity and scale.
        "\n"
        "const float radius = 0.98;\n"
        "const float surface = 1.0;\n"
        "\n"
        "in vec3 m_eye;\n"
        "in mat3 m_tangent;\n"
        "\n"
        "const float PI               = 3.1415926535897932;\n"
        "const float PI_05            = 1.5707963267948966;\n"
        "const float TWO_OVER_THREEPI = 0.2122065907891938;\n"
        "\n"

        // Hapke-Lommel-Seeliger approximation of the moons reflectance function.

        "float brdf(\n"
        "    float cos_r\n"
        ",   float cos_i\n"
        ",   float cos_p)\n"
        "{\n"
        // i between incident  (+sun) and surface
        // r between reflected (-eye) and surface
        // p as i + r

        "    float g = 0.6;\n" // surface densitiy parameter which determines the sharpness of the peak at the full Moon
        "    float t = 0.1;\n" // small amount of forward scattering
        "\n"
        // The following two params have various fixes applied
        "    float p = cos_p < 1 ? acos(cos_p) : 0;\n"
        "    float p05 = abs(p + step(0, -p) * 0.000001) * 0.5;\n"
        "\n"
        "    float tan_p = tan(p);\n"
        "\n"
        // Retrodirective. - Formular (Hapke66.3)
        "    float s = 1 - step(PI_05, p);\n"
        "    float e = - exp(-g / tan_p);\n"
        "    float B = s * (2 - tan_p / (2 * g) * (1 + e) * (3 + e))  + (1 - s);\n"
        "\n"
        // Scattering
        //    float S = (sin(p) + (PI - p) * cos_p) / PI
        //        + t * (1.0 - cos_p * 0.5) * (1.0 - cos_p * 0.5);

        // improved (Hapke66.12) term
        "    float S = (1 - sin(p05) * tan(p05) * log(tan(PI_05 - p05 * 0.5)))\n"
        "            + t * (1 - cos_p) * (1 - cos_p);\n"
        "\n"
        // BRDF
        "    float F = TWO_OVER_THREEPI * B * S / (1.0 + cos_r / cos_i);\n"
        "\n"
        "    if(cos_i > 0)\n"
        "        return 0.0;\n"
        "    return (1.0 - step(0, cos_i)) * F;\n"
        "}\n"
        "\n"


        "vec3 eclipse(vec3 m, vec3 s, vec3 n, sampler1D e\n"
        ",   float e0, float e1, float e2, float b)\n"
        "{\n"
        "    if(b <= 0)\n"
        "        return vec3(1);\n"
        "\n"
        "    float Df = length(cross(m - n * e0, s)) / e0;\n"
        "\n"
        "    float t;\n"
        "    if(Df < e1)\n"
        "        t = Df / (2.0 * e1);\n"
        "    else\n"
        "        t = 0.5 + (Df - e1) / (2 * (e2 - e1));\n"
        "\n"
        "    return texture1D(e, t).rgb * b;\n"
        "}\n"
        "\n"


        "void main(void)\n"
        "{\n"
        "    float x = gl_TexCoord[0].x;\n"
        "    float y = gl_TexCoord[0].y;\n"
        "\n"

        "    float zz = 1.0 - x * x - y * y;\n"
        "    float w  = smoothstep(0.0, 2.828 * q / moon.a, zz);\n" // fov and size indepentent antialiasing 
        "    if(w <= 0.0)\n"
        "        discard;\n"
        "\n"
        "    vec3 eye = normalize(m_eye.xyz);\n"
        "\n"
        "    if(belowHorizon(eye))\n"
        "        discard;\n"
        "\n"
        "    vec3 v = vec3(x, y, sqrt(zz));\n"
        "\n"

        // horizontal space
        "    vec3 hn = m_tangent * v;\n"
        "\n"

        // apply orientation for correct "FrontFacing" with optical librations in selenocentric space
        "    vec3 sn = normalize((vec4(v, 1.0) * R).xyz);\n"
        "\n"
        
        // fetch albedo and normals
        "    vec4 c  = textureCube(moonCube, sn);\n"
        "    vec3 cn = vec3(c.r * 2 - 1, c.g * 2 - 1, c.b);\n"
        "\n"
        
        // convert normals to selenocentric space
        "    mat3 sm  = mat3(v.zxy, v.yzx, v);\n"
        "    vec3 s_n = sm * cn;\n"
        "\n"
        
        // convert normals to horizontal space
        "    vec3 h_n = mix(hn, m_tangent * s_n, surface);\n"
        "\n"
        
        // brdf
        "    float cos_p = dot(-eye, sun);\n"
        "    float cos_i = dot( sun, h_n);\n"
        "    float cos_r = dot(-eye, h_n);\n"
        "\n"
        "    float f = brdf(cos_r, cos_i, cos_p);\n"
        "\n"

        // accuire lunar eclipse coefficients
        "    vec3 e = eclipse(moon.xyz, sun, hn, eclCoeffs\n"
	    "       , eclParams[0], eclParams[1], eclParams[2], eclParams[3]);\n"
        "\n"
        
            // Day-Twilight-Night-Intensity Mapping (Butterworth-Filter)
        "    float b = 0.5 / sqrt(1 + pow(sun.z + 1.05, 32)) + 0.33;\n"
        "\n"

        // diffuse
        "    vec3 diffuse = earthShine;\n"
        "    diffuse += f * sunShine.w * b;\n"
        "\n"
        "    diffuse *= c.a;\n"
        "    diffuse  = max(vec3(0.0), diffuse);\n"
        "\n"
        "    diffuse *= sunShine.rgb;\n"
        "    diffuse *= e;\n"
        "\n"
        // TODO: make scattering coeff 4 as uniform...
        "    diffuse *= (1 - 4 * scatt(acos(eye.z)));\n"
        "\n"
        "    gl_FragColor = w * vec4(diffuse, 1.0);\n"
        "}");
}




#ifdef OSGHIMMEL_EXPOSE_SHADERS

osg::Shader *MoonGeode::getVertexShader()
{
    return m_vShader;
}
osg::Shader *MoonGeode::getGeometryShader()
{
    return NULL;
}
osg::Shader *MoonGeode::getFragmentShader()
{
    return m_fShader;
}

#endif // OSGHIMMEL_EXPOSE_SHADERS

} // namespace osgHimmel