
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

#include "scene_cubemappedhimmel.h"

#include "include/cubemappedhimmel.h"

#include <osg/TextureCubeMap>

#include <osgDB/ReadFile>


namespace
{
    // Properties

    static const QString GROUP_CUBEMAPPED(TR("Cube Mapped"));

    static const QString PROPERTY_TEST1  (TR("Test1"));
    static const QString PROPERTY_TEST2  (TR("Test2"));
}

Scene_CubeMappedHimmel::Scene_CubeMappedHimmel(osg::Camera *camera)
:   AbstractHimmelScene(camera)
,   m_himmel(NULL)
{
    initializeProperties();

    m_himmel = new CubeMappedHimmel();

    m_himmel->setTransitionDuration(0.2f);

    osg::TextureCubeMap *tcm0 = m_himmel->getOrCreateTextureCubeMap(0);
    //osg::TextureCubeMap *tcm1 = m_himmel->getOrCreateTextureCubeMap(1);
    //osg::TextureCubeMap *tcm2 = m_himmel->getOrCreateTextureCubeMap(2);

    // px = lf; nx = rt; py = dn; ny = up; pz = ft; nz = bk    <- common skybox mapping (lhs to rhs)

    /*
    tcm0->setImage(osg::TextureCubeMap::POSITIVE_X, osgDB::readImageFile("resources/sky_box_0_px.tga"));
    tcm0->setImage(osg::TextureCubeMap::NEGATIVE_X, osgDB::readImageFile("resources/sky_box_0_nx.tga"));
    tcm0->setImage(osg::TextureCubeMap::POSITIVE_Y, osgDB::readImageFile("resources/sky_box_0_py.tga"));
    tcm0->setImage(osg::TextureCubeMap::NEGATIVE_Y, osgDB::readImageFile("resources/sky_box_0_ny.tga"));
    tcm0->setImage(osg::TextureCubeMap::POSITIVE_Z, osgDB::readImageFile("resources/sky_box_0_pz.tga"));
    tcm0->setImage(osg::TextureCubeMap::NEGATIVE_Z, osgDB::readImageFile("resources/sky_box_0_nz.tga"));

    tcm1->setImage(osg::TextureCubeMap::POSITIVE_X, osgDB::readImageFile("resources/sky_box_1_px.tga"));
    tcm1->setImage(osg::TextureCubeMap::NEGATIVE_X, osgDB::readImageFile("resources/sky_box_1_nx.tga"));
    tcm1->setImage(osg::TextureCubeMap::POSITIVE_Y, osgDB::readImageFile("resources/sky_box_1_py.tga"));
    tcm1->setImage(osg::TextureCubeMap::NEGATIVE_Y, osgDB::readImageFile("resources/sky_box_1_ny.tga"));
    tcm1->setImage(osg::TextureCubeMap::POSITIVE_Z, osgDB::readImageFile("resources/sky_box_1_pz.tga"));
    tcm1->setImage(osg::TextureCubeMap::NEGATIVE_Z, osgDB::readImageFile("resources/sky_box_1_nz.tga"));

    tcm2->setImage(osg::TextureCubeMap::POSITIVE_X, osgDB::readImageFile("resources/sky_box_2_px.tga"));
    tcm2->setImage(osg::TextureCubeMap::NEGATIVE_X, osgDB::readImageFile("resources/sky_box_2_nx.tga"));
    tcm2->setImage(osg::TextureCubeMap::POSITIVE_Y, osgDB::readImageFile("resources/sky_box_2_py.tga"));
    tcm2->setImage(osg::TextureCubeMap::NEGATIVE_Y, osgDB::readImageFile("resources/sky_box_2_ny.tga"));
    tcm2->setImage(osg::TextureCubeMap::POSITIVE_Z, osgDB::readImageFile("resources/sky_box_2_pz.tga"));
    tcm2->setImage(osg::TextureCubeMap::NEGATIVE_Z, osgDB::readImageFile("resources/sky_box_2_nz.tga"));
    */

    osg::Image *img0 = osgDB::readImageFile("resources/noise8k.png");

    tcm0->setImage(osg::TextureCubeMap::POSITIVE_X, img0);
    tcm0->setImage(osg::TextureCubeMap::NEGATIVE_X, img0);
    tcm0->setImage(osg::TextureCubeMap::POSITIVE_Y, img0);
    tcm0->setImage(osg::TextureCubeMap::NEGATIVE_Y, img0);
    tcm0->setImage(osg::TextureCubeMap::POSITIVE_Z, img0);
    tcm0->setImage(osg::TextureCubeMap::NEGATIVE_Z, img0);

    m_himmel->pushTextureUnit(0, 0.00f);

    addChild(m_himmel);
}


Scene_CubeMappedHimmel::~Scene_CubeMappedHimmel()
{
}


AbstractHimmel *Scene_CubeMappedHimmel::himmel()
{
    return m_himmel;
}


void Scene_CubeMappedHimmel::registerProperties()
{
    AbstractHimmelScene::registerProperties();

    QtProperty *cubeGroup = createGroup(GROUP_CUBEMAPPED);
}
