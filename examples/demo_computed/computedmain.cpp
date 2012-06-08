
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


#include "osgHimmel/himmel.h"
#include "osgHimmel/timef.h"

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/TerrainManipulator>

#include <iostream>


osgHimmel::TimeF *g_timef(new osgHimmel::TimeF(0.0L, 3600.0L));

const float g_fovBackup = 60.f;
float g_fov = g_fovBackup;

osgViewer::View *g_view = NULL;


void initializeManipulators(osgViewer::View &view)
{
    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

    keyswitchManipulator->addMatrixManipulator('1', "Terrain",   new osgGA::TerrainManipulator());
    keyswitchManipulator->addMatrixManipulator('2', "Trackball", new osgGA::TrackballManipulator());
    keyswitchManipulator->addMatrixManipulator('3', "Flight",    new osgGA::FlightManipulator());
    keyswitchManipulator->addMatrixManipulator('4', "Drive",     new osgGA::DriveManipulator());

    view.setCameraManipulator(keyswitchManipulator.get());
}


void fovChanged()
{
    const double aspectRatio(g_view->getCamera()->getViewport()->aspectRatio());
    g_view->getCamera()->setProjectionMatrixAsPerspective(g_fov, aspectRatio, 0.1f, 8.0f);
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
        case(osgGA::GUIEventAdapter::FRAME):

            g_timef->update();
            break;

        case(osgGA::GUIEventAdapter::KEYDOWN):
            {
                if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Space)
                {
                }
                else if(ea.getKey() == 'r' || ea.getKey() == 'R')
                {
                    g_timef->reset();
                    g_timef->setSecondsPerCycle(60.f);
                }
                else if(ea.getKey() == 's' || ea.getKey() == 'S')
                {
                    g_timef->stop();
                    g_timef->setSecondsPerCycle(60.f);
                }
                else if(ea.getKey() == 'p' || ea.getKey() == 'P')
                {
                    if(g_timef->isRunning())
                        g_timef->pause();
                    else
                        g_timef->start();
                }
                else if(ea.getKey() == '-')
                {
                    g_timef->setSecondsPerCycle(g_timef->getSecondsPerCycle() + 1);
                }
                else if(ea.getKey() == '+')
                {
                    if(g_timef->getSecondsPerCycle() - 1 > 0.f)
                        g_timef->setSecondsPerCycle(g_timef->getSecondsPerCycle() - 1);
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


int main(int argc, char* argv[])
{
    osg::ArgumentParser arguments(&argc, argv);

    arguments.getApplicationUsage()->setDescription(
        arguments.getApplicationName() + " demonstrates the procedural sky of osgHimmel");

    arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName());

    arguments.getApplicationUsage()->addCommandLineOption("-h or --help", "Display this information.");
    //arguments.getApplicationUsage()->addCommandLineOption("--polar", "Start with a himmel using polar mapped textures.");

    osgViewer::Viewer viewer(arguments);

    if(arguments.read("-h") || arguments.read("--help"))
    {
        arguments.getApplicationUsage()->write(std::cout);
        return 1;
    }

    //osg::notify(osg::NOTICE) << "Use [1] to [4] to select camera manipulator." << std::endl;
    //osg::notify(osg::NOTICE) << "Use [space] to cycle mapping techniques." << std::endl;
    //osg::notify(osg::NOTICE) << "Use [p] to pause/unpause time." << std::endl;
    //osg::notify(osg::NOTICE) << "Use [r] or [s] to reset or stop the time." << std::endl;
    //osg::notify(osg::NOTICE) << "Use [+] and [-] to increase/decrease seconds per cycle." << std::endl;
    //osg::notify(osg::NOTICE) << "Use [mouse wheel] to change field of view." << std::endl;

    //while(arguments.read("--polar")) 
    //    g_demo = D_PolarMappedHimmel;

       
    g_view = dynamic_cast<osgViewer::View*>(&viewer);

    viewer.setUpViewInWindow(128, 128, 1280, 720); 
    initializeManipulators(viewer);

    osg::Camera *cam = g_view->getCamera();
    fovChanged();

    osg::ref_ptr<osg::Group> root  = new osg::Group();
    g_view->setSceneData(root.get());

    osg::ref_ptr<osgHimmel::Himmel>  himmel = osgHimmel::Himmel::create();
    himmel->assignTime(g_timef);

    root->addChild(himmel);


    viewer.addEventHandler(new KeyboardEventHandler);

    return viewer.run();
}