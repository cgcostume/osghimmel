
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

#include "scene_paraboloidmappedhimmel.h"

#include "include/paraboloidmappedhimmel.h"

#include <osg/Texture2D>

#include <osgDB/ReadFile>


namespace
{
    // Properties

    static const QString GROUP_PARABOLOIDMAPPED(TR("Paraboloid Mapped"));
}

Scene_ParaboloidMappedHimmel::Scene_ParaboloidMappedHimmel(osg::Camera *camera)
:   AbstractHimmelScene(camera)
,   m_himmel(NULL)
{
    initializeProperties();

    m_himmel = new ParaboloidMappedHimmel();

    m_himmel->setTransitionDuration(0.1f);

    m_himmel->getOrCreateTexture2D(0)->setImage(osgDB::readImageFile("resources/paraboloid_pho_0.jpg"));
    m_himmel->getOrCreateTexture2D(1)->setImage(osgDB::readImageFile("resources/paraboloid_pho_1.jpg"));
    m_himmel->getOrCreateTexture2D(2)->setImage(osgDB::readImageFile("resources/paraboloid_pho_2.jpg"));
    m_himmel->getOrCreateTexture2D(3)->setImage(osgDB::readImageFile("resources/paraboloid_pho_3.jpg"));

    m_himmel->pushTextureUnit(0, 0.00f);
    m_himmel->pushTextureUnit(1, 0.25f);
    m_himmel->pushTextureUnit(2, 0.50f);
    m_himmel->pushTextureUnit(3, 0.75f);

    addChild(m_himmel);
}


Scene_ParaboloidMappedHimmel::~Scene_ParaboloidMappedHimmel()
{
}


AbstractHimmel *Scene_ParaboloidMappedHimmel::himmel()
{
    return m_himmel;
}


void Scene_ParaboloidMappedHimmel::registerProperties()
{
    AbstractHimmelScene::registerProperties();

    QtProperty *paraboloidGroup = createGroup(GROUP_PARABOLOIDMAPPED);
}
