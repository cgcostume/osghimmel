
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

#include "logoutputwidget.h"

LogOutputWidget::LogOutputWidget(QWidget* parent)
:   LogOutput()
,   QTextBrowser(parent)
{
    setTextInteractionFlags(Qt::TextBrowserInteraction);
    setReadOnly(true);
    setOpenLinks(false);
    setOpenExternalLinks(false);

    m_cursor = new QTextCursor(this->textCursor());
    m_swap = new QTextEdit();

    update();
    clear();

    setCursor(Qt::IBeamCursor);

    setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
}


LogOutputWidget::~LogOutputWidget()
{
    delete m_cursor;
    delete m_swap;
}


void LogOutputWidget::setColor(const LogEntry::e_LogEntryType type, const QColor &color)
{
    switch(type)
    {
    case LogEntry::INFORMATION_LOG:
        m_informationCharFormat.setForeground(QBrush(color));
        break;
    case LogEntry::WARNING_LOG:
        m_warningCharFormat.setForeground(QBrush(color));
        break;
    case LogEntry::ERROR_LOG:
        m_errorCharFormat.setForeground(QBrush(color));
        break;
    case LogEntry::DEBUG_LOG:
        m_debugCharFormat.setForeground(QBrush(color));
        break;
    case LogEntry::MESSAGE_LOG:
        m_messageCharFormat.setForeground(QBrush(color));
        break;
    case LogEntry::WORKFLOW_LOG:
    case LogEntry::UNDEFINED_LOG:
    default:
        break;
    };
}




void LogOutputWidget::print(const LogEntry &entry)
{
#ifdef _NDEBUG
    const QString timestamp(entry.timestamp().toString("hh:mm:ss"));
#else
    const QString timestamp(entry.timestamp().toString("hh:mm:ss:zzz"));
#endif
    //const QString timestamp(entry.timestamp().toString("hh:mm:ss"));

    m_swap->clear();
    m_swap->setFont(font());

    QString message = entry.message();

    switch(entry.type())
    {
    case LogEntry::INFORMATION_LOG:
        m_swap->textCursor().insertText(QString("%1: %2\n").arg(timestamp).arg(message), m_informationCharFormat);
        break;
    case LogEntry::WARNING_LOG:
        m_swap->textCursor().insertText(QString("%1: %2\n").arg(timestamp).arg(message), m_warningCharFormat);
        break;
    case LogEntry::ERROR_LOG:
        m_swap->textCursor().insertText(QString("%1: %2\n").arg(timestamp).arg(message), m_errorCharFormat);
        break;
    case LogEntry::DEBUG_LOG:
        m_swap->textCursor().insertText(QString("%1 %2 %3: %4\n").arg(timestamp).arg(entry.file()).arg(entry.line()).arg(message), m_debugCharFormat);
        break;
    case LogEntry::MESSAGE_LOG:
        m_swap->textCursor().insertText(QString("%1: %2\n").arg(timestamp).arg(message), m_messageCharFormat);
        break;
    case LogEntry::EMPTYLINE_LOG:
        m_swap->textCursor().insertText("\n");
        break;
    case LogEntry::WORKFLOW_LOG:
    case LogEntry::UNDEFINED_LOG:
    default:
        break;
    };

    QString htmlMessage = m_swap->toHtml();

    QRegExp reEmail("\\b([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4})\\b");
    QRegExp reLink("\\b((ftp|http|https):\\/\\/(\\w+:{0,1}\\w*@)?(\\S+)(:[0-9]+)?(\\/|\\/([\\w#!:.?+=&%@!\\-\\/]))?)\\b");

    int pos = 0;
    while((pos = reEmail.indexIn(message, pos)) != -1) 
    {
        pos += reEmail.matchedLength();
        const QString email = reEmail.capturedTexts()[0];
        htmlMessage.replace(email, QString("<a href =\"mailto:%1\">%1</a>").arg(email));
    }

    pos = 0;
    while((pos = reLink.indexIn(message, pos)) != -1) 
    {
        pos += reLink.matchedLength();
        const QString link = reLink.capturedTexts()[0];
        htmlMessage.replace(link, QString("<a href =\"%1\">%1</a>").arg(link));
    }

    m_cursor->insertHtml(htmlMessage);

    ensureCursorVisible();
    update();
}