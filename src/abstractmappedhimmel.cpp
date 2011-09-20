
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

#include "abstractmappedhimmel.h"


AbstractMappedHimmel::AbstractMappedHimmel()
:   AbstractHimmel()
,   m_srcAlpha(new osg::Uniform("srcAlpha", 0.f))
,   m_back(new osg::Uniform("back", 0))
,   m_src(new osg::Uniform("src", 1))
{
    getOrCreateStateSet()->addUniform(m_srcAlpha);
    getOrCreateStateSet()->addUniform(m_back);
    getOrCreateStateSet()->addUniform(m_src);
};


AbstractMappedHimmel::~AbstractMappedHimmel()
{
};


void AbstractMappedHimmel::update()
{
    AbstractHimmel::update();

    // Update two texture status for arbitrary blending (e.g. normal).

    const float t(timef());

    m_srcAlpha->set(m_changer.getSrcAlpha(t));

    if(m_changer.hasBackChanged())
    {
        const GLint backUnit(m_changer.getBack(t));

        m_back->set(backUnit);
        setActiveBackUnit(backUnit);
    }

    if(m_changer.hasSrcChanged())
    {
        const GLint srcUnit(m_changer.getSrc(t));

        m_src->set(srcUnit);
        setActiveSrcUnit(srcUnit);
    }
}


void AbstractMappedHimmel::setTransitionDuration(const float duration)
{
    m_changer.setTransitionDuration(duration);
}


void AbstractMappedHimmel::pushTextureUnit(
    const GLint textureUnit
,   const float time)
{
    m_changer.pushUnit(textureUnit, time);
}

