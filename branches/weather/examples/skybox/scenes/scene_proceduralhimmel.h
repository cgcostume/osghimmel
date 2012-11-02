
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
#ifndef __SCENE_PROCEDURALHIMMEL_H__
#define __SCENE_PROCEDURALHIMMEL_H__

#include "abstracthimmelscene.h"

namespace osgHimmel
{
    class Himmel;
}


class CameraLock : public osg::NodeCallback 
{
public:

    enum e_Target
    {
        T_None
    ,   T_Moon
    ,   T_Sun
    };

public:

    CameraLock(osgHimmel::Himmel *himmel, osg::Camera *camera);

    virtual void operator()(
        osg::Node *node
    ,   osg::NodeVisitor *nv);

    void setTarget(const e_Target target);

protected:
    osgHimmel::Himmel* m_himmel;
    osg::Camera *m_camera;

    e_Target m_target;
};


class Scene_ProceduralHimmel : public AbstractHimmelScene
{
public:
    Scene_ProceduralHimmel(
        osg::Camera *camera
    ,   const bool withClouds);

    virtual ~Scene_ProceduralHimmel();

    virtual osgHimmel::AbstractHimmel *himmel();

    virtual const bool hasLocationSupport() const 
    {
        return true;
    }

    virtual const double setLatitude(const double latitude);
    virtual const double setLongitude(const double longitude);
    virtual const double setAltitude(const double altitude);

    void setCameraLockTarget(const CameraLock::e_Target target);

protected:

    // from AbstractPropertySupport
    virtual void registerProperties();
    virtual void propertyChanged(
        QtProperty *p
    ,   const QString &name);

    virtual void postInitialize();

protected:
    osg::ref_ptr<osgHimmel::Himmel> m_himmel;

    CameraLock *m_cameraLock;
};


#endif // __SCENE_PROCEDURALHIMMEL_H__