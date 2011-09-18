
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

#include "shadersource.h"

#include <assert.h>

#include <osgDB/FileUtils>


ShaderSource::t_shaderSourcesByFileNames ShaderSource::s_shaderSourcesByFileNames;


ShaderSource::ShaderSource(
    const std::string &fileName
,   osg::Shader *shader)
:   m_valid(false)
,   m_shader(shader)
,   m_fileName(fileName)
{
    registerShaderSource(this);

    update();

    if(!isValid())
        return;

    assert(m_shader->getFileName().compare(m_fileName) == 0);
}


ShaderSource::~ShaderSource()
{
    unregisterShaderSource(this);
}


const bool ShaderSource::update()
{
    assert(m_shader);

    m_valid = loadShaderSourceFromFile(m_fileName, m_shader);
    return m_valid;
}


const int ShaderSource::updateByFileName(const std::string &fileName)
{
    const t_shaderSourcesByFileNames::const_iterator shaderSources(s_shaderSourcesByFileNames.find(fileName));

    if(shaderSources == s_shaderSourcesByFileNames.end())
        return 0;


    t_shaderSources::const_iterator i(shaderSources->second.begin());
    const t_shaderSources::const_iterator iEnd(shaderSources->second.end());

    int numSuccessfulUpdates(0); 

    for(; i != iEnd; ++i)
        if((*i)->update())
            ++numSuccessfulUpdates;


    return numSuccessfulUpdates;
}


void ShaderSource::registerShaderSource(ShaderSource *ss)
{
    assert(ss);
    s_shaderSourcesByFileNames[ss->getFileName()].insert(ss);
}


void ShaderSource::unregisterShaderSource(ShaderSource *ss)
{
    assert(ss);
    const std::string &fileName(ss->getFileName());

    const t_shaderSourcesByFileNames::iterator i(s_shaderSourcesByFileNames.find(fileName));

    if(i != s_shaderSourcesByFileNames.end())
    {
        t_shaderSources &shaderSources(i->second);
        shaderSources.erase(ss);

        if(shaderSources.empty())
            s_shaderSourcesByFileNames.erase(fileName);
    }
}


// modification of http://www.openscenegraph.org/projects/osg/wiki/Support/Tutorials/ShadersIntroduction

const bool ShaderSource::loadShaderSourceFromFile(
    const std::string &fileName
,   osg::Shader *shader)
{
    if(!shader)
    {
        osg::notify(osg::WARN) << "No valid Shader object available for source \"" << fileName << "\"." << std::endl;
        return false;
    }

    const std::string fqFileName(osgDB::findDataFile(fileName));

    if(fqFileName.empty())
    {
        osg::notify(osg::WARN) << "\"" << fileName << "\" not found." << std::endl;
        return false;
    }

    if(!shader->loadShaderSourceFromFile(fqFileName.c_str()))
    {
        osg::notify(osg::WARN) << "Could not load source from \"" << fileName << "\"." << std::endl;
        return false;
    }
    return true;
}