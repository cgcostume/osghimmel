
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
#ifndef __ABSTRACTMAPPEDHIMMEL_H__
#define __ABSTRACTMAPPEDHIMMEL_H__

#include "abstracthimmel.h"
#include "twounitschanger.h"

class TimeF;

namespace osg
{
    class MatrixTransform;
}


class AbstractMappedHimmel : public AbstractHimmel
{
public:

    enum RAZDirection
    {
        RAZD_NorthWestSouthEast
    ,   RAZD_NorthEastSouthWest
    };

public:

    AbstractMappedHimmel();
    virtual ~AbstractMappedHimmel();

    // This call gets redirected to a TwoUnitsChanger instance (see comment there).
    void setTransitionDuration(const float duration);
    const float getTransitionDuration() const;

    // This call gets redirected to a TwoUnitsChanger instance  (see comment there).
    void pushTextureUnit(
        const GLint textureUnit
    ,   const float time = 1.f);

    // RAZ = time dependent Rotation around Zenith - this is independent from himmels' timef.
    void setSecondsPerRAZ(const float secondsPerRAZ); // reasonable values should be around 1000+
    const float getSecondsPerRAZ() const;

    void setRAZDirection(const RAZDirection razDirection);
    const RAZDirection getRAZDirection() const;

protected:

    // Inherited by AbstractHimmel. Call this first when inherited!
    virtual void update();

    void assignUnit(
        const GLint textureUnit
    ,   const GLint targetIndex);

    // For convenience in subclasses.
    inline void assignBackUnit(const GLint textureUnit);
    inline void assignSrcUnit(const GLint textureUnit);

    const float updateSrcAlpha() const; // Accesses TwoUnitsChanger.

    // Interface

    virtual osg::StateAttribute *getTextureAttribute(const GLint textureUnit) const = 0;


    // AbstractHimmel interface

    virtual const std::string getVertexShaderSource();

protected:

    TwoUnitsChanger m_changer;

    osg::ref_ptr<osg::Uniform> u_back; // type depends on subclasses
    osg::ref_ptr<osg::Uniform> u_src;  // type depends on subclasses

    osg::ref_ptr<osg::Uniform> u_srcAlpha; // float

    GLint m_activeBackUnit;
    GLint m_activeSrcUnit;

    osg::ref_ptr<osg::MatrixTransform> m_razTransform;
    RAZDirection m_razDirection;

    TimeF *m_razTimef;    
};

#endif // __ABSTRACTMAPPEDHIMMEL_H__