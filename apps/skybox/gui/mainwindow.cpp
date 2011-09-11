
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

#include "logoutputlabel.h"
#include "logoutputwidget.h"
#include "collapsibledockwidget.h"


MainWindow::MainWindow(QWidget *parent)
:    QMainWindow(parent)
,    m_ui(new Ui::MainWindow)
{
    QCoreApplication::setOrganizationName("dm@g4t3.de");
    QCoreApplication::setApplicationName(APPLICATION_NAME);

    m_ui->setupUi(this);

    setWindowTitle(APPLICATION_NAME " " APPLICATION_VERSION);

    initializeLog();
}


MainWindow::~MainWindow()
{
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


void MainWindow::initializeToolBars()
{
}


void MainWindow::initializeDockWidgets()
{
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