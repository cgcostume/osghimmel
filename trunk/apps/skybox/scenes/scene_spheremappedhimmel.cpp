
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

#include "scene_spheremappedhimmel.h"

#include "include/spheremappedhimmel.h"
#include "include/timef.h"

#include <osg/Texture2D>

#include <osgDB/ReadFile>


Scene_SphereMappedHimmel::Scene_SphereMappedHimmel(
    osgViewer::View* view
,   const QSize &viewportSize)
:   AbstractHimmelScene(view, viewportSize)
,   m_timef(new TimeF(0.f, 60.f))
{
    osg::ref_ptr<SphereMappedHimmel> himmel = new SphereMappedHimmel();

    himmel->assignTime(m_timef, true);
    himmel->setTransitionDuration(0.2f);

    himmel->getOrCreateTexture2D(0)->setImage(osgDB::readImageFile("resources/sky_sphere_0.tga"));
    himmel->getOrCreateTexture2D(1)->setImage(osgDB::readImageFile("resources/sky_sphere_1.tga"));
    himmel->getOrCreateTexture2D(2)->setImage(osgDB::readImageFile("resources/sky_sphere_2.tga"));

    himmel->pushTextureUnit(0, 0.00f);
    himmel->pushTextureUnit(1, 0.33f);
    himmel->pushTextureUnit(2, 0.66f);

    addChild(himmel);
}


Scene_SphereMappedHimmel::~Scene_SphereMappedHimmel()
{
}