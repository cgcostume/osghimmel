
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

#pragma once
#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>

class Ui_MainWindow;

#include <osg/ref_ptr>

#include <QUrl>
#include <QList>

class QFileInfo;
class QLabel;
class QTimer;
class QOsgEventHandler;

class LogOutputWidget;
class LogOutputLabel;
class CollapsibleDockWidget;
class DateTimeWidget;
class GlslEditor;
class PropertyWidget;
class AbstractHimmelScene;
class TimeF;

namespace osgViewer 
{
    class View;
}

namespace osg
{
    class Group;
    class Camera;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    
    // dock widgets
    LogOutputWidget *m_logWidget;
    CollapsibleDockWidget *m_logDockWidget;

protected:
    void initializeScene();

    void initializeToolBars();
    void initializeDockWidgets();

    void initializeManipulator(osgViewer::View *viewer);

    virtual void changeEvent(QEvent *event);
    virtual void showEvent(QShowEvent *event);

    void clearHimmel();
    void himmelChanged();

    const bool insert3DObjectFromFile(const QFileInfo &fileInfo);
    void remove3DObjectsFromScene();

protected slots:

    void mouseDroped(QList<QUrl> urlList);

    void me_timeout();
    
    void setCameraFov(float fov);
    void hintViewSize(
        unsigned int width
    ,   unsigned int height);

    // ui
    void on_quitAction_triggered(bool);
    void on_aboutAction_triggered(bool);

    void on_polarMappedHimmelAction_triggered(bool);
    void on_cubeMappedHimmelAction_triggered(bool);
    void on_paraboloidMappedHimmelAction_triggered(bool);
    void on_sphereMappedHimmelAction_triggered(bool);
    
    void on_proceduralHimmelAction_triggered(bool);

    void on_insert3DObjectFromFileAction_triggered(bool);
    void on_clearSceneAction_triggered(bool);


private:
    void initializeLog();
    void uninitializeLog();

protected:

    QTimer *m_timer;

    QLabel *m_dateTimeLabel;
    QLabel *m_timefLabel;

    DateTimeWidget *m_dateTimeWidget;
    CollapsibleDockWidget *m_dateTimeDockWidget;

    GlslEditor *m_glslEditor;
    CollapsibleDockWidget *m_glslEditorDockWidget;

    PropertyWidget *m_propertyWidget;
    CollapsibleDockWidget *m_propertyDockWidget;

private:

    std::auto_ptr<Ui_MainWindow> m_ui;
    LogOutputLabel *m_logStatusLabel;

    TimeF *m_timef;

    osg::ref_ptr<AbstractHimmelScene> m_himmel;

    osg::ref_ptr<osg::Group> m_root;
    osg::ref_ptr<osg::Group> m_scene;

    osg::Camera *m_camera;

    QOsgEventHandler *m_eventHandler;
};


#endif // __MAINWINDOW_H__