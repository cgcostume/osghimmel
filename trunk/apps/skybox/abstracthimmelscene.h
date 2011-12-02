
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
#ifndef __ABSTRACTHIMMELSCENE_H__
#define __ABSTRACTHIMMELSCENE_H__

#include "utils/abstractpropertysupport.h"

#include <osg/Group>


namespace osg
{
    class Camera;
    class Viewport;
}


class AbstractHimmel;
class TimeF;

class AbstractHimmelScene 
:   public osg::Group
,   public AbstractPropertySupport
{
public:

    virtual ~AbstractHimmelScene();
    
    void assignTime(TimeF *timef);
    TimeF *timef();

    
    virtual const bool hasLocationSupport() const = 0;

    virtual const double setLatitude(const double latitude);
    virtual const double setLongitude(const double longitude);

protected:

    AbstractHimmelScene(osg::Camera *camera);

    virtual AbstractHimmel *himmel() = 0;

    // AbstractPropertySupport Interface

    virtual void propertyChanged(
        QtProperty *p
    ,   const QString &name);
    virtual void registerProperties();

protected:

    osg::Camera *m_camera;
    osg::Viewport *m_viewport;
};

#endif // __ABSTRACTHIMMELSCENE_H__