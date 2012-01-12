
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

#include "docktitlebarwidget.h"
#include "apps/skybox/ui_docktitlebarwidget.h"

#include <QDockWidget>
#include <assert.h>


DockTitleBarWidget::DockTitleBarWidget(
    QDockWidget *parent
,   const bool useParentIcon)
:   QWidget(parent)
,   m_ui(new Ui_DockTitleBarWidget)
{
    assert(parent);
    m_ui->setupUi(this);

    // 'copy' default title bar
    m_ui->closeToolButton->setIcon(QApplication::style()->standardPixmap(QStyle::SP_TitleBarCloseButton));
    m_ui->detachToolButton->setIcon(QApplication::style()->standardPixmap(QStyle::SP_TitleBarNormalButton));
    m_ui->collapseToolButton->setIcon(QApplication::style()->standardPixmap(QStyle::SP_TitleBarUnshadeButton));

    setWindowTitle(parent->windowTitle());
    if(useParentIcon)
        setWindowIcon(parent->windowIcon());
    else
        m_ui->iconLabel->setVisible(false);

    QPalette p(m_ui->frame->palette());
    p.setColor(QPalette::Window, QColor("#DBDBDB"));
    p.setColor(QPalette::Foreground, QColor("#B9B9B9"));
    m_ui->frame->setPalette(p);

    connect(m_ui->closeToolButton, SIGNAL(clicked(bool)), parent, SLOT(close()));
}


DockTitleBarWidget::~DockTitleBarWidget()
{    
    disconnect(m_ui->closeToolButton, SIGNAL(clicked(bool)), parent(), SLOT(close()));
}


void DockTitleBarWidget::setWindowIcon(const QIcon &icon)
{
    m_ui->iconLabel->setVisible(true);
    m_ui->iconLabel->setPixmap(icon.pixmap(QSize(16, 16)));
    QWidget::setWindowIcon(icon);
}


void DockTitleBarWidget::setWindowTitle(const QString &title)
{
    m_ui->titleLabel->setText(title);
    QWidget::setWindowTitle(title);
}


void DockTitleBarWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(m_ui->collapseToolButton->isEnabled())
        emit collapse();
}


void DockTitleBarWidget::on_collapseToolButton_clicked(bool checked)
{
    emit collapse();
}


void DockTitleBarWidget::on_detachToolButton_clicked(bool checked)
{
    emit detach();
}


void DockTitleBarWidget::setCollapsed(const bool collapsed)
{
    m_ui->collapseToolButton->setIcon(QApplication::style()->standardPixmap(
        collapsed ? QStyle::SP_TitleBarUnshadeButton : QStyle::SP_TitleBarShadeButton));
}


void DockTitleBarWidget::setAllowCollapse(const bool allow)
{
    m_ui->collapseToolButton->setEnabled(allow);
    m_ui->collapseToolButton->setVisible(allow);
}