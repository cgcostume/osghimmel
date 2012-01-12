
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

#include "proceduralhimmel.h"

#include "mathmacros.h"
#include "atime.h"
#include "timef.h"
#include "sideraltime.h"
#include "earth.h"
#include "sun.h"
#include "moon.h"

#include "himmelquad.h"


ProceduralHimmel::ProceduralHimmel()
:   AbstractHimmel()
,   m_latitude(0)
,   m_longitude(0)
,   m_moonScale(1.f)
,   u_sun (new osg::Uniform("sun",  osg::Vec4(1.0, 0.0, 0.0, 1.0))) // [3] = apparent angular radius (not diameter!)
,   u_moon(new osg::Uniform("moon", osg::Vec4(0.0, 0.0, 1.0, 1.0))) // [3] = apparent angular radius (not diameter!)
,   u_ditheringMultiplier(new osg::Uniform("ditheringMultiplier", 1.0f))
{
    getOrCreateStateSet()->addUniform(u_sun);
    getOrCreateStateSet()->addUniform(u_moon);
    getOrCreateStateSet()->addUniform(u_ditheringMultiplier);

    addChild(m_hquad);
    moon_hack();

    stars_hack();
};


ProceduralHimmel::~ProceduralHimmel()
{
};


void ProceduralHimmel::update()
{
    AbstractHimmel::update();

    const t_aTime aTime(t_aTime::fromTimeF(*getTime()));
    const t_julianDay t(jd(aTime));

    t_horCoords sun = sun_horizontalPosition(aTime, m_latitude, m_longitude);

    osg::Vec3 sunv  = sun.toEuclidean();
    sunv.normalize();

    const osg::Vec4f::value_type aasr = earth_apparentAngularSunDiameter(t) * 0.5;

    u_sun->set(osg::Vec4(sunv, aasr));


    t_horCoords moon = moon_horizontalPosition(aTime, m_latitude, m_longitude);
    osg::Vec3 moonv  = moon.toEuclidean();
    moonv.normalize();

    const osg::Vec4f::value_type aamr = earth_apparentAngularMoonDiameter(t) * 0.5 * m_moonScale;

    u_moon->set(osg::Vec4(moonv, aamr));


    //// TODO: MOON HACK

    u_rot->set(osg::Matrix::rotate(osg::Vec3(0.0, 1.0, 0.0), moonv));

    // TODO: Star HACK

    if(m_cameraHint)
    {
        double fov;
        double dummy;
        m_cameraHint->getProjectionMatrixAsPerspective(fov, dummy, dummy, dummy);

        static const float TWO_TIMES_SQRT2 = 2.0 * sqrt(2.0);

        if(m_widthHint > 0)
            u_starWidth->set((float)(tan(_rad(fov) / m_heightHint) * 2.0));
    }
}


const long double ProceduralHimmel::setLatitude(const long double latitude)
{
    m_latitude = _clamp(-90, +90, latitude);
    return getLatitude();
}

const long double ProceduralHimmel::getLatitude() const
{
    return m_latitude;
}


const long double ProceduralHimmel::setLongitude(const long double longitude)
{
    m_longitude = _clamp(-180, +180, longitude);
    return getLongitude();
}

const long double ProceduralHimmel::getLongitude() const
{
    return m_longitude;
}


const float ProceduralHimmel::setMoonScale(const float moonScale)
{
    m_moonScale = moonScale;
    return getMoonScale();
}

const float ProceduralHimmel::getMoonScale() const
{
    return m_moonScale;
}


const float ProceduralHimmel::setDitheringMultiplier(const float multiplier)
{
    u_ditheringMultiplier->set(multiplier);
    return getDitheringMultiplier();
}

const float ProceduralHimmel::getDitheringMultiplier() const
{
    float multiplier;
    u_ditheringMultiplier->get(multiplier);

    return multiplier;
}


#include "shadermodifier.h"
#include <osg/TextureCubeMap>
#include <osg/Texture2D>

#include <osgDB/ReadFile>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/BlendFunc>
#include <osg/Depth>

void ProceduralHimmel::moon_hack()
{
// - Hack that transforms a quad to the moons position into the canopy.
// - Generates circle with sphere normals and adds normals from moon cube map
// - Applies lighting from sun - moon phase is correct (no extra calc for that ;))
// - TODO: Correct Moon rotation (Face towards earth is still wrong)

#pragma NOTE("Beautify this!")

    m_mquad = new HimmelQuad();

    addChild(m_mquad);

    m_mprogram = new osg::Program;
    m_mvShader = new osg::Shader(osg::Shader::VERTEX);
    m_mfShader = new osg::Shader(osg::Shader::FRAGMENT);
 //   m_mgShader = new osg::Shader(osg::Shader::GEOMETRY);

    osg::StateSet* stateSet = m_mquad->getOrCreateStateSet();

   // setupNode(stateSet);
 
    stateSet->addUniform(u_moon);
    stateSet->addUniform(u_sun);


    u_mcube = new osg::Uniform("mooncube", 0);
    stateSet->addUniform(u_mcube);

    u_rot = new osg::Uniform("rot", osg::Matrixd());
    stateSet->addUniform(u_rot);


    osg::Depth* depth = new osg::Depth(osg::Depth::LESS);    // This is to not render inside the moon.
    stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);


    //m_mprogram->addShader(m_mgShader);
    m_mprogram->addShader(m_mvShader);
    m_mprogram->addShader(m_mfShader);

    stateSet->setAttributeAndModes(m_mprogram, osg::StateAttribute::ON);
            
    
    osg::BlendFunc *blend  = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE);
    stateSet->setAttributeAndModes(blend, osg::StateAttribute::ON);

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


 //   m_mgShader->loadShaderSourceFromFile("D:/p/osghimmel/tempg.txt");
    m_mvShader->loadShaderSourceFromFile("D:/p/osghimmel/tempv.txt");
    m_mfShader->loadShaderSourceFromFile("D:/p/osghimmel/tempf.txt");
    

/* VERTEX


#version 150 compatibility

uniform vec4 moon;

out mat4 test;
out vec3 m_eye;

const float SQRT2 = 1.41421356237;

void main(void)
{
	vec3 m = moon.xyz;

	vec3 u = normalize(cross(m, vec3(1)));
	vec3 v = normalize(cross(u, m));

	test = mat4(vec4(u, 0.0), vec4(v, 0.0), vec4(m, 0.0), vec4(vec3(0.0), 1.0));

	float mScale = tan(moon.a) * SQRT2;
    vec3 f = m - normalize(gl_Vertex.x * u + gl_Vertex.y * v) * mScale;
	m_eye = f;


	gl_TexCoord[0] = gl_Vertex;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(f, 1.0);
}



*/


/* FRAGMENT


#version 150 compatibility

uniform vec4 moon; // expected to be normalized
uniform vec4 sun;  // expected to be normalized

uniform samplerCube moonCube;

const float radius = 0.98;

in vec3 m_eye;

in mat4 test;

uniform mat4 rot;

// TODO: shorten
const float PI = 3.1415926535897932384626433832795;
const float TWO_OVER_THREEPI = 0.21220659078919378102517835116335;

void main(void)
{
	float x = gl_TexCoord[0].x;
	float y = gl_TexCoord[0].y;

	float zz = radius * radius - x * x - y * y;
			
	if(zz < 0.02)
		discard;

	float z = sqrt(zz);

	mat4 m = test;;

	vec3 mn = normalize((m * vec4(x, y, z, 1.0)).xyz);
	vec3 mt = mn.zyx;
	vec3 mb = mn.xzy;

	vec3 q = (vec4(mn.x, mn.y, mn.z, 1.0) * rot).xyz;
	vec4 c  = textureCube(moonCube, vec3(-q.x, q.y, -q.z));
	vec3 cn = normalize((c.xyz) * 2.0 - 1.0);

	vec3 n = normalize(vec3(dot(cn, mt), dot(cn, mb), dot(cn, mn)));


	vec3 l = normalize(sun.xyz);
	vec3 e = normalize(m_eye.xyz);

	float cos_p = clamp(dot(e, l), 0.0, 1.0);
	float p     = acos(cos_p);
	float tan_p = tan(p);

	float dot_ne = dot(n, e);
	float dot_nl = dot(n, l);

	float g = 0.6; // surface densitiy parameter which determines the sharpness of the peak at the full Moon
	float t = 0.1; // small amount of forward scattering


	float R = 2.0 - tan_p / (2.0 * g)
		* (1.0 - exp(-g / tan_p)) 
		* (3.0 - exp(-g / tan_p));

	float S = (sin(p) + (PI - p) * cos_p) / PI
		+ t * (1.0 - cos_p) * (1.0 - cos_p);

	float F = TWO_OVER_THREEPI * R * S * 1.0 / (1.0 + (-dot_ne) / dot_nl);

	if(dot_nl > 0.0)
		F = 0.0;

	// From "nightsky" paper [46] -> Van De Hulst, H. - "Light Scattering" - 1980
	float p2 = (PI - acos(dot(-moon.xyz, sun.xyz))) * 0.5;
	float Eem = 0.19 * 0.5 * (1.0 - sin(p2) * tan(p2) * log(1.0 / tan(p2 * 0.5)));
	
	// My approximation with non-perceivable difference.
//	float p2f = dot(-moon.xyz, sun.xyz);
//	float Eemf = 0.1 * (p2f * p2f);

	gl_FragColor = vec4(c.a *
		( vec3(1.0, 1.02, 1.04) * Eem
      	+ vec3(1.12, 1.10, 0.98) * F * p * 32.0 ), 1.0);


	// debug

//	gl_FragColor = vec4(x * 0.5 + 0.5, y * 0.5 + 0.5, 0.0, 0.0);
//	gl_FragColor = vec4(mn * 0.5 + 0.5, 1.0);
//	gl_FragColor = vec4(n * 0.5 + 0.5, 1.0);
//	gl_FragColor = vec4(vec3(cd), 1.0);
//	gl_FragColor = vec4(d * vec3(1.06, 1.06, 0.98), 1.0);
}




*/



#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
    if(shaderModifier())
    {
        shaderModifier()->registerShader("moon_hack", m_mfShader);
        shaderModifier()->registerShader("moon_hack", m_mvShader);
        //shaderModifier()->registerShader("moon_hack", m_mgShader);
    }
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER
}




#include <osg/Point>





namespace 
{
	#define NUM_RANDS 100000
	static int randI = 0;

	static double randds[NUM_RANDS];
	static double randfs[NUM_RANDS];
}


void initializeRands()
{
	for(int i = 0; i < NUM_RANDS; ++i)
		randds[i] = rand() % 351371 / 351371.0;	// use prime number
	for(int i = 0; i < NUM_RANDS; ++i)
        randfs[i] = rand() / (RAND_MAX * 1.0f);	// use prime number
}


const double randd() 
{
	randI = (randI + 1) % NUM_RANDS; 
	return randds[randI];
}


const float randf()
{
	randI = (randI + 1) % NUM_RANDS; 
	return randfs[randI];
}



class StarPoints : public osg::Geometry
{
public:
    StarPoints()
    {
        
        initializeRands();

        osg::Vec4Array* cAry = new osg::Vec4Array;
        setColorArray( cAry );
        setColorBinding( osg::Geometry::BIND_OVERALL );
        cAry->push_back( osg::Vec4(1,0,0,1) );

        osg::Vec4Array* vAry = new osg::Vec4Array();
        setVertexArray( vAry );

        for(int i = 0; i < 9110; i++)
        {
            osg::Vec4 v = osg::Vec4((rand() % 36000 / 36000.0) - 0.5, (rand() % 36000 / 36000.0) - 0.5, (rand() % 36000 / 36000.0) - 0.5, (rand() % 32 / 32.0));
            v.normalize();
            vAry->push_back(v);
        }

        addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vAry->size() ) );

        osg::StateSet* sset = getOrCreateStateSet();

        // if things go wrong, fall back to big points
        osg::Point* p = new osg::Point;
        p->setSize(6);
        sset->setAttribute( p );

    }

};



void ProceduralHimmel::stars_hack()
{

    osg::Geode* root( new osg::Geode );
    root->addDrawable( new StarPoints() );


    addChild(root);

    m_sprogram = new osg::Program;
    m_svShader = new osg::Shader(osg::Shader::VERTEX);
    m_sfShader = new osg::Shader(osg::Shader::FRAGMENT);
    m_sgShader = new osg::Shader(osg::Shader::GEOMETRY);
 
    osg::StateSet* stateSet = root->getOrCreateStateSet();


        u_starWidth = new osg::Uniform("starWidth", 1.0f);
    stateSet->addUniform(u_starWidth);


    stateSet->setMode( GL_BLEND, osg::StateAttribute::ON );

    //stateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF); 

        osg::BlendFunc *blend  = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE);
        stateSet->setAttributeAndModes(blend, osg::StateAttribute::ON);

    //osg::Depth* depth = new osg::Depth(osg::Depth::ALWAYS, 0.0, 1.0);
    //stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

    stateSet->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
 
    stateSet->addUniform(u_moon);
    stateSet->addUniform(u_sun);


    m_sprogram->addShader(m_sgShader);
    m_sprogram->addShader(m_svShader);
    m_sprogram->addShader(m_sfShader);
    
    stateSet->setAttributeAndModes(m_sprogram, osg::StateAttribute::ON);

    m_sgShader->loadShaderSourceFromFile("D:/p/osghimmel/tempsg.txt");
    m_svShader->loadShaderSourceFromFile("D:/p/osghimmel/tempsv.txt");
    m_sfShader->loadShaderSourceFromFile("D:/p/osghimmel/tempsf.txt");


/* VERTEX


*/


/* FRAGMENT



*/



#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
    if(shaderModifier())
    {
        shaderModifier()->registerShader("stars_hack", m_sfShader);
        shaderModifier()->registerShader("stars_hack", m_svShader);
        shaderModifier()->registerShader("stars_hack", m_sgShader);
    }
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER
}








// VertexShader

#include "shaderfragment/version.vsf"
#include "shaderfragment/quadretrieveray.vsf"
#include "shaderfragment/quadtransform.vsf"

const std::string ProceduralHimmel::getVertexShaderSource()
{
    return glsl_v_version

        +   glsl_v_quadRetrieveRay
        +   glsl_v_quadTransform
        +
        "out vec4 m_ray;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    m_ray = quadRetrieveRay();\n"
        "    quadTransform();\n"
        "}\n\n";
}


// FragmentShader

#include "shaderfragment/version.fsf"
#include "shaderfragment/pseudo_rand.fsf"
#include "shaderfragment/dither.fsf"

const std::string ProceduralHimmel::getFragmentShaderSource()
{
    return glsl_f_version
    
    +
        "in vec4 m_ray;\n"
        "\n"
        "uniform int osg_FrameNumber;\n"    // required by pseudo_rand
        "\n"

    +   glsl_f_pseudo_rand
    +   glsl_f_dither
    +
        // Color Retrieval

        "uniform vec4 sun;\n"
        //"uniform vec3 moon;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    vec3 stu = normalize(m_ray.xyz);\n"
        "\n"
        "    float s = 1.0 / length(normalize(sun.xyz) - stu)  * 0.08;\n"
        //"    float m = 1.0 / length(normalize(moon) - stu) * 0.08;\n"
        "\n"
	    "    vec3 su = vec3(0.6, .5, 0.4) * s;\n"
	    //"    vec3 mo = vec3(0.6, .6, 0.5) * clamp(m, 0.0, 2.0) * 0.66;\n"
        "\n"
        "    vec3 h = vec3(pow(1.0 - abs(stu.z), 2) * 0.8);\n"
        "\n"
        //"    gl_FragColor = vec4(mo + su + h, 1.0) + dither();\n"
        "    gl_FragColor = vec4(su + h, 1.0) + dither();\n"
        "}\n\n";
}

