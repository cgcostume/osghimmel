
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

#pragma once
#ifndef __SHADERSOURCE_H__
#define __SHADERSOURCE_H__

#include <string>
#include <map>
#include <set>

namespace osg 
{
    class Shader; 
}

//! Link a shader with its source fileName and update on demand. Supports 
//  static update by fileName, so GUIs do not need to know shader objects.

class ShaderSource
{
public:

    ShaderSource(
        const std::string &fileName
    ,   osg::Shader *shader);

    ~ShaderSource();

    // Reloads the source from file.
    const bool update();

    // Returns the number of updated shader sources.
    static const int updateByFileName(const std::string &fileName);

    // Allows to obtain all registered shader source filenames.
    // For example, useful for file system watchers.
    static const std::set<std::string> getFileNames();


    // Valid if shader source is loaded and set to shader.
    inline const bool isValid() const
    {
        return m_valid;
    }

    inline const std::string getFileName() const
    {
        return m_fileName;
    }

    inline osg::Shader *getShader() const
    {
        return m_shader;
    }

protected:

    void registerShaderSource(ShaderSource *ss);
    void unregisterShaderSource(ShaderSource *ss);

    static const bool loadShaderSourceFromFile(
        const std::string &fileName
    ,   osg::Shader *shader);

protected:

    typedef std::set<ShaderSource*> t_shaderSources;
    typedef std::map<std::string, t_shaderSources> t_shaderSourcesByFileNames;

    // Used to allow updating shader sources by filenames.
    static t_shaderSourcesByFileNames s_shaderSourcesByFileNames;

    bool m_valid;

    const std::string m_fileName;
    osg::Shader *m_shader;
};

#endif __SHADERSOURCE_H__