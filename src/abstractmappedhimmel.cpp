
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

#include <osg/MatrixTransform>

#include <assert.h>
#include <limits.h>

namespace 
{
    static const GLint BACK_TEXTURE_INDEX(0);
    static const GLint SRC_TEXTURE_INDEX(1);
}


AbstractMappedHimmel::AbstractMappedHimmel(
    const bool fakeSun)
:   AbstractHimmel()
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

,   m_withFakeSun(fakeSun)
{
    getOrCreateStateSet()->addUniform(u_srcAlpha);
    getOrCreateStateSet()->addUniform(u_back);
    getOrCreateStateSet()->addUniform(u_src);

    if(m_withFakeSun)
    {
        u_razInverse = new osg::Uniform("razInverse", osg::Matrix());

        u_sun       = new osg::Uniform("sun", osg::Vec3(1.0, 0.0, 1.0));
        u_sunCoeffs = new osg::Uniform("sunCoeffs", defaultSunCoeffs());
        u_sunScale  = new osg::Uniform("sunScale", 1.f);

        getOrCreateStateSet()->addUniform(u_razInverse);
        
        getOrCreateStateSet()->addUniform(u_sun);
        getOrCreateStateSet()->addUniform(u_sunCoeffs);
        getOrCreateStateSet()->addUniform(u_sunScale);
    }

    // Encapsulate hQuad into MatrixTransform.

    m_razTransform->addChild(m_hquad);
    addChild(m_razTransform.get());
};


AbstractMappedHimmel::~AbstractMappedHimmel()
{
    delete m_razTimef;
};

#pragma NOTE("Move includes...")

#include "coords.h"
#include "mathmacros.h"

void AbstractMappedHimmel::update()
{
    AbstractHimmel::update();

    // Update rotation around zenith.

    const float razd(m_razDirection == RD_NorthWestSouthEast ? 1.f : -1.f);
    m_razTransform->setMatrix(
        osg::Matrix::rotate(razd * m_razTimef->getf(true) * osg::PI * 2.f
    ,   osg::Vec3(0.f, 0.f, 1.f)));

    if(u_razInverse)
        u_razInverse->set(osg::Matrix::inverse(m_razTransform->getMatrix()));


#pragma NOTE("Interface for FakeSun required")

    const float t(timef());

    if(u_sun)
    {
        t_equCoords equ;
        equ.declination = _deg(sin(t * _PI2));
        equ.right_ascension = _deg(cos(t * _PI2));

        t_horCoords hor = equ.toHorizontal(0, 120.0, 0);
        u_sun->set(hor.toEuclidean());
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


inline void AbstractMappedHimmel::assignBackUnit(const GLint textureUnit)
{
    assignUnit(textureUnit, BACK_TEXTURE_INDEX);
    m_activeBackUnit = textureUnit;
}


inline void AbstractMappedHimmel::assignSrcUnit(const GLint textureUnit)
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
    m_razTimef->setSecondsPerCycle(abs(secondsPerRAZ));
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


const osg::Vec4 AbstractMappedHimmel::setSunCoeffs(const osg::Vec4 &coeffs)
{
    u_sunCoeffs->set(coeffs);
    return getSunCoeffs();
}

const osg::Vec4 AbstractMappedHimmel::getSunCoeffs() const
{
    osg::Vec4 coeffs;
    u_sunCoeffs->get(coeffs);

    return coeffs;
}
const osg::Vec4 AbstractMappedHimmel::defaultSunCoeffs()
{
    return osg::Vec4(0.63, 0.58, 0.49, 1.0);
}




// VertexShader

#include "shaderfragment/version.vsf"
#include "shaderfragment/quadretrieveray.vsf"
#include "shaderfragment/quadtransform.vsf"

const std::string AbstractMappedHimmel::getVertexShaderSource()
{
    return glsl_v_version

        +   glsl_v_quadRetrieveRay
        +   glsl_v_quadTransform
        +
        "uniform mat4 razInverse;\n"
        "\n"
        "out vec4 m_ray;\n"
        "out vec4 m_rayFixed;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    m_ray = quadRetrieveRay();\n"
        "    m_rayFixed = m_ray * razInverse;\n"
        "\n"
        "    quadTransform();\n"
        "}\n\n";
}

