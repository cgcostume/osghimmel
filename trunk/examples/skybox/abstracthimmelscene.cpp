
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

#include "abstracthimmelscene.h"

#include "utils/tr.h"
#include "utils/shadermodifier.h"

#include "osgHimmel/abstracthimmel.h"
#include "osgHimmel/timef.h"

#include <osg/Camera>
#include <osgViewer/View>

#include <assert.h>


namespace
{
    // Properties

    const QString GROUP_ABSTRACTHIMMEL(TR("Abstract Himmel"));
}

using namespace osgHimmel;

AbstractHimmelScene::AbstractHimmelScene(osg::Camera *camera)
:   osg::Group()
,   AbstractPropertySupport()

,   m_camera(camera)
,   m_viewport(NULL)

,   m_shaderModifier(new ShaderModifier)
{
    assert(camera);
    m_viewport = m_camera->getViewport();
}


AbstractHimmelScene::~AbstractHimmelScene()
{
    delete m_shaderModifier;
}


void AbstractHimmelScene::initialize()
{
    assert(himmel());
    himmel()->initialize();

    postInitialize();
}


ShaderModifier *AbstractHimmelScene::shaderModifier()
{
    return m_shaderModifier;
}


void AbstractHimmelScene::propertyChanged(
    QtProperty *p
,   const QString &name)
{
}


void AbstractHimmelScene::registerProperties()
{
    QtProperty *himmelGroup = createGroup(GROUP_ABSTRACTHIMMEL);
}


void AbstractHimmelScene::assignTime(TimeF *timef)
{
    if(himmel())
        himmel()->assignTime(timef, false);
}

TimeF *AbstractHimmelScene::timef()
{
    return himmel()->getTime();
}


const double AbstractHimmelScene::setLatitude(const double latitude)
{
    return 0.0;
}


const double AbstractHimmelScene::setLongitude(const double longitude)
{
    return 0.0;
}


const double AbstractHimmelScene::setAltitude(const double altitude)
{
    return 0.0;
}



// TODO: change this - seems ugly. Works as workaround for now...

void AbstractHimmelScene::hintCamera(osg::Camera *camera)
{
    himmel()->setCameraHint(camera);
}

void AbstractHimmelScene::hintViewSize(unsigned int width, unsigned int height)
{
    himmel()->setViewSizeHint(width, height);
}
