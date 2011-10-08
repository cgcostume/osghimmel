
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

#include "mainwindow.h"
#include "apps/skybox/ui_mainwindow.h"

#include "mainlogdispatcher.h"

#include "collapsibledockwidget.h"
#include "glsltextedit.h"
#include "logoutputlabel.h"
#include "logoutputwidget.h"
#include "qosgviewer.h"

#include "utils/import.h"

#include <QFileInfo>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/StateSetManipulator>

#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>


MainWindow::MainWindow(QWidget *parent)
:   QMainWindow(parent)
,   m_ui(new Ui::MainWindow)
,   m_scene(NULL)

,   m_glslEditor(NULL)
,   m_glslEditorDockWidget(NULL)
{
    QCoreApplication::setOrganizationName("dm@g4t3.de");
    QCoreApplication::setApplicationName(APPLICATION_NAME);

    m_ui->setupUi(this);

    setWindowTitle(APPLICATION_NAME " " APPLICATION_VERSION);

    initializeLog();

    const QSize size(m_ui->centralWidget->size());

    initializeScene(m_ui->centralWidget, size);
    initializeManipulator(m_ui->centralWidget);

    initializeDockWidgets();
}


MainWindow::~MainWindow()
{
    uninitializeLog();

    delete m_glslEditor;
    delete m_glslEditorDockWidget;
}


void MainWindow::initializeLog()
{
    MainLogDispatcher *logDispatcher = dynamic_cast<MainLogDispatcher*>(_LOG->dispatcher());
    assert(logDispatcher);

    m_logWidget = new LogOutputWidget();
    m_logWidget->setObjectName("Log");
    m_logWidget->setWindowTitle("Log");
    m_logDockWidget = new CollapsibleDockWidget(*m_logWidget , this);
    this->addDockWidget(Qt::BottomDockWidgetArea, m_logDockWidget );

    logDispatcher->setOutputWidget(m_logWidget);

    m_logStatusLabel = new LogOutputLabel(m_logDockWidget);
    logDispatcher->setOutputLabel(m_logStatusLabel);

    m_ui->statusBar->addWidget(m_logStatusLabel, 8);
}


void MainWindow::uninitializeLog()
{
    MainLogDispatcher *logDispatcher = dynamic_cast<MainLogDispatcher*>(_LOG->dispatcher());
    assert(logDispatcher);

    logDispatcher->setOutputWidget();
    logDispatcher->setOutputLabel();

    delete m_logWidget;
    delete m_logDockWidget;

    delete m_logStatusLabel;
}


void MainWindow::initializeManipulator(osgViewer::View *view)
{
    // set up the camera manipulators.
    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

    keyswitchManipulator->addMatrixManipulator('1', "Trackball", new osgGA::TrackballManipulator());
    keyswitchManipulator->addMatrixManipulator('2', "Flight",    new osgGA::FlightManipulator());
    keyswitchManipulator->addMatrixManipulator('3', "Drive",     new osgGA::DriveManipulator());
    keyswitchManipulator->addMatrixManipulator('4', "Terrain",   new osgGA::TerrainManipulator());

    m_ui->centralWidget->setCameraManipulator(keyswitchManipulator.get());

    m_ui->centralWidget->addEventHandler(new osgViewer::StatsHandler);
    m_ui->centralWidget->addEventHandler(new osgViewer::ThreadingHandler);
}


void MainWindow::initializeScene(
    osgViewer::View *view
,   const QSize &size)
{
    osg::Camera* cam = view->getCamera();

	cam->setViewport(new osg::Viewport(0, 0, size.width(), size.height()));
	cam->setProjectionMatrixAsPerspective(
		40.0f, static_cast<double>(size.width()) / static_cast<double>(size.height()), 0.1f, 8.0f);

	cam->setClearColor(osg::Vec4(1.f, 1.f, 1.f, 1.f));

	osg::ref_ptr<osg::Viewport> viewport = cam->getViewport();

	osg::ref_ptr<osg::Group> root = new osg::Group;

	m_scene = new osg::Group;
	root->addChild(m_scene);

	view->setSceneData(root.get());

//    osg::ref_ptr<osg::Node> node = importAndOptimizeNodeFromFile(QFileInfo("resources/farmhouse.3ds"));
  //  m_scene->addChild(node);
}


void MainWindow::initializeToolBars()
{
}

QTextDocument *g_document;

void MainWindow::initializeDockWidgets()
{
    m_glslEditor = new GLSLTextEdit();
    m_glslEditorDockWidget = new CollapsibleDockWidget(*m_glslEditor, this);

    addDockWidget(Qt::RightDockWidgetArea, m_glslEditorDockWidget);

    g_document = new QTextDocument();

    m_glslEditor->setDocument(g_document, GLSL_FRAGMENT);
}


void MainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);

    switch(event->type())
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;

    default:
        break;
    }
}


void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
}


void MainWindow::on_quitAction_triggered(bool)
{
    close();
}


void MainWindow::on_aboutAction_triggered(bool)
{
}