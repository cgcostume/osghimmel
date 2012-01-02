
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

#include "collapsibledockwidget.h"

#include <QMainWindow>
#include <QSettings>
#include <QHBoxLayout>
#include <QEvent>


QSet<CollapsibleDockWidget*> CollapsibleDockWidget::_instances;


CollapsibleDockWidget::CollapsibleDockWidget(
    QWidget &widget
,   QWidget *parent
,   const bool collapsed)
:    
    QDockWidget(widget.windowTitle(), parent)
,    _originalWidget(&widget)
,    _emptyWidget(new QWidget())
{
    _instances.insert(this);

    setObjectName(widget.objectName());
    setWindowIcon(widget.windowIcon());

    // create dock widget titlebar -> used to toggle visibility (button behaviour)
    _titleBar = new DockTitleBarWidget(this);
    setTitleBarWidget(_titleBar);
    _titleBar->setWindowTitle(_originalWidget->windowTitle());

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addSpacerItem(new QSpacerItem(0, 0));

    _originalWidget->setParent(this);

    _emptyWidget->setLayout(layout);
    _emptyWidget->setMaximumHeight(0);
    _emptyWidget->setVisible(false);

    toggleCollapse(collapsed);

    connect(_titleBar, SIGNAL(collapse()), this, SLOT(toggleCollapse()));
    connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(dockStatusChanged()));
    connect(_titleBar, SIGNAL(detach()), this, SLOT(detach()));

    installEventFilter(this);
}


CollapsibleDockWidget::~CollapsibleDockWidget()
{
    _instances.remove(this);
    delete _titleBar;
}


const bool CollapsibleDockWidget::isCollapsed() const
{
    return !_originalWidget->isVisible(); 
}


void CollapsibleDockWidget::toggleCollapse()
{
    toggleCollapse(!isCollapsed());
}


void CollapsibleDockWidget::toggleCollapse(const bool collapse)
{
    setWidget(collapse ? _emptyWidget : _originalWidget);

    _emptyWidget->setVisible(collapse);
    _originalWidget->setVisible(!collapse);

    if(_titleBar)
        _titleBar->setCollapsed(collapse);
}


void CollapsibleDockWidget::updateWindowTitle(QString windowTitle)
{
    if(_titleBar)
        _titleBar->setWindowTitle(windowTitle);
}


void CollapsibleDockWidget::detach()
{
    setFloating(true);
    setTitleBarWidget(NULL);
}


void CollapsibleDockWidget::dockStatusChanged()
{
    QMainWindow *mw = qobject_cast<QMainWindow*>(parentWidget());
    if(mw)
        updateCollapsePolicy(*mw);

    addTitleBarIfFloating();
}


void CollapsibleDockWidget::setAllowCollapse(const bool allow)
{
    if(_titleBar)
        _titleBar->setAllowCollapse(allow);

    if(!allow && isCollapsed())
        toggleCollapse();                
}


void CollapsibleDockWidget::updateCollapsePolicy(QMainWindow& mainWindow)
{
    QSet<QDockWidget*> tabifiedDockWidgets;
    QSet<QDockWidget*> notTabifiedDockWidgets;

    foreach(CollapsibleDockWidget* cdw, _instances)
    {
        notTabifiedDockWidgets << qobject_cast<QDockWidget*>(cdw);
        tabifiedDockWidgets.unite(mainWindow.tabifiedDockWidgets(cdw).toSet());
    }

    notTabifiedDockWidgets.subtract(tabifiedDockWidgets);

    foreach(QDockWidget* dw, tabifiedDockWidgets)
    {
        CollapsibleDockWidget* cdw = qobject_cast<CollapsibleDockWidget*>(dw);
        if(cdw)
            cdw->setAllowCollapse(false);
    }

    foreach(QDockWidget* dw, notTabifiedDockWidgets)
    {
        CollapsibleDockWidget* cdw = qobject_cast<CollapsibleDockWidget*>(dw);
        if(cdw)
            cdw->setAllowCollapse(true);
    }
}


bool CollapsibleDockWidget::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonRelease)
        addTitleBarIfFloating();

    return false; // do not filter any event
}


void CollapsibleDockWidget::addTitleBarIfFloating()
{
    if(isFloating() && titleBarWidget() != NULL)
    {
        if(isCollapsed())
            toggleCollapse();

        setTitleBarWidget(NULL);
    }
    else if(!isFloating() && titleBarWidget() == NULL)
        setTitleBarWidget(_titleBar);
}