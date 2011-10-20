
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

#include "include/polarmappedhimmel.h"
#include "include/cubemappedhimmel.h"
#include "include/timef.h"

#include <osgViewer/Viewer>

#include <osg/TextureCubeMap>
#include <osg/Texture2D>
#include <osg/PolygonMode>

#include <osgDB/ReadFile>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/TerrainManipulator>

#include <iostream>


enum e_Demo
{
    D_PolarMappedHimmel = 0
,   D_CubeMappedHimmel  = 1
};


osg::ref_ptr<osg::Group> g_scene = NULL;

TimeF *g_timef(new TimeF(0.f, 60.f));

e_Demo g_demo;
std::map<e_Demo, osg::ref_ptr<AbstractHimmel> > g_himmelsByDemo;


void activateDemo(e_Demo demo)
{
    g_himmelsByDemo[D_PolarMappedHimmel]->setNodeMask(D_PolarMappedHimmel == demo);
    g_himmelsByDemo[D_CubeMappedHimmel] ->setNodeMask(D_CubeMappedHimmel  == demo);
}


class KeyboardEventHandler : public osgGA::GUIEventHandler
{
public:
    KeyboardEventHandler()
    {
    }

    virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &)
    {
        switch(ea.getEventType())
        {
        case(osgGA::GUIEventAdapter::KEYDOWN):
            {
                if (ea.getKey()==osgGA::GUIEventAdapter::KEY_Space)
                {
                    g_demo = static_cast<e_Demo>((g_demo + 1) % 2);
                    activateDemo(g_demo);
                }
            }
            break;

        default:
            break;
        };
        return false;
    }
};


osg::ref_ptr<AbstractHimmel> createPolarMappedDemo()
{
    osg::ref_ptr<PolarMappedHimmel> himmel = new PolarMappedHimmel();

    himmel->assignTime(g_timef, true);
    himmel->setTransitionDuration(0.2f);

    himmel->getOrCreateTexture2D(0)->setImage(osgDB::readImageFile("resources/sky_sphere_0.tga"));
    himmel->getOrCreateTexture2D(1)->setImage(osgDB::readImageFile("resources/sky_sphere_1.tga"));
    himmel->getOrCreateTexture2D(2)->setImage(osgDB::readImageFile("resources/sky_sphere_2.tga"));

    himmel->pushTextureUnit(0, 0.00f);
    himmel->pushTextureUnit(1, 0.33f);
    himmel->pushTextureUnit(2, 0.66f);

    return himmel;
}


osg::ref_ptr<AbstractHimmel> createCubeMappedDemo()
{
    osg::ref_ptr<CubeMappedHimmel> himmel = new CubeMappedHimmel();

    himmel->assignTime(g_timef, true);
    himmel->setTransitionDuration(0.2f);

    osg::TextureCubeMap *tcm0 = himmel->getOrCreateTextureCubeMap(0);
    osg::TextureCubeMap *tcm1 = himmel->getOrCreateTextureCubeMap(1);
    osg::TextureCubeMap *tcm2 = himmel->getOrCreateTextureCubeMap(2);

    // px = lf; nx = rt; py = dn; ny = up; pz = ft; nz = bk    <- common skybox mapping (lhs to rhs)

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

    himmel->pushTextureUnit(0, 0.00f);
    himmel->pushTextureUnit(1, 0.33f);
    himmel->pushTextureUnit(2, 0.66f);

    return himmel;
}


void initializeScene(osgViewer::View &view)
{
    osg::Camera* cam = view.getCamera();
    cam->setProjectionMatrixAsPerspective(45.0f, 4.f / 3.f, 0.1f, 8.0f);

    osg::ref_ptr<osg::Group> root = new osg::Group;

    g_scene = new osg::Group;
    root->addChild(g_scene);

    g_himmelsByDemo[D_PolarMappedHimmel] = createPolarMappedDemo();
    root->addChild(g_himmelsByDemo[D_PolarMappedHimmel]);

    g_himmelsByDemo[D_CubeMappedHimmel] = createCubeMappedDemo();
    root->addChild(g_himmelsByDemo[D_CubeMappedHimmel]);

    view.setSceneData(root.get());

//    osg::ref_ptr<osg::Node> loadedScene = osgDB::readNodeFile("resources/knot.obj");
//    g_scene->addChild(loadedScene.get());
}


void initializeManipulators(osgViewer::View &view)
{
    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

    keyswitchManipulator->addMatrixManipulator('1', "Terrain",   new osgGA::TerrainManipulator());
    keyswitchManipulator->addMatrixManipulator('2', "Trackball", new osgGA::TrackballManipulator());
    keyswitchManipulator->addMatrixManipulator('3', "Flight",    new osgGA::FlightManipulator());
    keyswitchManipulator->addMatrixManipulator('4', "Drive",     new osgGA::DriveManipulator());

    view.setCameraManipulator(keyswitchManipulator.get());
}


int main(int argc, char* argv[])
{
    osg::notify(osg::NOTICE) << "Use [1] to [4] to select camera manipulator." << std::endl;
    osg::notify(osg::NOTICE) << "Use [space] to cycle mapping techniques." << std::endl;

    osg::ArgumentParser psr(&argc, argv);
    osgViewer::Viewer viewer(psr);

    viewer.setUpViewInWindow(128, 128, 640, 480);

    // Setup default demo.
    g_demo = D_CubeMappedHimmel;

    while (psr.read("--polar")) 
        g_demo = D_PolarMappedHimmel;

    while (psr.read("--cube")) 
        g_demo = D_CubeMappedHimmel;


    initializeManipulators(viewer);
    initializeScene(viewer);

    activateDemo(g_demo);

    viewer.addEventHandler(new KeyboardEventHandler);

    return viewer.run();
}