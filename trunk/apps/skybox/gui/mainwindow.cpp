
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

#include "utils/log.h"
#include "mainlogdispatcher.h"

#include "collapsibledockwidget.h"
#include "datetimewidget.h"
#include "glsleditor.h"
#include "logoutputlabel.h"
#include "logoutputwidget.h"
#include "qosgviewer.h"

#include "propertywidget.h"

#include "scenes/scene_spheremappedhimmel.h"
#include "scenes/scene_cubemappedhimmel.h"

#include "utils/import.h"

#include "include/timef.h"


#include <QFileInfo>
#include <QFileDialog>
#include <QSettings>
#include <QTimer>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/StateSetManipulator>

#include <osgUtil/Optimizer>

#include <osgDB/ReadFile>

#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>

#include <assert.h>

namespace
{
    const QString SETTINGS_GROUP_COMMON_GUI("common");
}


MainWindow::MainWindow(QWidget *parent)
:   QMainWindow(parent)
,   m_ui(new Ui::MainWindow)
,   m_himmel(NULL)
,   m_scene(NULL)
,   m_root(NULL)
,   m_timef(NULL)
,   m_camera(NULL)

,   m_timer(NULL)
,   m_dateTimeLabel(NULL)
,   m_timefLabel(NULL)
,   m_dateTimeWidget(NULL)
,   m_dateTimeDockWidget(NULL)
,   m_glslEditor(NULL)
,   m_glslEditorDockWidget(NULL)
,   m_propertyWidget(NULL)
,   m_propertyDockWidget(NULL)
{
    QCoreApplication::setOrganizationName("dm@g4t3.de");
    QCoreApplication::setApplicationName(APPLICATION_NAME);

    m_ui->setupUi(this);

    setWindowTitle(APPLICATION_NAME " " APPLICATION_VERSION);

    initializeLog();

    const QSize size(m_ui->centralWidget->size());

    initializeScene();
    initializeManipulator(m_ui->centralWidget);

    initializeDockWidgets();

	connect(m_ui->centralWidget, SIGNAL(mouseDrop(QList<QUrl>)),
		this, SLOT(mouseDroped(QList<QUrl>)));
}


MainWindow::~MainWindow()
{
    clearHimmel();

    m_root->removeChild(m_scene);
    m_scene->unref();

    m_ui->centralWidget->setSceneData(NULL);
    m_root->unref();

    delete m_timer;

    delete m_timef;
    m_timef = NULL;

    uninitializeLog();
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


void MainWindow::initializeScene()
{
    m_camera = m_ui->centralWidget->getCamera();
    m_camera->setViewport(new osg::Viewport(
        0, 0, m_ui->centralWidget->width(), m_ui->centralWidget->height()));
    
    m_camera->setProjectionMatrixAsPerspective(
        40.f, static_cast<double>(m_ui->centralWidget->width()) / static_cast<double>(m_ui->centralWidget->height()), 0.1, 8.f);

    m_camera->setClearColor(osg::Vec4(1.f, 1.f, 1.f, 1.f));

    assert(!m_scene);

    m_root  = new osg::Group();
    m_scene = new osg::Group();

    m_root->addChild(m_scene.get());
    m_ui->centralWidget->setSceneData(m_root.get());

    m_timef = new TimeF(time(NULL), 30.f);
}


void MainWindow::himmelChanged()
{
    m_ui->cubeMappedHimmelAction->setChecked(false);
    m_ui->sphereMappedHimmelAction->setChecked(false);
    m_ui->proceduralHimmelAction->setChecked(false);

    if(m_himmel)
    {
        m_himmel->assignTime(m_timef);
        m_root->addChild(m_himmel.get());
    }

    m_propertyWidget->assign(m_himmel);
}


void MainWindow::clearHimmel()
{
    if(m_himmel)
    {
        m_root->removeChild(m_himmel.get());

        m_himmel->unref();
        m_himmel = NULL;
    }
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


void MainWindow::initializeToolBars()
{
}


void MainWindow::initializeDockWidgets()
{
    // initialize further status bar items

    m_dateTimeLabel = new QLabel();
    m_ui->statusBar->addWidget(m_dateTimeLabel, 2);
    
    m_timefLabel = new QLabel();
    m_ui->statusBar->addWidget(m_timefLabel, 1);


    // setup timer for status bar updates()

    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(me_timeout()));

    m_timer->start(16);

    // initialize widgets

    m_dateTimeWidget = new DateTimeWidget(*m_timef);
    m_dateTimeDockWidget = new CollapsibleDockWidget(*m_dateTimeWidget, this);

    addDockWidget(Qt::RightDockWidgetArea, m_dateTimeDockWidget);

    
    m_propertyWidget = new PropertyWidget();
    m_propertyDockWidget = new CollapsibleDockWidget(*m_propertyWidget, this);

    addDockWidget(Qt::RightDockWidgetArea, m_propertyDockWidget);


    m_glslEditor = new GLSLEditor();
    m_glslEditorDockWidget = new CollapsibleDockWidget(*m_glslEditor, this);

    addDockWidget(Qt::RightDockWidgetArea, m_glslEditorDockWidget);
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


void MainWindow::on_sphereMappedHimmelAction_triggered(bool)
{
    clearHimmel();

    m_himmel = new Scene_SphereMappedHimmel(m_camera);

    himmelChanged();

    m_ui->sphereMappedHimmelAction->setChecked(true);
}


void MainWindow::on_cubeMappedHimmelAction_triggered(bool)
{
    clearHimmel();

    m_himmel = new Scene_CubeMappedHimmel(m_camera);

    himmelChanged();

    m_ui->cubeMappedHimmelAction->setChecked(true);
}


void MainWindow::on_proceduralHimmelAction_triggered(bool)
{
    clearHimmel();

    himmelChanged();

    m_ui->proceduralHimmelAction->setChecked(true);
}


const bool MainWindow::insert3DObjectFromFile(const QFileInfo &fileInfo)
{
    assert(m_scene);

    QString source = fileInfo.absoluteFilePath();
    const std::string c_source = source.toLatin1();

    osg::ref_ptr<osg::Node> loadedScene = osgDB::readNodeFile(c_source);
    if(!loadedScene)
    {
        _LOG_WARNING("Load Drop Data failed", "Loading " + source + " has failed.");
        return false;
    }

    // optimize the scene graph, remove redundant nodes and state etc.
    osgUtil::Optimizer optimizer;
    optimizer.optimize(loadedScene.get());

    m_scene->addChild(loadedScene.get());
    return true;
}


void MainWindow::remove3DObjectsFromScene()
{
    assert(m_scene);

    m_scene->removeChildren(0, m_scene->getNumChildren());
}


void MainWindow::on_insert3DObjectFromFileAction_triggered(bool)
{
    static const QString SETTINGS_KEY_LAST_PATH("Last3DObjectFromFilePath");

    QSettings s;
    s.beginGroup(SETTINGS_GROUP_COMMON_GUI);
    QString last3DObjectFromFilePath(s.value(SETTINGS_KEY_LAST_PATH).toString());
    s.endGroup();

    const QStringList fileNames(QFileDialog::getOpenFileNames(
        this, TR("Insert 3D Objects from File(s)"), last3DObjectFromFilePath));

    foreach(const QString &fileName, fileNames)
    {
        if(insert3DObjectFromFile(QFileInfo(fileName)))
            last3DObjectFromFilePath = QFileInfo(fileName).absolutePath();
    }

    s.beginGroup(SETTINGS_GROUP_COMMON_GUI);
    s.setValue(SETTINGS_KEY_LAST_PATH, last3DObjectFromFilePath);
    s.endGroup();
}


void MainWindow::on_clearSceneAction_triggered(bool)
{
    remove3DObjectsFromScene();
}


void MainWindow::mouseDroped(QList<QUrl> urlList)
{
	for(int i = 0; i < urlList.size(); ++i)
	{
		QFileInfo fileInfo(urlList.at(i).path().right(urlList.at(i).path().length() - 1));
        insert3DObjectFromFile(fileInfo);
	}
}


void MainWindow::me_timeout()
{
    assert(m_timef);

    m_timefLabel->setText(QString::number(m_timef->getf(), 'f', 4));

    const QDateTime dt(QDateTime::fromTime_t(m_timef->gett()));
    m_dateTimeLabel->setText(dt.toString(Qt::ISODate));
}