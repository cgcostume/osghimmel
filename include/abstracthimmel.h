
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
#ifndef __ABSTRACTHIMMEL_H__
#define __ABSTRACTHIMMEL_H__

#include <osg/Transform>
#include <osg/NodeCallback>

#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
#include "shadermodifier.h"
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER


class TimeF;


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

    AbstractHimmel(); // Inheriting constructors should add m_hQuad as child as desired.
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

    // From osg::Transform:

    // Get the transformation matrix which moves from local coords to world coords.
    virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const;

    // Get the transformation matrix which moves from world coords to local coords.
    virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const;


    void dirty(const bool enable = true);
    virtual const bool isDirty() const;

    // TODO: seems ugly - optimize this!

    void setCameraHint(osg::Camera *camera);
    const float getCameraFovHint() const;

    void setViewSizeHint(unsigned int width, unsigned int height);
    const unsigned int getViewSizeWidthHint() const;
    const unsigned int getViewSizeHeightHint() const;


#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
    static void setupShaderModifier(ShaderModifier *shaderModifier);
    static ShaderModifier *shaderModifier();
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER

protected:

    void setupNode(osg::StateSet* stateSet);

    // Called by the HimmelUpdateCallback. Call this first when inherited!
    virtual void update();

    void initialize();
    virtual void postInitialize() { };


    const float timef() const;

protected:

    bool m_initialized;
    bool m_dirty;

    //

    TimeF *m_timef;
    bool m_autoUpdateTime;

    // This is used for "is dirty decisions" in update.
    // m_timef can be updated from outsite, so we need check for changes manually.
    long double m_lastElapsed;

    // TODO  temp
    osg::Camera *m_cameraHint;
    unsigned int m_widthHint;
    unsigned int m_heightHint;


#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
private:
    static ShaderModifier *m_shaderModifier;
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER

};

#endif // __ABSTRACTHIMMEL_H__