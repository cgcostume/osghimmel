
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
#include "include/paraboloidmappedhimmel.h"
#include "include/spheremappedhimmel.h"
#include "include/horizonband.h"

#include "include/timef.h"


#include <osg/TextureCubeMap>
#include <osg/Texture2D>

#include <osgDB/ReadFile>


// scenes

TimeF *g_timef(new TimeF(0.f, 60.f));


osg::ref_ptr<AbstractHimmel> createPolarMappedDemo()
{
    osg::ref_ptr<PolarMappedHimmel> himmel = new PolarMappedHimmel(PolarMappedHimmel::MM_Half, true);

    himmel->hBand()->setBottomColor(osg::Vec4(0.30f, 0.30f, 0.30f, 1.00f));
    himmel->hBand()->setColor(      osg::Vec4(0.20f, 0.20f, 0.20f, 1.00f));
    himmel->hBand()->setScale(0.3f);

    himmel->assignTime(g_timef, true);
    himmel->setTransitionDuration(0.05f);

    himmel->getOrCreateTexture2D(0)->setImage(osgDB::readImageFile("resources/polar_half_art_1.jpg"));
    himmel->getOrCreateTexture2D(1)->setImage(osgDB::readImageFile("resources/polar_half_art_2.jpg"));
    himmel->getOrCreateTexture2D(2)->setImage(osgDB::readImageFile("resources/polar_half_gen_3.jpg"));
    himmel->getOrCreateTexture2D(3)->setImage(osgDB::readImageFile("resources/polar_half_pho_1.jpg"));
    himmel->getOrCreateTexture2D(4)->setImage(osgDB::readImageFile("resources/polar_half_pho_7.jpg"));

    himmel->pushTextureUnit(0, 0.0f);
    himmel->pushTextureUnit(1, 0.2f);
    himmel->pushTextureUnit(2, 0.4f);
    himmel->pushTextureUnit(3, 0.6f);
    himmel->pushTextureUnit(4, 0.8f);

    return himmel;
}


osg::ref_ptr<AbstractHimmel> createCubeMappedDemo()
{
    osg::ref_ptr<CubeMappedHimmel> himmel = new CubeMappedHimmel();

    himmel->assignTime(g_timef, true);
    himmel->setTransitionDuration(0.05f);

    std::string name[] = { "4", "6", "9", "17", "19" };
    osg::TextureCubeMap *tcm[5];

    const int n = 5;
    for(int i = 0; i < n; ++i)
    {
        tcm[i] = himmel->getOrCreateTextureCubeMap(i);

        tcm[i]->setImage(osg::TextureCubeMap::POSITIVE_X, osgDB::readImageFile("resources/cube_gen_" + name[i] + "_px.jpg"));
        tcm[i]->setImage(osg::TextureCubeMap::NEGATIVE_X, osgDB::readImageFile("resources/cube_gen_" + name[i] + "_nx.jpg"));
        tcm[i]->setImage(osg::TextureCubeMap::POSITIVE_Y, osgDB::readImageFile("resources/cube_gen_" + name[i] + "_py.jpg"));
        tcm[i]->setImage(osg::TextureCubeMap::NEGATIVE_Y, osgDB::readImageFile("resources/cube_gen_" + name[i] + "_ny.jpg"));
        tcm[i]->setImage(osg::TextureCubeMap::POSITIVE_Z, osgDB::readImageFile("resources/cube_gen_" + name[i] + "_pz.jpg"));
        tcm[i]->setImage(osg::TextureCubeMap::NEGATIVE_Z, osgDB::readImageFile("resources/cube_gen_" + name[i] + "_nz.jpg"));

      himmel->pushTextureUnit(i, (i * 1.f) / (n * 1.f));
    }
    return himmel;
}


osg::ref_ptr<AbstractHimmel> createParaboloidMappedDemo()
{
    osg::ref_ptr<ParaboloidMappedHimmel> himmel = new ParaboloidMappedHimmel();

    himmel->assignTime(g_timef, true);
    himmel->setTransitionDuration(0.05f);

    himmel->getOrCreateTexture2D(0)->setImage(osgDB::readImageFile("resources/paraboloid_gen_0.jpg"));
    himmel->getOrCreateTexture2D(1)->setImage(osgDB::readImageFile("resources/paraboloid_gen_1.jpg"));
    himmel->getOrCreateTexture2D(2)->setImage(osgDB::readImageFile("resources/paraboloid_gen_2.jpg"));
    himmel->getOrCreateTexture2D(3)->setImage(osgDB::readImageFile("resources/paraboloid_pho_0.jpg"));
    himmel->getOrCreateTexture2D(4)->setImage(osgDB::readImageFile("resources/paraboloid_pho_1.jpg"));

    himmel->pushTextureUnit(0, 0.0f);
    himmel->pushTextureUnit(1, 0.2f);
    himmel->pushTextureUnit(2, 0.4f);
    himmel->pushTextureUnit(3, 0.6f);
    himmel->pushTextureUnit(4, 0.8f);

    return himmel;
}


osg::ref_ptr<AbstractHimmel> createSphereMappedDemo()
{
    osg::ref_ptr<SphereMappedHimmel> himmel = new SphereMappedHimmel(SphereMappedHimmel::MM_TowardsNegY);

    himmel->assignTime(g_timef, true);
    himmel->setTransitionDuration(0.05f);

    himmel->getOrCreateTexture2D(0)->setImage(osgDB::readImageFile("resources/sphere_gen_0.jpg"));
    himmel->getOrCreateTexture2D(1)->setImage(osgDB::readImageFile("resources/sphere_gen_2.jpg"));

    himmel->pushTextureUnit(0, 0.00f);
    himmel->pushTextureUnit(1, 0.50f);

    return himmel;
}


// demo

#include <osgViewer/Viewer>

#include <osg/PolygonMode>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/TerrainManipulator>

#include <iostream>

enum e_Demo
{
    D_PolarMappedHimmel       = 0
,   D_CubeMappedHimmel        = 1
,   D_ParaboloidMappedHimmel  = 2
,   D_SphereMappedHimmel      = 3
};


osg::ref_ptr<osg::Group> g_scene = NULL;

e_Demo g_demo;
std::map<e_Demo, osg::ref_ptr<AbstractHimmel> > g_himmelsByDemo;


osgViewer::View *g_view = NULL;

const float g_fovBackup = 60.f;
float g_fov = g_fovBackup;


void activateDemo(e_Demo demo)
{
    g_himmelsByDemo[D_PolarMappedHimmel]     ->setNodeMask(D_PolarMappedHimmel      == demo);
    g_himmelsByDemo[D_CubeMappedHimmel]      ->setNodeMask(D_CubeMappedHimmel       == demo);
    g_himmelsByDemo[D_ParaboloidMappedHimmel]->setNodeMask(D_ParaboloidMappedHimmel == demo);
    g_himmelsByDemo[D_SphereMappedHimmel]    ->setNodeMask(D_SphereMappedHimmel     == demo);
}


void fovChanged();

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
                    g_demo = static_cast<e_Demo>((g_demo + 1) % 4);
                    activateDemo(g_demo);
                }
            }
            break;

        case(osgGA::GUIEventAdapter::SCROLL):
            {
                const float f = 1.00f
                    + (ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_DOWN ? -0.08f : +0.08f);

                if(g_fov * f >= 1.f && g_fov * f <= 179.f)
                    g_fov *= f;

                fovChanged();

                return true;
            }
            break;

        case(osgGA::GUIEventAdapter::RELEASE):

            if(ea.getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON
            && (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL) != 0)
            {
                g_fov = g_fovBackup;
                fovChanged();

                return true;
            }
            break;

        default:
            break;
        };
        return false;
    }
};


void fovChanged()
{
    const double aspectRatio(g_view->getCamera()->getViewport()->aspectRatio());
    g_view->getCamera()->setProjectionMatrixAsPerspective(g_fov, aspectRatio, 0.1f, 8.0f);
}


void initializeScene()
{
    osg::Camera* cam = g_view->getCamera();
    fovChanged();

    osg::ref_ptr<osg::Group> root = new osg::Group;

    g_scene = new osg::Group;
    root->addChild(g_scene);

    g_himmelsByDemo[D_PolarMappedHimmel] = createPolarMappedDemo();
    root->addChild(g_himmelsByDemo[D_PolarMappedHimmel]);

    g_himmelsByDemo[D_CubeMappedHimmel] = createCubeMappedDemo();
    root->addChild(g_himmelsByDemo[D_CubeMappedHimmel]);

    g_himmelsByDemo[D_ParaboloidMappedHimmel] = createParaboloidMappedDemo();
    root->addChild(g_himmelsByDemo[D_ParaboloidMappedHimmel]);

    g_himmelsByDemo[D_SphereMappedHimmel] = createSphereMappedDemo();
    root->addChild(g_himmelsByDemo[D_SphereMappedHimmel]);


    g_view->setSceneData(root.get());

    osg::ref_ptr<osg::Node> loadedScene = osgDB::readNodeFile("resources/knot.obj");
    g_scene->addChild(loadedScene.get());
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
    osg::ArgumentParser arguments(&argc, argv);
    arguments.getApplicationUsage()->setDescription(
        arguments.getApplicationName() + " is the demo which demonstrates using of various texture mapped skies implemented in osghimmel");

    arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName());
    arguments.getApplicationUsage()->addCommandLineOption("-h or --help", "Display this information.");
    arguments.getApplicationUsage()->addCommandLineOption("--polar", "Start with a himmel using polar mapped textures.");
    arguments.getApplicationUsage()->addCommandLineOption("--cube", "Start with a himmel using cube mapped textures.");
    arguments.getApplicationUsage()->addCommandLineOption("--paraboloid", "Start with a himmel using paraboloid mapped textures.");
    arguments.getApplicationUsage()->addCommandLineOption("--sphere", "Start with a himmel using sphere mapped textures.");


    osg::notify(osg::NOTICE) << "Use [1] to [4] to select camera manipulator." << std::endl;
    osg::notify(osg::NOTICE) << "Use [space] to cycle mapping techniques." << std::endl;

    osgViewer::Viewer viewer(arguments);
    g_view = dynamic_cast<osgViewer::View*>(&viewer);

    viewer.setUpViewInWindow(128, 128, 640, 480);

    // Setup default demo.
    g_demo = D_CubeMappedHimmel;

    if(arguments.read("-h") || arguments.read("--help"))
    {
        arguments.getApplicationUsage()->write(std::cout);
        return 1;
    }

    while(arguments.read("--polar")) 
        g_demo = D_PolarMappedHimmel;

    while(arguments.read("--cube")) 
        g_demo = D_CubeMappedHimmel;

    while(arguments.read("--paraboloid")) 
        g_demo = D_ParaboloidMappedHimmel;

    while(arguments.read("--sphere")) 
        g_demo = D_SphereMappedHimmel;

    initializeManipulators(viewer);
    initializeScene();

    activateDemo(g_demo);

    viewer.addEventHandler(new KeyboardEventHandler);

    return viewer.run();
}