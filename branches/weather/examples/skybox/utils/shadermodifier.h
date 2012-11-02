
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

#pragma once
#ifndef __SHADERMODIFIER_H__
#define __SHADERMODIFIER_H__

#include <string>
#include <map>
#include <set>
#include <vector>

#include <osg/Shader>


// Allows manipulation of shader sources registered elsewhere. 
// Helps to decouple shader manipulation of specific shader instances and 
// GUIs via identifiers.

class ShaderModifier
{
public:
    typedef std::string t_identifier;
    typedef std::vector<t_identifier> t_identifiers;

public:
    ShaderModifier();
    virtual ~ShaderModifier();
   
    void unregisterIdentifiersChangedCallback(void *object);
    void registerIdentifiersChangedCallback(
        void *object
    ,   void(*callback)(void*));


    // Returns all registered identifier.
    const t_identifiers getIdentifiers() const;

    // Registers a shader by an identifier and replaces its source if 
    // already modified. Returns identifier that differs from given one.
    const t_identifier registerShader(
        t_identifier identifier
    ,   osg::Shader *shader);

    void unregisterShader(osg::Shader *shader);


    // manipulation

    const osg::Shader::Type getType(const t_identifier &identifier) const;

    const std::string getSource(const t_identifier &identifier) const;

    // All shaders sources related to identifier get replaced. Update is
    // optional and can be explcitily called later via update().
    void setSource(
        const t_identifier &identifier
        ,   const std::string &source
        ,   const bool update = true);

    // Updates all modified shader sources, that are not updated yet.
    void update();

protected:

    static const std::string makeIdentifier(
        const t_identifier &identifier
    ,   osg::Shader *shader);

    void identifiersChanged();

protected:

    typedef std::set<osg::Shader*> t_shaderSet;
    typedef std::set<t_identifier> t_identifierSet;

    typedef std::map<t_identifier, std::string> t_sourcesByIdentifier;
    typedef std::map<t_identifier, osg::Shader::Type> t_typesByIdentifier;
    typedef std::map<t_identifier, t_shaderSet> t_shaderSetsByIdentifier;
    typedef std::map<osg::Shader*, t_identifier> t_identifiersByShader;

    t_sourcesByIdentifier m_sourcesByIdentifier;

    t_shaderSetsByIdentifier m_shaderSetsByIdentifier;
    t_identifiersByShader m_identifiersByShader;

    // Identifiers of shaders whose source was modified and not updated yet.
    t_identifierSet m_modified;


    typedef std::map<void*, void(*)(void*)> t_callbacks;
    t_callbacks m_callbacks;
};

#endif // __SHADERMODIFIER_H__