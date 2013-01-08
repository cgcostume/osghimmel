
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

#include "abstractmappedhimmel.h"

#include "timef.h"
#include "himmelquad.h"
#include "coords.h"
#include "mathmacros.h"

#include "shaderfragment/common.h"


#include <osg/Geode>
#include <osg/MatrixTransform>

#include <assert.h>
#include <limits>


#pragma NOTE("detach sun stuff to sun composite")

namespace 
{
    const GLint BACK_TEXTURE_INDEX(0);
    const GLint SRC_TEXTURE_INDEX (1);
}

namespace osgHimmel
{

AbstractMappedHimmel::AbstractMappedHimmel(
    const bool fakeSun)
:   AbstractHimmel()

,   m_hquad(new HimmelQuad())

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   u_srcAlpha  (new osg::Uniform("srcAlpha", 0.f))
,   u_back      (new osg::Uniform("back", BACK_TEXTURE_INDEX))
,   u_src       (new osg::Uniform("src",  SRC_TEXTURE_INDEX))

,   u_razInverse(NULL)
,   u_sun(NULL)
,   u_sunCoeffs(NULL)
,   u_sunScale(NULL)

,   m_activeBackUnit(std::numeric_limits<GLint>::max())
,   m_activeSrcUnit( std::numeric_limits<GLint>::max())

,   m_razTransform(new osg::MatrixTransform())
,   m_razDirection(RD_NorthWestSouthEast)
,   m_razTimef(new TimeF())

,   m_fakeSun(fakeSun)
{
    osg::StateSet* stateSet = getOrCreateStateSet();

    setupProgram(stateSet);
    setupUniforms(stateSet);

    m_razTimef->start();

    // Encapsulate hQuad into MatrixTransform.

    osg::Geode *geode = new osg::Geode();
    geode->addDrawable(m_hquad);

    m_razTransform->addChild(geode);
    addChild(m_razTransform);
};


AbstractMappedHimmel::~AbstractMappedHimmel()
{
    unmakeVertexShader();
    unmakeFragmentShader();

    delete m_razTimef;
};


void AbstractMappedHimmel::update()
{
    AbstractHimmel::update();

    // Update rotation around zenith.

    const float razd(m_razDirection == RD_NorthWestSouthEast ? 1.f : -1.f);
    m_razTransform->setMatrix(
        osg::Matrixf::rotate(razd * m_razTimef->getf(true) * osg::PI * 2.f
    ,   osg::Vec3f(0.f, 0.f, 1.f)));

    if(u_razInverse)
        u_razInverse->set(osg::Matrixf::inverse(m_razTransform->getMatrix()));


#pragma NOTE("interface for FakeSun required")

    const float t(timef());

    if(u_sun)
    {
        t_equd equ;
        // TODO: fix this
        equ.declination = _deg(t * _PI_2);
        equ.right_ascension = _deg(t * _PI2 - _PI);

        u_sun->set(equ.toEuclidean());
    }

    // Update two texture status for arbitrary blending (e.g. normal).

    // update texture change
    u_srcAlpha->set(m_changer.getSrcAlpha(t));

    // Avoid unnecessary unit switches.

    const GLint backUnit(m_changer.getBackUnit(t));
    if(backUnit != m_activeBackUnit)
    {
        assignBackUnit(backUnit);
        assert(backUnit == m_activeBackUnit);
    }

    const GLint srcUnit(m_changer.getSrcUnit(t));
    if(srcUnit != m_activeSrcUnit)
    {
        assignSrcUnit(srcUnit);
        assert(srcUnit == m_activeSrcUnit);
    }
}


void AbstractMappedHimmel::assignBackUnit(const GLint textureUnit)
{
    assignUnit(textureUnit, BACK_TEXTURE_INDEX);
    m_activeBackUnit = textureUnit;
}


void AbstractMappedHimmel::assignSrcUnit(const GLint textureUnit)
{
    assignUnit(textureUnit, SRC_TEXTURE_INDEX);
    m_activeSrcUnit = textureUnit;
}


void AbstractMappedHimmel::assignUnit(
    const GLint textureUnit
,   const GLint targetIndex)
{
    osg::StateAttribute *sa(getTextureAttribute(textureUnit));

    if(sa)
        getOrCreateStateSet()->setTextureAttributeAndModes(targetIndex, sa);
    else
        getOrCreateStateSet()->setTextureAttributeAndModes(targetIndex, NULL, osg::StateAttribute::OFF);
}


void AbstractMappedHimmel::setupProgram(osg::StateSet *stateSet)
{
    m_program->addShader(m_vShader);
    m_program->addShader(m_fShader);

    stateSet->setAttributeAndModes(m_program, osg::StateAttribute::ON);
}


void AbstractMappedHimmel::setupUniforms(osg::StateSet *stateSet)
{
    stateSet->addUniform(u_srcAlpha);
    stateSet->addUniform(u_back);
    stateSet->addUniform(u_src);

    if(m_fakeSun)
    {
        u_razInverse = new osg::Uniform("razInverse", osg::Matrixf());

        u_sun       = new osg::Uniform("sun", osg::Vec3f(1.0, 0.0, 1.0));
        u_sunCoeffs = new osg::Uniform("sunCoeffs", defaultSunCoeffs());
        u_sunScale  = new osg::Uniform("sunScale", 1.f);

        stateSet->addUniform(u_razInverse);
        
        stateSet->addUniform(u_sun);
        stateSet->addUniform(u_sunCoeffs);
        stateSet->addUniform(u_sunScale);
    }
}


void AbstractMappedHimmel::postInitialize()
{
    makeVertexShader();
    makeFragmentShader();
}


void AbstractMappedHimmel::reassignShader()
{
    makeVertexShader();
    makeFragmentShader();    
}


void AbstractMappedHimmel::makeVertexShader()
{
    m_vShader->setShaderSource(getVertexShaderSource());
}


void AbstractMappedHimmel::unmakeVertexShader()
{
}


void AbstractMappedHimmel::makeFragmentShader()
{
    m_fShader->setShaderSource(getFragmentShaderSource());
}


void AbstractMappedHimmel::unmakeFragmentShader()
{
}




void AbstractMappedHimmel::setTransitionDuration(const float duration)
{
    m_changer.setTransitionDuration(duration);
}

const float AbstractMappedHimmel::getTransitionDuration() const
{
    return m_changer.getTransitionDuration();
}


void AbstractMappedHimmel::pushTextureUnit(
    const GLint textureUnit
,   const float time)
{
    m_changer.pushUnit(textureUnit, time);
}


void AbstractMappedHimmel::setSecondsPerRAZ(const float secondsPerRAZ)
{
    m_razTimef->setSecondsPerCycle(_abs(secondsPerRAZ));
}


const float AbstractMappedHimmel::getSecondsPerRAZ() const
{
    return m_razTimef->getSecondsPerCycle();
}


void AbstractMappedHimmel::setRazDirection(const e_RazDirection razDirection)
{
    m_razDirection = razDirection;
}

 
const AbstractMappedHimmel::e_RazDirection AbstractMappedHimmel::getRazDirection() const
{
    return m_razDirection;
}


const float AbstractMappedHimmel::setSunScale(const float scale)
{
    u_sunScale->set(scale);
    return getSunScale();
}

const float AbstractMappedHimmel::getSunScale() const
{
    float sunScale;
    u_sunScale->get(sunScale);

    return sunScale;
}


const osg::Vec4f AbstractMappedHimmel::setSunCoeffs(const osg::Vec4f &coeffs)
{
    u_sunCoeffs->set(coeffs);
    return getSunCoeffs();
}

const osg::Vec4f AbstractMappedHimmel::getSunCoeffs() const
{
    osg::Vec4f coeffs;
    u_sunCoeffs->get(coeffs);

    return coeffs;
}
const osg::Vec4f AbstractMappedHimmel::defaultSunCoeffs()
{
    return osg::Vec4f(0.63, 0.58, 0.49, 1.0);
}




const std::string AbstractMappedHimmel::getVertexShaderSource()
{
    return glsl_version_150()
    
    +   glsl_quadRetrieveRay()
    +   glsl_quadTransform()

    +   PRAGMA_ONCE(main,

        "uniform mat4 razInverse;\n"
        "\n"
        "out vec4 m_ray;\n"
        "out vec4 m_razInvariant;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    m_ray = quadRetrieveRay();\n"
        "    m_razInvariant = m_ray * razInverse;\n"
        "\n"
        "    quadTransform();\n"
        "}");
}




#ifdef OSGHIMMEL_EXPOSE_SHADERS

osg::Shader *AbstractMappedHimmel::getVertexShader()
{
    return m_vShader;
}
osg::Shader *AbstractMappedHimmel::getGeometryShader()
{
    return NULL;
}
osg::Shader *AbstractMappedHimmel::getFragmentShader()
{
    return m_fShader;
}

#endif // OSGHIMMEL_EXPOSE_SHADERS

} // namespace osgHimmel
