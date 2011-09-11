
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
#ifndef __COLLAPSIBLEDOCKWIDGET_H__
#define __COLLAPSIBLEDOCKWIDGET_H__


#include "docktitlebarwidget.h"

#include <QDockWidget>
#include <QSet>


class QMainWindow;
class QSettings;


class DockTitleBarWidget;

class CollapsibleDockWidget
:    public QDockWidget
{
    Q_OBJECT

public:
    CollapsibleDockWidget(
        QWidget& widget
    ,   QWidget* parent = NULL
    ,   const bool collapsed = false);
    ~CollapsibleDockWidget();

    const bool isCollapsed() const;

    void saveLayoutState(QSettings &settings);
    void restoreLayoutState(const QSettings &settings);

public slots:
    void toggleCollapse();
    void toggleCollapse(const bool collapse);

    void updateWindowTitle(QString windowTitle);

protected:
    QWidget *_originalWidget;
    QWidget *_emptyWidget;

    DockTitleBarWidget *_titleBar;

    static QSet<CollapsibleDockWidget*> _instances;

protected:
    void setAllowCollapse(const bool allow);

    static void updateCollapsePolicy(QMainWindow& mainWindow);

    // this is for a qdockwidget hot fix
    virtual bool eventFilter(QObject *object, QEvent *event);

protected slots:
    void detach();
    void dockStatusChanged();
    void addTitleBarIfFloating();
};


#endif __COLLAPSIBLEDOCKWIDGET_H__