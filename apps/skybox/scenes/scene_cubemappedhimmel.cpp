
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

#include "scene_cubemappedhimmel.h"

#include "include/cubemappedhimmel.h"

#include <osg/TextureCubeMap>

#include <osgDB/ReadFile>


namespace
{
    // Properties

    static const QString GROUP_CUBEMAPPED (TR("Cube Mapped"));

    static const QString PROPERTY_RAZSPEED(TR("RAZ Speed"));
}

Scene_CubeMappedHimmel::Scene_CubeMappedHimmel(osg::Camera *camera)
:   AbstractHimmelScene(camera)
,   m_himmel(NULL)
{
    initializeProperties();

    m_himmel = new CubeMappedHimmel();

    m_himmel->setTransitionDuration(0.05f);

    osg::TextureCubeMap *tcm;

    tcm = m_himmel->getOrCreateTextureCubeMap(0);

    tcm->setImage(osg::TextureCubeMap::POSITIVE_X, osgDB::readImageFile("resources/moon_px.png"));
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_X, osgDB::readImageFile("resources/moon_nx.png"));
    tcm->setImage(osg::TextureCubeMap::POSITIVE_Y, osgDB::readImageFile("resources/moon_py.png"));
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_Y, osgDB::readImageFile("resources/moon_ny.png"));
    tcm->setImage(osg::TextureCubeMap::POSITIVE_Z, osgDB::readImageFile("resources/moon_pz.png"));
    tcm->setImage(osg::TextureCubeMap::NEGATIVE_Z, osgDB::readImageFile("resources/moon_nz.png"));

    m_himmel->pushTextureUnit(0);

    /*
    std::string name[] = { "4", "6", "9", "17", "19" };
    osg::TextureCubeMap *tcm[5];

    const int n = 5;
    for(int i = 0; i < n; ++i)
    {
        tcm[i] = m_himmel->getOrCreateTextureCubeMap(i);

        tcm[i]->setImage(osg::TextureCubeMap::POSITIVE_X, osgDB::readImageFile("resources/cube_gen_" + name[i] + "_px.jpg"));
        tcm[i]->setImage(osg::TextureCubeMap::NEGATIVE_X, osgDB::readImageFile("resources/cube_gen_" + name[i] + "_nx.jpg"));
        tcm[i]->setImage(osg::TextureCubeMap::POSITIVE_Y, osgDB::readImageFile("resources/cube_gen_" + name[i] + "_py.jpg"));
        tcm[i]->setImage(osg::TextureCubeMap::NEGATIVE_Y, osgDB::readImageFile("resources/cube_gen_" + name[i] + "_ny.jpg"));
        tcm[i]->setImage(osg::TextureCubeMap::POSITIVE_Z, osgDB::readImageFile("resources/cube_gen_" + name[i] + "_pz.jpg"));
        tcm[i]->setImage(osg::TextureCubeMap::NEGATIVE_Z, osgDB::readImageFile("resources/cube_gen_" + name[i] + "_nz.jpg"));

      m_himmel->pushTextureUnit(i, (i * 1.f) / (n * 1.f));
    }
    */
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

    createProperty(*cubeGroup, PROPERTY_RAZSPEED, 0.0, -99999.0, 99999.0, 10.0); 
}

void Scene_CubeMappedHimmel::propertyChanged(
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
