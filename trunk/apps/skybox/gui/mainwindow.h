
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

#pragma once
#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>

#include <osg/Group>

class LogOutputWidget;
class LogOutputLabel;
class CollapsibleDockWidget;

class Ui_MainWindow;

namespace osgViewer 
{
    class View;
}


class GLSLTextEdit;
class CollapsibleDockWidget;


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
    void initializeToolBars();
    void initializeDockWidgets();

    void initializeManipulator(osgViewer::View *viewer);
    void initializeScene(
        osgViewer::View *view
    ,   const QSize &size);

    virtual void changeEvent(QEvent *event);
    virtual void showEvent(QShowEvent *event);

protected slots:

    // ui
    void on_quitAction_triggered(bool);
    void on_aboutAction_triggered(bool);


private:
    void initializeLog();
    void uninitializeLog();


protected:

    GLSLTextEdit *m_glslEditor;
    CollapsibleDockWidget *m_glslEditorDockWidget;


private:

    std::auto_ptr<Ui_MainWindow> m_ui;
    LogOutputLabel *m_logStatusLabel;

    osg::ref_ptr<osg::Group> m_scene;
};


#endif // __MAINWINDOW_H__