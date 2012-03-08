
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

#include "scene_paraboloidmappedhimmel.h"

#include "include/paraboloidmappedhimmel.h"
#include "include/horizonband.h"

#include "utils/qt2osg.h"

#include <osg/Texture2D>

#include <osgDB/ReadFile>


namespace
{
    // Properties

    const QString GROUP_PARABOLOIDMAPPED(TR("Paraboloid Mapped"));

    const QString PROPERTY_RAZSPEED(TR("RAZ Speed"));

    const QString GROUP_HBAND(TR("HorizonBand"));

    const QString PROPERTY_HBAND_SCALE (TR("Scale"));
    const QString PROPERTY_HBAND_WIDTH (TR("Width"));
    const QString PROPERTY_HBAND_OFFSET(TR("Offset"));
    const QString PROPERTY_HBAND_COLOR (TR("Color"));
    const QString PROPERTY_BOTTOM_COLOR(TR("Bottom Color"));
}

Scene_ParaboloidMappedHimmel::Scene_ParaboloidMappedHimmel(osg::Camera *camera)
:   AbstractHimmelScene(camera)
,   m_himmel(NULL)
{
    initializeProperties();

    m_himmel = new ParaboloidMappedHimmel(true);

    m_himmel->setTransitionDuration(0.05f);

    m_himmel->getOrCreateTexture2D(0)->setImage(osgDB::readImageFile("resources/paraboloid_gen_0.jpg"));
    m_himmel->getOrCreateTexture2D(1)->setImage(osgDB::readImageFile("resources/paraboloid_gen_1.jpg"));
    m_himmel->getOrCreateTexture2D(2)->setImage(osgDB::readImageFile("resources/paraboloid_gen_2.jpg"));
    m_himmel->getOrCreateTexture2D(3)->setImage(osgDB::readImageFile("resources/paraboloid_pho_0.jpg"));
    m_himmel->getOrCreateTexture2D(4)->setImage(osgDB::readImageFile("resources/paraboloid_pho_1.jpg"));

    m_himmel->pushTextureUnit(0, 0.0f);
    m_himmel->pushTextureUnit(1, 0.2f);
    m_himmel->pushTextureUnit(2, 0.4f);
    m_himmel->pushTextureUnit(3, 0.6f);
    m_himmel->pushTextureUnit(4, 0.8f);

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

    createProperty(*paraboloidGroup, PROPERTY_RAZSPEED, 0.0, -99999.0, 99999.0, 10.0); 

    QtProperty *hbandGroup = createGroup(*paraboloidGroup, GROUP_HBAND);

    createProperty(*hbandGroup, PROPERTY_HBAND_SCALE, HorizonBand::defaultScale(), 0.0, 1.0, 0.02); 
    createProperty(*hbandGroup, PROPERTY_HBAND_WIDTH, HorizonBand::defaultWidth(), 0.0, 1.0, 0.01);
    createProperty(*hbandGroup, PROPERTY_HBAND_OFFSET, HorizonBand::defaultOffset(), -1.0, 1.0, 0.01);
    createProperty(*hbandGroup, PROPERTY_HBAND_COLOR,  toQColor(HorizonBand::defaultColor()));
    createProperty(*hbandGroup, PROPERTY_BOTTOM_COLOR, toQColor(HorizonBand::defaultBottomColor()));
}


void Scene_ParaboloidMappedHimmel::propertyChanged(
    QtProperty *p
,   const QString &name)
{
        if(PROPERTY_RAZSPEED == name)
    {
        const double secondsPerRAZ(doubleValue(PROPERTY_RAZSPEED));

        m_himmel->setSecondsPerRAZ(secondsPerRAZ);
        m_himmel->setRazDirection(secondsPerRAZ < 0 ? 
            AbstractMappedHimmel::RD_NorthEastSouthWest : AbstractMappedHimmel::RD_NorthWestSouthEast);
    }

    else if(PROPERTY_HBAND_SCALE == name)
        m_himmel->hBand()->setScale(doubleValue(PROPERTY_HBAND_SCALE));
    else if(PROPERTY_HBAND_WIDTH == name)
        m_himmel->hBand()->setWidth(doubleValue(PROPERTY_HBAND_WIDTH));
    else if(PROPERTY_HBAND_OFFSET == name)
        m_himmel->hBand()->setOffset(doubleValue(PROPERTY_HBAND_OFFSET));
    else if(PROPERTY_HBAND_COLOR == name)
        m_himmel->hBand()->setColor(toVec4(colorValue(PROPERTY_HBAND_COLOR)));
    else if(PROPERTY_BOTTOM_COLOR == name)
        m_himmel->hBand()->setBottomColor(toVec4(colorValue(PROPERTY_BOTTOM_COLOR)));
}
