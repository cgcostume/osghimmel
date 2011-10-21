
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

#include "scene_polarmappedhimmel.h"

#include "include/polarmappedhimmel.h"

#include <osg/Texture2D>
#include <osgDB/ReadFile>


namespace
{
    // Properties

    static const QString GROUP_POLARMAPPED(TR("Polar Mapped"));

    static const QString PROPERTY_HBAND_SCALE    (TR("H-Band Scale"));
    static const QString PROPERTY_HBAND_THICKNESS(TR("H-Band Thickness"));
}

Scene_PolarMappedHimmel::Scene_PolarMappedHimmel(osg::Camera *camera)
:   AbstractHimmelScene(camera)
,   m_himmel(NULL)
{
    initializeProperties();

    m_himmel = new PolarMappedHimmel(PolarMappedHimmel::MM_HALF);

    m_himmel->setTransitionDuration(0.01f);

    m_himmel->getOrCreateTexture2D(0)->setImage(osgDB::readImageFile("resources/polar_half_pho_0.jpg"));
    m_himmel->getOrCreateTexture2D(1)->setImage(osgDB::readImageFile("resources/polar_half_pho_1.jpg"));
    m_himmel->getOrCreateTexture2D(2)->setImage(osgDB::readImageFile("resources/polar_half_pho_2.jpg"));
    m_himmel->getOrCreateTexture2D(3)->setImage(osgDB::readImageFile("resources/polar_half_pho_3.jpg"));
    m_himmel->getOrCreateTexture2D(4)->setImage(osgDB::readImageFile("resources/polar_half_pho_4.jpg"));
    m_himmel->getOrCreateTexture2D(5)->setImage(osgDB::readImageFile("resources/polar_half_pho_5.jpg"));
    m_himmel->getOrCreateTexture2D(6)->setImage(osgDB::readImageFile("resources/polar_half_pho_6.jpg"));
    m_himmel->getOrCreateTexture2D(7)->setImage(osgDB::readImageFile("resources/polar_half_pho_7.jpg"));
//    m_himmel->getOrCreateTexture2D(8)->setImage(osgDB::readImageFile("resources/polar_half_gen_8.jpg"));

    m_himmel->pushTextureUnit(0, 0.0f);
    m_himmel->pushTextureUnit(1, 0.1f);
    m_himmel->pushTextureUnit(2, 0.2f);
    m_himmel->pushTextureUnit(3, 0.3f);
    m_himmel->pushTextureUnit(4, 0.4f);
    m_himmel->pushTextureUnit(5, 0.5f);
    m_himmel->pushTextureUnit(6, 0.6f);
    m_himmel->pushTextureUnit(7, 0.7f);
//    m_himmel->pushTextureUnit(8, 0.8f);

    addChild(m_himmel);
}


Scene_PolarMappedHimmel::~Scene_PolarMappedHimmel()
{
}


AbstractHimmel *Scene_PolarMappedHimmel::himmel()
{
    return m_himmel;
}


void Scene_PolarMappedHimmel::registerProperties()
{
    AbstractHimmelScene::registerProperties();

    QtProperty *polarGroup = createGroup(GROUP_POLARMAPPED);

    createProperty(*polarGroup, PROPERTY_HBAND_SCALE, 0.1, 0.0, 1.0, 0.02);
    createProperty(*polarGroup, PROPERTY_HBAND_THICKNESS, 0.05, 0.0, 1.0, 0.01);
}


void Scene_PolarMappedHimmel::propertyChanged(
    QtProperty *p
,   const QString &name)
{
         if(PROPERTY_HBAND_SCALE == name)
        m_himmel->setHBandScale(doubleValue(PROPERTY_HBAND_SCALE));
    else if(PROPERTY_HBAND_THICKNESS == name)
        m_himmel->setHBandThickness(doubleValue(PROPERTY_HBAND_THICKNESS));
}
