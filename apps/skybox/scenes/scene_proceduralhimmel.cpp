
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

#include "scene_proceduralhimmel.h"

#include "include/proceduralhimmel.h"


namespace
{
    // Properties

    static const QString GROUP_PROCEDURAL(TR("Procedrual"));
}

Scene_ProceduralHimmel::Scene_ProceduralHimmel(osg::Camera *camera)
:   AbstractHimmelScene(camera)
,   m_himmel(NULL)
{
    initializeProperties();

    m_himmel = new ProceduralHimmel();
    addChild(m_himmel);
}


Scene_ProceduralHimmel::~Scene_ProceduralHimmel()
{
}


AbstractHimmel *Scene_ProceduralHimmel::himmel()
{
    return m_himmel;
}


void Scene_ProceduralHimmel::registerProperties()
{
    AbstractHimmelScene::registerProperties();

    QtProperty *proceduralGroup = createGroup(GROUP_PROCEDURAL);
}


void Scene_ProceduralHimmel::propertyChanged(
    QtProperty *p
,   const QString &name)
{
}


const double Scene_ProceduralHimmel::setLatitude(const double latitude)
{
    return m_himmel->setLatitude(latitude);
}


const double Scene_ProceduralHimmel::setLongitude(const double longitude)
{
    return m_himmel->setLongitude(longitude);
}