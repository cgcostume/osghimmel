
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
#include "osgHimmel/osgposter.h"

#include "osgHimmel/timef.h"


#include <osg/TextureCubeMap>
#include <osg/Texture2D>

#include <osgDB/ReadFile>
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/WriteFile>


using namespace osgHimmel;

const int CUBE_MAP_RES = 128;


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

TimeF *g_timef(new TimeF((t_longf)0.0, (t_longf)30.0));

osg::ref_ptr<AbstractHimmel> createCubeMappedDemo()
{
    osg::ref_ptr<CubeMappedHimmel> himmel = new CubeMappedHimmel(false);

    himmel->assignTime(g_timef);
    himmel->setTransitionDuration(0.2f);

    //std::string name[] = { "9", "6", "4", "17", "19" };
	std::string name[] = { "4", "6", "9", "17", "19" };
    osg::TextureCubeMap *tcm[5];

    const int n = 1;
    for(int i = 0; i < n; ++i)
    {
        tcm[i] = himmel->getOrCreateTextureCubeMap(i);
        loadCubeMapFromFiles("resources/cube_gen_" + name[i] + "?.jpg", *tcm[i]);

        himmel->pushTextureUnit(i, (float)i * 1.0f / (float)n);
    }
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
    D_CubeMappedHimmel        = 0
};

osg::ref_ptr<osg::Group> g_root;


e_Demo g_demo;
std::map<e_Demo, osg::ref_ptr<AbstractHimmel> > g_himmelsByDemo;


osgViewer::View *g_view = NULL;

const float g_fovBackup = 60.f;
float g_fov = g_fovBackup;


void activateDemo(e_Demo demo)
{
    g_himmelsByDemo[D_CubeMappedHimmel]      ->setNodeMask(D_CubeMappedHimmel       == demo);
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

    g_himmelsByDemo[D_CubeMappedHimmel] = createCubeMappedDemo();
    group->addChild(g_himmelsByDemo[D_CubeMappedHimmel]);

    return group;
}


// render with envmap

#include <osg/TexMat>
#include <osg/TexGenNode>
#include <osg/Material>

osg::Node *createReflector()
{
	osg::Group *group = new osg::Group;
	
	//barrel
	osg::Node *barrel = osgDB::readNodeFile("resources/barrel.obj");
	osg::ref_ptr<osg::MatrixTransform> bTrans = new osg::MatrixTransform;
	osg::Matrix bMat; 
	bMat.makeTranslate(2.0f, 2.0f, 0.0f);
	bTrans->setMatrix(bMat);
	bTrans->addChild(barrel);
	group->addChild(bTrans);

	//monkey
	osg::Node *monkey = osgDB::readNodeFile("resources/models/monkey.obj");
	osg::ref_ptr<osg::MatrixTransform> mTrans = new osg::MatrixTransform;
	osg::Matrix mMat; 
	mMat.makeTranslate(2.0f, 2.0f, 0.0f);
	mTrans->setMatrix(mMat);
	mTrans->addChild(monkey);
	group->addChild(mTrans);

	//cube
	osg::Node *cube = osgDB::readNodeFile("resources/models/cube.obj");
	osg::ref_ptr<osg::MatrixTransform> cTrans = new osg::MatrixTransform;
	osg::Matrix cMat; 
	cMat.makeTranslate(-2.0f, 2.0f, 0.0f);
	cTrans->setMatrix(cMat);
	cTrans->addChild(cube);
	group->addChild(cTrans);

	//sphere
	osg::Node *sphere = osgDB::readNodeFile("resources/models/sphere.obj");
	osg::ref_ptr<osg::MatrixTransform> sTrans = new osg::MatrixTransform;
	osg::Matrix sMat; 
	sMat.makeTranslate(2.0f, -2.0f, 0.0f);
	sTrans->setMatrix(sMat);
	sTrans->addChild(sphere);
	group->addChild(sTrans);

    osg::ref_ptr<osg::Material> m = new osg::Material;
    m->setColorMode(osg::Material::DIFFUSE);
    //m->setAmbient  (osg::Material::FRONT_AND_BACK, osg::Vec4(6.0f, 6.0f, 6.0f, 1.f));

    group->getOrCreateStateSet()->setAttributeAndModes(m.get(), osg::StateAttribute::ON);

	//create Shader for Image Based Lighting
	osg::StateSet *iblState = group->getOrCreateStateSet();
	osg::Program* iblProgram = new osg::Program;

	//Vertex Shader
	osg::Shader* iblVertex = new osg::Shader( osg::Shader::VERTEX );
	if(!( iblVertex->loadShaderSourceFromFile("shaders/ibl.vert") ))
    {
        osg::notify(osg::WARN) << "Vertex Shader \"shaders/ibl.vert\" could not be loaded." << std::endl;
        return NULL;
    }

	//Fragment Shader
	osg::Shader* iblFragment = new osg::Shader( osg::Shader::FRAGMENT );
	if(!( iblFragment->loadShaderSourceFromFile("shaders/ibl.frag") ))
    {
        osg::notify(osg::WARN) << "Fragment Shader \"shaders/ibl.frag\" could not be loaded." << std::endl;
        return NULL;
    }
	
	//attach Shaders
	iblProgram->addShader( iblVertex );
	iblProgram->addShader( iblFragment );

	//Uniforms
	osg::Uniform* cubeMapRes = new osg::Uniform( "cubeMapRes", CUBE_MAP_RES );

	//osg::Uniform* baseColor = new osg::Uniform( "baseColor", osg::Vec3f(.7f, .7f, .7f) );
	osg::Uniform* diffusePercent = new osg::Uniform( "diffusePercent", 0.7f );

	// create unfirom to point to the texture
	osg::Uniform* himmelCube = new osg::Uniform("himmelCube", osg::Uniform::SAMPLER_CUBE);
    himmelCube->set((int)0);

	iblState->setAttributeAndModes(iblProgram, osg::StateAttribute::ON);
	iblState->addUniform(cubeMapRes);
	//iblState->addUniform(baseColor);
	iblState->addUniform(diffusePercent);
	iblState->addUniform(himmelCube);
	
    return group;
}

/*class TexMatCullCallback : public osg::NodeCallback
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
};*/

osg::Group *createScene(
    osg::Node *scene
,   osg::Node *reflector)
{
    const unsigned int unit = 0;

    // create the texgen node to project the tex coords onto the subgraph:
    /*osg::TexGenNode* texgenNode = new osg::TexGenNode;

    texgenNode->getTexGen()->setMode(osg::TexGen::REFLECTION_MAP);
    texgenNode->setTextureUnit(unit);

    texgenNode->addChild(reflector);*/


    HimmelEnvMap *envMap = new HimmelEnvMap(16);
    envMap->addChild(scene);


    osg::Group *group = new osg::Group();
    group->addChild(envMap);
    //group->addChild(texgenNode);
	group->addChild(reflector);

    osg::StateSet* stateset = reflector->getOrCreateStateSet();
    stateset->setTextureAttributeAndModes(unit, envMap->cubeMap(), osg::StateAttribute::ON);
    
	/*
	stateset->setTextureMode(unit, GL_TEXTURE_GEN_S, osg::StateAttribute::ON);
    stateset->setTextureMode(unit, GL_TEXTURE_GEN_T, osg::StateAttribute::ON);
    stateset->setTextureMode(unit, GL_TEXTURE_GEN_R, osg::StateAttribute::ON);
    stateset->setTextureMode(unit, GL_TEXTURE_GEN_Q, osg::StateAttribute::ON);

    osg::TexMat* texmat = new osg::TexMat;
    stateset->setTextureAttributeAndModes(unit, texmat, osg::StateAttribute::ON);
        
    reflector->setCullCallback(new TexMatCullCallback(texmat));*/

    return group;
}

int main(int argc, char* argv[])
{
    osg::ArgumentParser arguments(&argc, argv);
    osgViewer::Viewer viewer(arguments);

    osg::notify(osg::NOTICE) << "Use [1] to [4] to select camera manipulator." << std::endl;
    osg::notify(osg::NOTICE) << "Use [space] to cycle mapping techniques." << std::endl;
    osg::notify(osg::NOTICE) << "Use [p] to pause/unpause time." << std::endl;
    osg::notify(osg::NOTICE) << "Use [r] or [s] to reset or stop the time." << std::endl;
    osg::notify(osg::NOTICE) << "Use [+] and [-] to increase/decrease seconds per cycle." << std::endl;
    osg::notify(osg::NOTICE) << "Use [mouse wheel] to change field of view." << std::endl;

    g_demo = D_CubeMappedHimmel;
    g_view = dynamic_cast<osgViewer::View*>(&viewer);

    viewer.setUpViewInWindow(128, 128, 1280, 720);
    
	g_timef->start();
    

    osg::Camera *cam = g_view->getCamera();
    fovChanged();

    cam->setClearColor(osg::Vec4(0.8, 0.84, 0.9, 1.0));

    g_root  = new osg::Group();
    g_view->setSceneData(g_root.get());
    
    osg::ref_ptr<osg::Group> himmel = createHimmelScene();

    osg::Node *reflector(createReflector());
    osg::ref_ptr<osg::Group> scene = himmel;

    if(reflector)
    {
        scene = createScene(
            himmel.get(), reflector);
    }

    g_root->addChild(scene.get());


    activateDemo(D_CubeMappedHimmel);

    viewer.addEventHandler(new KeyboardEventHandler);




    // 
    int tileWidth = 512, tileHeight = 512;
    int posterWidth = 1024, posterHeight = 1024;
    int numCameras = 2;
    std::string posterName = "poster.bmp", extName = "bmp";
    osg::Vec4 bgColor(0.2f, 0.2f, 0.6f, 1.0f);
    
 
    // Create cameras for rendering tiles offscreen. FrameBuffer is recommended because it requires less memory.
    osg::ref_ptr<osg::Group> cameraRoot = new osg::Group;
    for ( int i=0; i<numCameras; ++i )
    {
        osg::ref_ptr<osg::Camera> camera = new osg::Camera();
        camera->setClearColor(bgColor);
        camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        camera->setRenderOrder(osg::Camera::PRE_RENDER);
        camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER);
        camera->setViewport( 0, 0, tileWidth, tileHeight );
        
        camera->addChild(scene);
        cameraRoot->addChild(camera.get());
    }
    g_root->addChild(cameraRoot.get());

    // Set the printing handler
    PrintPosterHandler* handler = new PrintPosterHandler;
    handler->setTileSize( tileWidth, tileHeight );
    handler->setSize( posterWidth, posterHeight );
    handler->setCameraRoot( cameraRoot.get() );
    
    osg::ref_ptr<osg::Image> posterImage = 0;


    viewer.addEventHandler(handler);
    viewer.setUpViewInWindow( 64, 64, 512, 512);
    
    viewer.run();


    return 0;

}