
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

#include "osgHimmel/polarmappedhimmel.h"
#include "osgHimmel/cubemappedhimmel.h"
#include "osgHimmel/paraboloidmappedhimmel.h"
#include "osgHimmel/spheremappedhimmel.h"
#include "osgHimmel/horizonband.h"
#include "osgHimmel/himmelenvmap.h"

#include "osgHimmel/timef.h"


#include <osg/TextureCubeMap>
#include <osg/Texture2D>

#include <osgDB/ReadFile>
#include <osgViewer/ViewerEventHandlers>


using namespace osgHimmel;


// utils

// cubeMapFilePaths should contain a questionmark '?' that is replaced
// by cubemap extensions '_px', '_nx', '_py', etc. 
// e.g. "resources/starmap?.png" points to "resources/starmap_px.png" etc.

void loadCubeMapFromFiles(
    const std::string cubeMapFilePath
,   osg::TextureCubeMap &tcm)
{
    std::string px = cubeMapFilePath; px.replace(px.find("?"), 1, "_px");
    std::string nx = cubeMapFilePath; nx.replace(nx.find("?"), 1, "_nx");
    std::string py = cubeMapFilePath; py.replace(py.find("?"), 1, "_py");
    std::string ny = cubeMapFilePath; ny.replace(ny.find("?"), 1, "_ny");
    std::string pz = cubeMapFilePath; pz.replace(pz.find("?"), 1, "_pz");
    std::string nz = cubeMapFilePath; nz.replace(nz.find("?"), 1, "_nz");

    tcm.setImage(osg::TextureCubeMap::POSITIVE_X, osgDB::readImageFile(px));
    tcm.setImage(osg::TextureCubeMap::NEGATIVE_X, osgDB::readImageFile(nx));
    tcm.setImage(osg::TextureCubeMap::POSITIVE_Y, osgDB::readImageFile(py));
    tcm.setImage(osg::TextureCubeMap::NEGATIVE_Y, osgDB::readImageFile(ny));
    tcm.setImage(osg::TextureCubeMap::POSITIVE_Z, osgDB::readImageFile(pz));
    tcm.setImage(osg::TextureCubeMap::NEGATIVE_Z, osgDB::readImageFile(nz));
}


// scenes

TimeF *g_timef(new TimeF());


osg::ref_ptr<AbstractHimmel> createPolarMappedDemo()
{
    osg::ref_ptr<PolarMappedHimmel> himmel = new PolarMappedHimmel(PolarMappedHimmel::MM_Half, true);

    himmel->hBand()->setBottomColor(osg::Vec4(0.48f, 0.46f, 0.42f, 1.00f));
    himmel->hBand()->setColor(      osg::Vec4(0.70f, 0.65f, 0.6f, 1.00f));
    himmel->hBand()->setScale(0.1f);

    himmel->assignTime(g_timef);
    himmel->setTransitionDuration(0.1f);

    himmel->setSecondsPerRAZ(300.f);
    himmel->setRazDirection(AbstractMappedHimmel::RD_NorthWestSouthEast);

    himmel->getOrCreateTexture2D(0)->setImage(osgDB::readImageFile("resources/polar_half_art_0.jpg"));
    himmel->getOrCreateTexture2D(1)->setImage(osgDB::readImageFile("resources/polar_half_art_1.jpg"));
    himmel->getOrCreateTexture2D(2)->setImage(osgDB::readImageFile("resources/polar_half_gen_2.jpg"));
    himmel->getOrCreateTexture2D(3)->setImage(osgDB::readImageFile("resources/polar_half_pho_1.jpg"));
    himmel->getOrCreateTexture2D(4)->setImage(osgDB::readImageFile("resources/polar_half_pho_7.jpg"));

    himmel->pushTextureUnit(0, 0.0f);
    himmel->pushTextureUnit(1, 0.33f);
    himmel->pushTextureUnit(2, 0.66f);
    himmel->pushTextureUnit(3, 0.6f);
    himmel->pushTextureUnit(4, 0.8f);

    return himmel;
}


osg::ref_ptr<AbstractHimmel> createCubeMappedDemo()
{
    osg::ref_ptr<CubeMappedHimmel> himmel = new CubeMappedHimmel(false);

    himmel->assignTime(g_timef);
    himmel->setTransitionDuration(0.2f);

    std::string name[] = { "4", "6", "9", "17", "19" };
    osg::TextureCubeMap *tcm[5];

    const int n = 1;
    for(int i = 0; i < n; ++i)
    {
        tcm[i] = himmel->getOrCreateTextureCubeMap(i);
        loadCubeMapFromFiles("resources/cube_gen_" + name[i] + "?.jpg", *tcm[i]);

        himmel->pushTextureUnit(i);
    }
    return himmel;
}


osg::ref_ptr<AbstractHimmel> createParaboloidMappedDemo()
{
    osg::ref_ptr<ParaboloidMappedHimmel> himmel = new ParaboloidMappedHimmel();

    himmel->assignTime(g_timef);
    himmel->setTransitionDuration(0.05f);

    himmel->getOrCreateTexture2D(0)->setImage(osgDB::readImageFile("resources/paraboloid_gen_0.jpg"));
    himmel->getOrCreateTexture2D(1)->setImage(osgDB::readImageFile("resources/paraboloid_gen_1.jpg"));
    himmel->getOrCreateTexture2D(2)->setImage(osgDB::readImageFile("resources/paraboloid_gen_2.jpg"));
    himmel->getOrCreateTexture2D(3)->setImage(osgDB::readImageFile("resources/paraboloid_gen_3.jpg"));
    himmel->getOrCreateTexture2D(4)->setImage(osgDB::readImageFile("resources/paraboloid_pho_0.jpg"));
    himmel->getOrCreateTexture2D(5)->setImage(osgDB::readImageFile("resources/paraboloid_pho_1.jpg"));
    himmel->getOrCreateTexture2D(6)->setImage(osgDB::readImageFile("resources/paraboloid_pho_2.jpg"));
    himmel->getOrCreateTexture2D(7)->setImage(osgDB::readImageFile("resources/paraboloid_pho_3.jpg"));

    const float a = 1.0 / 8.0;

    himmel->pushTextureUnit(0, 0.0 * a);
    himmel->pushTextureUnit(1, 1.0 * a);
    himmel->pushTextureUnit(2, 2.0 * a);
    himmel->pushTextureUnit(3, 3.0 * a);
    himmel->pushTextureUnit(4, 4.0 * a);
    himmel->pushTextureUnit(5, 5.0 * a);
    himmel->pushTextureUnit(6, 6.0 * a);
    himmel->pushTextureUnit(7, 7.0 * a);

    return himmel;
}


osg::ref_ptr<AbstractHimmel> createSphereMappedDemo()
{
    osg::ref_ptr<SphereMappedHimmel> himmel = new SphereMappedHimmel();
    himmel->setRazDirection(AbstractMappedHimmel::RD_NorthEastSouthWest);

    himmel->assignTime(g_timef);
    himmel->setTransitionDuration(0.05f);

    himmel->getOrCreateTexture2D(0)->setImage(osgDB::readImageFile("resources/sphere_gen_0.jpg"));
    himmel->getOrCreateTexture2D(1)->setImage(osgDB::readImageFile("resources/sphere_gen_2.jpg"));

    himmel->pushTextureUnit(0, 0.00f);
    himmel->pushTextureUnit(1, 0.50f);

    return himmel;
}


// demo

#include <osgViewer/Viewer>
#include <osg/MatrixTransform>

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
,   D_None                    = 4
};

osg::ref_ptr<osg::Group> g_root;


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
        case(osgGA::GUIEventAdapter::FRAME):

            g_timef->update();
            break;

        case(osgGA::GUIEventAdapter::KEYDOWN):
            {
                if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Space)
                {
                    g_demo = static_cast<e_Demo>((g_demo + 1) % 4);
                    activateDemo(g_demo);
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


void fovChanged()
{
    const double aspectRatio(g_view->getCamera()->getViewport()->aspectRatio());
    g_view->getCamera()->setProjectionMatrixAsPerspective(g_fov, aspectRatio, 0.1f, 8.0f);
}


osg::Group *createHimmelScene()
{
    osg::Group *group = new osg::Group;

    g_himmelsByDemo[D_PolarMappedHimmel] = createPolarMappedDemo();
    group->addChild(g_himmelsByDemo[D_PolarMappedHimmel]);

    g_himmelsByDemo[D_CubeMappedHimmel] = createCubeMappedDemo();
    group->addChild(g_himmelsByDemo[D_CubeMappedHimmel]);

    g_himmelsByDemo[D_ParaboloidMappedHimmel] = createParaboloidMappedDemo();
    group->addChild(g_himmelsByDemo[D_ParaboloidMappedHimmel]);

    g_himmelsByDemo[D_SphereMappedHimmel] = createSphereMappedDemo();
    group->addChild(g_himmelsByDemo[D_SphereMappedHimmel]);

    return group;
}


// render with envmap

#include <osg/TexMat>
#include <osg/TexGenNode>
#include <osg/Material>

osg::Node *createReflector()
{
    osg::Node *node = osgDB::readNodeFile("resources/knot.obj");

    if(!node)
    {
        osg::notify(osg::WARN) << "Mesh \"resources/knot.obj\" not found." << std::endl;
        return NULL;
    }

    osg::ref_ptr<osg::Material> m = new osg::Material;
    m->setColorMode(osg::Material::DIFFUSE);
    m->setAmbient  (osg::Material::FRONT_AND_BACK, osg::Vec4(6.0f, 6.0f, 6.0f, 1.f));

    node->getOrCreateStateSet()->setAttributeAndModes(m.get(), osg::StateAttribute::ON);

    return node;
}


class TexMatCullCallback : public osg::NodeCallback
{
    public:
    
        TexMatCullCallback(osg::TexMat *texmat)
        :   m_texmat(texmat)
        {
        }
       
        virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
        {
            traverse(node, nv);

            osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
            if(cv)
            {
                const osg::Quat quat = cv->getModelViewMatrix()->getRotate();
                m_texmat->setMatrix(osg::Matrix::rotate(quat.inverse()));
            }
        }
        
    protected:
        osg::ref_ptr<osg::TexMat> m_texmat;
};


osg::Group *createScene(
    osg::Node *scene
,   osg::Node *reflector)
{
    const unsigned int unit = 0;

    // create the texgen node to project the tex coords onto the subgraph:
    osg::TexGenNode* texgenNode = new osg::TexGenNode;

    texgenNode->getTexGen()->setMode(osg::TexGen::REFLECTION_MAP);
    texgenNode->setTextureUnit(unit);

    texgenNode->addChild(reflector);


    HimmelEnvMap *envMap = new HimmelEnvMap(128);
    envMap->addChild(scene);


    osg::Group *group = new osg::Group();
    group->addChild(envMap);
    group->addChild(texgenNode);

    osg::StateSet* stateset = reflector->getOrCreateStateSet();
    stateset->setTextureAttributeAndModes(unit, envMap->cubeMap(), osg::StateAttribute::ON);
    stateset->setTextureMode(unit, GL_TEXTURE_GEN_S, osg::StateAttribute::ON);
    stateset->setTextureMode(unit, GL_TEXTURE_GEN_T, osg::StateAttribute::ON);
    stateset->setTextureMode(unit, GL_TEXTURE_GEN_R, osg::StateAttribute::ON);
    stateset->setTextureMode(unit, GL_TEXTURE_GEN_Q, osg::StateAttribute::ON);

    osg::TexMat* texmat = new osg::TexMat;
    stateset->setTextureAttributeAndModes(unit, texmat, osg::StateAttribute::ON);
        
    reflector->setCullCallback(new TexMatCullCallback(texmat));


    return group;
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





//class RecordCameraPathHandlerFix : public osgGA::GUIEventHandler
//{
//public:
//
//        RecordCameraPathHandlerFix(const std::string &filename = "saved_animation.path", float fps = 25.0f);
//
//        void setKeyEventToggleRecord(int key) { _keyEventToggleRecord = key; }
//        int getKeyEventToggleRecord() const { return _keyEventToggleRecord; }
//
//        void setKeyEventTogglePlayback(int key) { _keyEventTogglePlayback = key; }
//        int getKeyEventTogglePlayback() const { return _keyEventTogglePlayback; }
//
//        void setAutoIncrementFilename( bool autoinc = true ) { _autoinc = autoinc?0:-1; }
//
//        virtual void getUsage(osg::ApplicationUsage &usage) const;
//
//        bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
//
//protected:
//
//        std::string                                     _filename;
//        int                                             _autoinc;
//        osgDB::ofstream                                 _fout;
//
//        int                                             _keyEventToggleRecord;
//        int                                             _keyEventTogglePlayback;
//
//
//        bool                                            _currentlyRecording;
//        bool                                            _currentlyPlaying;
//        double                                          _interval;
//        double                                          _delta;
//        osg::Timer_t                                    _animStartTime;
//        osg::Timer_t                                    _lastFrameTime;
//        osg::ref_ptr<osg::AnimationPath>                _animPath;
//        osg::ref_ptr<osgGA::AnimationPathManipulator>   _animPathManipulator;
//        osg::ref_ptr<osgGA::CameraManipulator>          _oldManipulator;
//};

//
//RecordCameraPathHandlerFix::RecordCameraPathHandlerFix(const std::string& filename, float fps):
//    _filename(filename),
//    _autoinc( -1 ),
//    _keyEventToggleRecord('z'),
//    _keyEventTogglePlayback('Z'),
//    _currentlyRecording(false),
//    _currentlyPlaying(false),
//    _delta(0.0f),
//    _animStartTime(0),
//    _lastFrameTime(osg::Timer::instance()->tick())
//{
//    _animPath = new osg::AnimationPath();
//
//    const char* str = getenv("OSG_RECORD_CAMERA_PATH_FPS");
//    if (str)
//    {
//        _interval = 1.0f / osg::asciiToDouble(str);
//    }
//    else
//    {
//        _interval = 1.0f / fps;
//    }
//}
//
//void RecordCameraPathHandlerFix::getUsage(osg::ApplicationUsage &usage) const
//{
//    usage.addKeyboardMouseBinding(reinterpret_cast<const char*>(&_keyEventToggleRecord), "Toggle camera path recording.");
//    usage.addKeyboardMouseBinding(reinterpret_cast<const char*>(&_keyEventTogglePlayback), "Toggle camera path playback.");
//}
//
//
//#include <iomanip>
//#include <sstream>
//
//#include <osgDB/FileNameUtils>
//
//bool RecordCameraPathHandlerFix::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
//{
//    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
//
//    if (view == NULL)
//    {
//        return false;
//    }
//
//    if(ea.getEventType()==osgGA::GUIEventAdapter::FRAME)
//    {
//        // Calculate our current delta (difference) in time between the last frame and
//        // current frame, regardless of whether we actually store a ControlPoint...
//        osg::Timer_t time = osg::Timer::instance()->tick();
//        double delta = osg::Timer::instance()->delta_s(_lastFrameTime, time);
//        _lastFrameTime = time;
//
//        // If our internal _delta is finally large enough to warrant a ControlPoint
//        // insertion, do so now. Be sure and reset the internal _delta, so we can start
//        // calculating when the next insert should happen.
//        if (_currentlyRecording && _delta >= _interval)
//        {
//            const osg::Matrixd& m = view->getCamera()->getInverseViewMatrix();
//            double animationPathTime = osg::Timer::instance()->delta_s(_animStartTime, time);            
//            _animPath->insert(animationPathTime, osg::AnimationPath::ControlPoint(m.getTrans(), m.getRotate()));
//            _delta = 0.0f;
//
//            if (_fout)
//            {
//                _animPath->write(_animPath->getTimeControlPointMap().find(animationPathTime), _fout);
//                _fout.flush();
//            }
//
//        }
//        else _delta += delta;
//        
//        return true;
//    }
//
//    if (ea.getHandled()) return false;
//
//    switch(ea.getEventType())
//    {
//        case(osgGA::GUIEventAdapter::KEYUP):
//        {
//            // The user has requested to toggle recording.
//            if (ea.getKey() ==_keyEventToggleRecord)
//            {
//                // The user has requested to BEGIN recording.
//                if (!_currentlyRecording)
//                {
//                    _currentlyRecording = true;
//                    _animStartTime = osg::Timer::instance()->tick();
//                    _animPath->clear();
//                    
//                    if (!_filename.empty())
//                    {
//                        std::stringstream ss;
//                        ss << osgDB::getNameLessExtension(_filename);
//                        if ( _autoinc != -1 )
//                        {
//                            ss << "_"<<std::setfill( '0' ) << std::setw( 2 ) << _autoinc;
//                            _autoinc++;
//                        }
//                        ss << "."<<osgDB::getFileExtension(_filename);
//                        
//                        OSG_NOTICE << "Recording camera path to file " << ss.str() << std::endl;
//                        _fout.open( ss.str().c_str() );
//
//                        // make sure doubles are not trucated by default stream precision = 6
//                        _fout.precision( 15 );
//                    }
//                    else
//                    {
//                        OSG_NOTICE<<"Recording camera path."<<std::endl;
//                    }
//                }
//
//                // The user has requested to STOP recording, write the file!
//                else
//                {
//                    _currentlyRecording = false;
//                    _delta = 0.0f;
//
//                    if (_fout) _fout.close();
//                }
//
//                return true;
//            }
//
//            // The user has requested to toggle playback. You'll notice in the code below that
//            // we take over the current manipulator; it was originally recommended that we
//            // check for a KeySwitchManipulator, create one if not present, and then add this
//            // to either the newly created one or the existing one. However, the code do that was
//            // EXTREMELY dirty, so I opted for a simpler solution. At a later date, someone may
//            // want to implement the original recommendation (which is in a mailing list reply
//            // from June 1st by Robert in a thread called "osgviewer Camera Animation (preliminary)".
//            else if (ea.getKey() == _keyEventTogglePlayback)
//            {
//                if (_currentlyRecording)
//                {
//                    _currentlyRecording = false;
//                    _delta = 0.0f;
//
//                    // In the future this will need to be written continuously, rather
//                    // than all at once.
//                    osgDB::ofstream out(_filename.c_str());
//                    OSG_NOTICE<<"Writing camera file: "<<_filename<<std::endl;
//                    _animPath->write(out);
//                    out.close();
//                }
//
//                // The user has requested to BEGIN playback.
//                if (!_currentlyPlaying)
//                {                    
//                    _animPathManipulator = new osgGA::AnimationPathManipulator(_filename);
//                    _animPathManipulator->home(ea,aa);
//                    
//
//                    // If we successfully found our _filename file, set it and keep a copy
//                    // around of the original CameraManipulator to restore later.
//                    if (_animPathManipulator.valid() && _animPathManipulator->valid())
//                    {
//                        _oldManipulator = view->getCameraManipulator();
//                        view->setCameraManipulator(_animPathManipulator.get());
//                        _currentlyPlaying = true;
//                    }
//                }
//
//                // The user has requested to STOP playback.
//                else
//                {
//                    // Restore the old manipulator if necessary and stop playback.
//                    if(_oldManipulator.valid()) view->setCameraManipulator(_oldManipulator.get());
//                    _currentlyPlaying = false;
//                    _oldManipulator = 0;
//                }
//
//                return true;
//            }        
//
//            break;
//        }
//    default:
//        break;
//    }
//
//    return false;
//}
//


#include <osg/Light>
#include <osg/LightSource>

int main(int argc, char* argv[])
{
    osg::ArgumentParser arguments(&argc, argv);

    arguments.getApplicationUsage()->setDescription(
        arguments.getApplicationName() + " demonstrates the various texture mapped skies of osgHimmel");

    arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName());

    arguments.getApplicationUsage()->addCommandLineOption("-h or --help", "Display this information.");
    arguments.getApplicationUsage()->addCommandLineOption("--polar", "Start with a himmel using polar mapped textures.");
    arguments.getApplicationUsage()->addCommandLineOption("--cube", "Start with a himmel using cube mapped textures.");
    arguments.getApplicationUsage()->addCommandLineOption("--paraboloid", "Start with a himmel using paraboloid mapped textures.");
    arguments.getApplicationUsage()->addCommandLineOption("--sphere", "Start with a himmel using sphere mapped textures.");

    osgViewer::Viewer viewer(arguments);

    if(arguments.read("-h") || arguments.read("--help"))
    {
        arguments.getApplicationUsage()->write(std::cout);
        return 1;
    }

    osg::notify(osg::NOTICE) << "Use [1] to [4] to select camera manipulator." << std::endl;
    osg::notify(osg::NOTICE) << "Use [space] to cycle mapping techniques." << std::endl;
    osg::notify(osg::NOTICE) << "Use [p] to pause/unpause time." << std::endl;
    osg::notify(osg::NOTICE) << "Use [r] or [s] to reset or stop the time." << std::endl;
    osg::notify(osg::NOTICE) << "Use [+] and [-] to increase/decrease seconds per cycle." << std::endl;
    osg::notify(osg::NOTICE) << "Use [mouse wheel] to change field of view." << std::endl;

    g_demo = D_PolarMappedHimmel;

    while(arguments.read("--polar")) 
        g_demo = D_PolarMappedHimmel;

    while(arguments.read("--cube")) 
        g_demo = D_CubeMappedHimmel;

    while(arguments.read("--paraboloid")) 
        g_demo = D_ParaboloidMappedHimmel;

    while(arguments.read("--sphere")) 
        g_demo = D_SphereMappedHimmel;

    g_view = dynamic_cast<osgViewer::View*>(&viewer);

    viewer.setUpViewInWindow(128, 128, 1280, 720);
    
    //RecordCameraPathHandlerFix *rcph = new RecordCameraPathHandlerFix("D:/p/osghimmel/exchange/movie-döllner/New folder/animation.path");
    //rcph->setKeyEventTogglePlayback('w');
    //
    //viewer.addEventHandler(rcph);


    initializeManipulators(viewer);
    

    osg::Camera *cam = g_view->getCamera();
    fovChanged();

    cam->setClearColor(osg::Vec4(0.8, 0.84, 0.9, 1.0));

    g_root  = new osg::Group();
    g_view->setSceneData(g_root.get());


    osg::ref_ptr<osg::Group> himmel = createHimmelScene();

    osg::Node *reflector(createReflector());
    if(reflector)
    {
        osg::ref_ptr<osg::Group> scene = createScene(
            himmel.get(), reflector);

        g_root->addChild(scene.get());
    }
    else
        g_root->addChild(himmel.get());


        osg::Group *group = new osg::Group();


  osg::Light *light(new osg::Light);
    osg::LightSource *lsource(new osg::LightSource);

    lsource->setLight(light);
    g_root->addChild(lsource);
    group->addChild(lsource);

    lsource->setLocalStateSetModes(osg::StateAttribute::ON);

    lsource->setStateSetModes(*group->getOrCreateStateSet(), osg::StateAttribute::ON);

 
    g_root->addChild(group);

    activateDemo(D_PolarMappedHimmel);

    viewer.addEventHandler(new KeyboardEventHandler);

    return viewer.run();
}