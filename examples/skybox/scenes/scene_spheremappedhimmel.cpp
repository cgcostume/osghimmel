
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

#include "scene_spheremappedhimmel.h"

#include "utils/tr.h"
#include "utils/shadermodifier.h"

#include "osgHimmel/spheremappedhimmel.h"

#include <osg/Texture2D>
#include <osgDB/ReadFile>


namespace
{
    // Properties

    const QString GROUP_SPHEREMAPPED(TR("Sphere Mapped"));

    const QString PROPERTY_RAZSPEED(TR("RAZ Speed"));
}

using namespace osgHimmel;


Scene_SphereMappedHimmel::Scene_SphereMappedHimmel(osg::Camera *camera)
:   AbstractHimmelScene(camera)
,   m_himmel(NULL)
{
    initializeProperties();

    m_himmel = new SphereMappedHimmel(SphereMappedHimmel::MM_TowardsNegY);

    m_himmel->setTransitionDuration(0.05f);

    m_himmel->getOrCreateTexture2D(0)->setImage(osgDB::readImageFile("resources/sphere_gen_0.jpg"));
    m_himmel->getOrCreateTexture2D(1)->setImage(osgDB::readImageFile("resources/sphere_gen_2.jpg"));

    m_himmel->pushTextureUnit(0, 0.0f);
    m_himmel->pushTextureUnit(1, 0.5f);

    addChild(m_himmel);
}


Scene_SphereMappedHimmel::~Scene_SphereMappedHimmel()
{
}


AbstractHimmel *Scene_SphereMappedHimmel::himmel()
{
    return m_himmel;
}


void Scene_SphereMappedHimmel::postInitialize()
{
    shaderModifier()->registerShader(m_himmel->getName(), m_himmel->getVertexShader());
    shaderModifier()->registerShader(m_himmel->getName(), m_himmel->getGeometryShader());
    shaderModifier()->registerShader(m_himmel->getName(), m_himmel->getFragmentShader());
}


void Scene_SphereMappedHimmel::registerProperties()
{
    AbstractHimmelScene::registerProperties();

    QtProperty *sphereGroup = createGroup(GROUP_SPHEREMAPPED);

    createProperty(*sphereGroup, PROPERTY_RAZSPEED, 0.0, -99999.0, 99999.0, 10.0); 
}


void Scene_SphereMappedHimmel::propertyChanged(
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
}
