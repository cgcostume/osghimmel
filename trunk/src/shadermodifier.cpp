
// Copyright (c) 2011, Daniel M�ller <dm@g4t3.de>
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

#include "shadermodifier.h"


#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER

#include <assert.h>

#include <osg/Shader>
#include <osg/Notify>


ShaderModifier::ShaderModifier()
{
}


ShaderModifier::~ShaderModifier()
{
}


std::vector<ShaderModifier::t_identifier> ShaderModifier::getIdentifier() const
{
    std::vector<t_identifier> identifier;

    t_shaderSetsByIdentifier::const_iterator i(m_shaderSetsByIdentifier.begin());
    const t_shaderSetsByIdentifier::const_iterator iEnd(m_shaderSetsByIdentifier.end());

    for(; i != iEnd; ++i)
    {
        assert(!i->second.empty()); // shall never be empty, since identifier should be removed if no shader is attached
        identifier.push_back(i->first);
    }
    return identifier;
}


void ShaderModifier::registerShader(
    const t_identifier &identifier
,   osg::Shader *shader)
{
    if(!shader)
    {
        osg::notify(osg::WARN) << "An attempt was made and ignored, to register an invalid shader (NULL) for identifier \"" << identifier << "\"." << std::endl;
        return;
    }

    m_identifiersByShader[shader] = identifier;
    m_shaderSetsByIdentifier[identifier].insert(shader);

    t_sourcesByIdentifier::const_iterator i(m_sourcesByIdentifier.find(identifier));

    if(i == m_sourcesByIdentifier.end())
    {
        m_sourcesByIdentifier[identifier] = shader->getShaderSource();
    }
    else if(m_sourcesByIdentifier[identifier].compare(shader->getShaderSource()) != 0
        && m_modified.find(identifier) == m_modified.end())
    {   
        // Check if shader source equals the source related to this 
        // identifier, and update the shaders source if not and this 
        // identifier is not marked for update (this occurs, if multiple 
        // shader of the same identifier are available, and one of them 
        // got already modified).
        shader->setShaderSource(m_sourcesByIdentifier[identifier]);
    }
}


void ShaderModifier::unregisterShader(osg::Shader *shader)
{
    if(!shader)
    {
        osg::notify(osg::WARN) << "An attempt was made and ignored, to unregister an invalid shader (NULL)." << std::endl;
        return;
    }

    t_identifiersByShader::const_iterator f(m_identifiersByShader.find(shader));
    if(f == m_identifiersByShader.end())
    {
        osg::notify(osg::WARN) << "An attempt was made and discarded, to unregister an not registered shader." << std::endl;
        return;
    }

    const t_identifier &identifier(f->second);

    assert(m_shaderSetsByIdentifier.find(identifier) != m_shaderSetsByIdentifier.end());

    m_shaderSetsByIdentifier[identifier].erase(shader);
    if(m_shaderSetsByIdentifier[identifier].empty())
    {
        m_shaderSetsByIdentifier.erase(identifier);
        m_sourcesByIdentifier.erase(identifier);
        m_modified.erase(identifier);
    }

    m_identifiersByShader.erase(shader);
}


const std::string ShaderModifier::getSource(const ShaderModifier::t_identifier &identifier) const
{
    t_sourcesByIdentifier::const_iterator f(m_sourcesByIdentifier.find(identifier));

    if(f == m_sourcesByIdentifier.end())
    {
        osg::notify(osg::WARN) << "No source was registered for identifier \"" << identifier << "\"." << std::endl;
        return std::string();
    }

    return f->second;
}


void ShaderModifier::setSource(
    const ShaderModifier::t_identifier &identifier
,   const std::string &source
,   const bool update)
{
    t_sourcesByIdentifier::const_iterator f(m_sourcesByIdentifier.find(identifier));

    if(f == m_sourcesByIdentifier.end())
    {
        osg::notify(osg::WARN) << "No source was registered for identifier \"" << identifier << "\". Setting source was discarded." << std::endl;
        return;
    }
    assert(m_shaderSetsByIdentifier.find(identifier) != m_shaderSetsByIdentifier.end());

    m_sourcesByIdentifier[identifier] = source;

    if(update)
    {
        t_shaderSet::iterator i(m_shaderSetsByIdentifier[identifier].begin());
        const t_shaderSet::const_iterator iEnd(m_shaderSetsByIdentifier[identifier].end());

        for(; i != iEnd; ++i)
            (*i)->setShaderSource(source);
    }
    else
         m_modified.insert(identifier);
}


void ShaderModifier::update()
{
    t_identifierSet::const_iterator i(m_modified.begin());
    const t_identifierSet::const_iterator iEnd(m_modified.end());

    for(; i != iEnd; ++i)
    {
        assert(m_shaderSetsByIdentifier.find(*i) != m_shaderSetsByIdentifier.end());
        assert(m_sourcesByIdentifier.find(*i) != m_sourcesByIdentifier.end());

        const std::string &source(m_sourcesByIdentifier[*i]);

        t_shaderSet::iterator s(m_shaderSetsByIdentifier[*i].begin());
        const t_shaderSet::const_iterator sEnd(m_shaderSetsByIdentifier[*i].end());

        for(; s != sEnd; ++s)
            (*s)->setShaderSource(source);
    }
}

#endif OSGHIMMEL_ENABLE_SHADERMODIFIER