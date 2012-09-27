
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

#include "himmeloverlay.h"

#include "atime.h"
#include "julianday.h"
#include "timef.h"
#include "abstracthimmel.h"
#include "himmel.h"
#include "astronomy.h"

#include <osg/Vec3>

#include <assert.h>
#include <cstdio>

namespace osgHimmel
{

HimmelOverlay::HimmelOverlay(
    const std::string &font
,   const int width
,   const int height)
:   osg::Projection()

,   m_transform(new osg::MatrixTransform)

,   m_geode(new osg::Geode)
    
,   m_text_time(new osgText::Text)
,   m_text_geo(new osgText::Text)

,   m_himmel(NULL)
{
    m_transform->setMatrix(osg::Matrix::identity());
    m_transform->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

    setSize(width, height);

    addChild(m_transform);
    m_transform->addChild(m_geode);

    m_geode->addDrawable(m_text_time);
    m_geode->addDrawable(m_text_geo);

    m_text_time->setCharacterSize(44);
    m_text_time->setFont(font);
    m_text_time->setAxisAlignment(osgText::Text::SCREEN);
    m_text_time->setPosition(osg::Vec3(64, 64, 0));

    m_text_geo->setCharacterSize(22);
    m_text_geo->setFont(font);
    m_text_geo->setAxisAlignment(osgText::Text::SCREEN);
    m_text_geo->setPosition(osg::Vec3(64, 32, 0));
}


HimmelOverlay::~HimmelOverlay()
{
}


void HimmelOverlay::hide()
{
    setNodeMask(0x0);
}


void HimmelOverlay::show()
{
    setNodeMask(0xffffffff);
}


void HimmelOverlay::setSize(
    const int width
,   const int height)
{
    setMatrix(osg::Matrix::ortho2D(0, width, 0, height));
}


void HimmelOverlay::update()
{
    assert(m_himmel);

    AbstractAstronomy *astro(m_himmel->astro());
    const t_aTime atime = t_aTime::fromTimeF(*m_himmel->getTime());

    const osg::Vec4 c = astro->getSunPosition(false).z() < 0.35 ? osg::Vec4(246, 246, 246, 0.33) : osg::Vec4(8, 8, 8, 0.33);

    m_text_time->setColor(c);
    m_text_geo->setColor(c);



    char deb1[64];
    sprintf(deb1, "%04i-%02i-%02i %02i:%02i UTC\0", atime.year, atime.month, atime.day, atime.hour, atime.minute);

    char deb2[64];
    sprintf(deb2, "lat %03.2f°; lon %03.2f°; alt %01.3fkm\0"
        , astro->getLatitude(), astro->getLongitude(), m_himmel->getAltitude());

    m_text_time->setText(deb1);
    m_text_geo->setText(deb2);
}


void HimmelOverlay::assignHimmel(Himmel *himmel)
{
    m_himmel = himmel;
}

} // namespace osgHimmel