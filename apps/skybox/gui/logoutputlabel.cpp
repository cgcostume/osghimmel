
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

#include "logoutputlabel.h"

#include <QMouseEvent>
#include <QDockWidget>
#include <QTimer>
#include <QTextDocument>


LogOutputLabel::LogOutputLabel(QDockWidget *output, QWidget* parent)
:    LogOutput()
,    QLabel(parent)
,    m_output(output)
,    m_timer(new QTimer())
,    m_readyShowHelpShown(false)
,    m_readyHideHelpShown(false)
{
    m_ready = readyString();

    if(m_output)
    {
        m_output->hide();
        setToolTip(tr("click to show/hide log"));
    }

    // Interpret all text as plain text so html tags with formatting and line breaks don't cause the status bar to grow in size
    setTextFormat(Qt::PlainText);

    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(timedout()));
    m_timer->setInterval(4000);
    m_timer->setSingleShot(true);
}

LogOutputLabel::~LogOutputLabel()
{
    delete m_timer;
}

void LogOutputLabel::print(const LogEntry &entry)
{
    QString message = entry.message();
    
    message = message.left(message.indexOf("\n"));
    
    switch(entry.type())
    {
    case LogEntry::WARNING_LOG:
        setText(QString("warning: %1").arg(message));
        if(m_output && m_output->isVisible())
            m_timer->start();
        else
            m_timer->stop();

        break;
    case LogEntry::ERROR_LOG:
        setText(QString("error: %1").arg(message));
        if(m_output && m_output->isVisible())
            m_timer->start();
        else
            m_timer->stop();

        break;
    case LogEntry::MESSAGE_LOG:
    case LogEntry::INFORMATION_LOG:
        setText(QString("%1").arg(message));
        m_timer->start();
        break;
    case LogEntry::DEBUG_LOG:
        setText(QString("debug: %1").arg(message));
        m_timer->start();
        break;
    case LogEntry::WORKFLOW_LOG:
    case LogEntry::UNDEFINED_LOG:
    default:
        break;
    };
}

void LogOutputLabel::mousePressEvent(QMouseEvent *event)
{
    if(m_output)
    {
        m_output->setVisible(!m_output->isVisible());
        timedout();
    }

    return QLabel::mousePressEvent(event);
}

void LogOutputLabel::timedout()
{
    setText(readyString());
}

inline const QString LogOutputLabel::readyString()
{
    if(m_output)
    {
        if(m_output->isVisible() && !m_readyHideHelpShown)
        {
            m_readyHideHelpShown = true;
            return QString("ready (click to hide log...)");
        }
        else if(m_output->isHidden() && !m_readyShowHelpShown)
        {
            m_readyShowHelpShown = true;
            return QString("ready (click to show log...)");
        }
    }
    return QString("ready");
}