
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
#ifndef __ABSTRACTHIMMEL_H__
#define __ABSTRACTHIMMEL_H__

#include <osg/Transform>
#include <osg/NodeCallback>


class TimeF;
class HimmelQuad;

#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
class ShaderModifier;
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER


class AbstractHimmel : public osg::Transform
{
private:
    // This callback calls update.
    class HimmelNodeCallback : public osg::NodeCallback 
    {
    public:
        virtual void operator()(
            osg::Node *node
        ,   osg::NodeVisitor *nv);
    };

public:

    AbstractHimmel();
    virtual ~AbstractHimmel();

    // The timef gets updated but takes no ownership.
    void assignTime(
        TimeF *timef
    ,   const bool autoUpdate = false);

    inline TimeF *getTime() const
    {
        return m_timef;
    }

    // Update timef on update.
    void setAutoUpdateTime(const bool autoUpdate);
    inline const bool getAutoUpdateTime() const
    {
        return m_autoUpdateTime;
    }

#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
    static inline ShaderModifier *shaderModifier() 
    {
        return &s_shaderModifier;
    }
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER

protected:

    void setupNode(osg::StateSet* stateSet);
    void setupProgram(osg::StateSet *stateSet);

    // Called by the HimmelUpdateCallback. Call this first when inherited!
    virtual void update();

    void initialize();
    virtual void postInitialize() { };

    void makeVertexShader();
    void makeFragmentShader();

    // abstract interface

    virtual const std::string getVertexShaderSource() = 0;
    virtual const std::string getFragmentShaderSource() = 0;

    // getter

    inline osg::Program &program()
    {
        return *m_program;
    }

    const float timef() const;

protected:

    bool m_initialized;

    // shader

    osg::Program *m_program;

    osg::Shader *m_vShader;
    osg::Shader *m_fShader;

    //

    TimeF *m_timef;
    bool m_autoUpdateTime;

    HimmelQuad *m_hquad;

#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
    static ShaderModifier s_shaderModifier;
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER
};

#endif // __ABSTRACTHIMMEL_H__