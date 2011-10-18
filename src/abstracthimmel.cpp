
// Copyright (c) 2011, Daniel Müller <dm@g4t3.de>
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

#include "abstracthimmel.h"

#include "timef.h"
#include "himmelquad.h"

#include <osg/Depth>


#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
#include "shadermodifier.h"
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER


void AbstractHimmel::HimmelNodeCallback::operator()(
    osg::Node *node
,   osg::NodeVisitor *nv)
{
    osg::ref_ptr<AbstractHimmel> himmel = dynamic_cast<AbstractHimmel*>(node);

    if(himmel)
        himmel->update();

    traverse(node, nv); 
}


AbstractHimmel::AbstractHimmel()
:   osg::Transform()
,   m_timef(NULL)
,   m_autoUpdateTime(false)

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   m_hquad(new HimmelQuad())

,   m_initialized(false)
{
    osg::StateSet* stateSet = getOrCreateStateSet();

    setupNode(stateSet);
    setupProgram(stateSet);

    addChild(m_hquad);

    setUpdateCallback(new HimmelNodeCallback);
};


AbstractHimmel::~AbstractHimmel()
{
    unmakeVertexShader();
    unmakeFragmentShader();
};


void AbstractHimmel::setupNode(osg::StateSet* stateSet)
{
    // Only draw at back plane.
    osg::Depth* depth = new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0);
    stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);
}


void AbstractHimmel::setupProgram(osg::StateSet *stateSet)
{
    m_program->addShader(m_vShader);
    m_program->addShader(m_fShader);

    stateSet->setAttributeAndModes(m_program, osg::StateAttribute::ON);
}


void AbstractHimmel::initialize()
{
    if(m_initialized)
        return;

    m_initialized = true;

    makeVertexShader();
    makeFragmentShader();

    postInitialize();
}


void AbstractHimmel::makeVertexShader()
{
    m_vShader->setShaderSource(getVertexShaderSource());

#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
    if(shaderModifier())
        shaderModifier()->registerShader(getName(), m_vShader);
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER
}


void AbstractHimmel::unmakeVertexShader()
{
#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
    if(shaderModifier())
        shaderModifier()->unregisterShader(m_vShader);
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER
}


void AbstractHimmel::makeFragmentShader()
{
    m_fShader->setShaderSource(getFragmentShaderSource());

#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
    if(shaderModifier())
        shaderModifier()->registerShader(getName(), m_fShader);
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER
}


void AbstractHimmel::unmakeFragmentShader()
{
#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
    if(shaderModifier())
        shaderModifier()->unregisterShader(m_fShader);
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER
}



void AbstractHimmel::update()
{
    if(!m_initialized)
        initialize();

    if(m_autoUpdateTime && m_timef)
        m_timef->update();
}


void AbstractHimmel::assignTime(
    TimeF *timef
,   const bool autoUpdate)
{
    m_timef = timef;
    m_autoUpdateTime = autoUpdate;
}


void AbstractHimmel::setAutoUpdateTime(const bool autoUpdate)
{
    m_autoUpdateTime = autoUpdate;
}


const float AbstractHimmel::timef() const
{
    if(m_timef)
        return m_timef->getf();

    return 0.f;
}


#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER

ShaderModifier *AbstractHimmel::m_shaderModifier = NULL;

void AbstractHimmel::setupShaderModifier(ShaderModifier *shaderModifier)
{
    m_shaderModifier = shaderModifier;
}

ShaderModifier *AbstractHimmel::shaderModifier()
{  
    return m_shaderModifier;
}

#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER