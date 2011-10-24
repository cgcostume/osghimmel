
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
#ifndef __GLSLTEXTEDIT_H__
#define __GLSLTEXTEDIT_H__

#include "plaintextwithlinenumberareasupport.h"
#include "utils/glsltypes.h"

class GlslHighlighter;
class LineNumberArea;

class QKeyEvent;
class QResizeEvent;
class QFocusEvent;
class QFont;
class QCompleter;


class GlslTextEdit : public PlainTextWithLineNumberAreaSupport
{
    Q_OBJECT

public:
    GlslTextEdit(QWidget *parent = 0);
    virtual ~GlslTextEdit();

    void setDocument(
        QTextDocument *textdoc
    ,   const e_GlslLanguageType language = GLSLT_Undefined);

    QTextDocument *createDocument();

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void focusInEvent(QFocusEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

    void setupCompleter(const QStringList &strings = QStringList());

protected:
    QString textUnderCursor() const;

protected slots:
    void highlightCurrentLine();
    void insertCompletion(const QString &completion);

private:
    QFont *m_font;

    GlslHighlighter *m_highlighter;
    QCompleter *m_completer;

    LineNumberArea *m_lineNumberArea;

    bool m_gotDocumentAssigned;
};

#endif // __GLSLTEXTEDIT_H__