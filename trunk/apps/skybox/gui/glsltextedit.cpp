
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

#include "glsltextedit.h"

#include "glslhighlighter.h"
#include "linenumberarea.h"

#include "utils/glslsyntax.hpp"

#include <QFont>
#include <QCompleter>
#include <QResizeEvent>
#include <QFocusEvent>
#include <QPlainTextDocumentLayout>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QScrollBar>

#include <assert.h>


GLSLTextEdit::GLSLTextEdit(QWidget *parent) 
:   PlainTextWithLineNumberAreaSupport(parent)
,   m_highlighter(NULL)
,   m_completer(NULL)
,   m_lineNumberArea(NULL)
,   m_gotDocumentAssigned(false)
,   m_font(new QFont("Consolas, Courier New, Courier", 9))
{
    setWindowTitle("GLSL-Editor");

    m_highlighter = new GLSLHighlighter();
    m_lineNumberArea = new LineNumberArea(*this);

    connect(this, SIGNAL(cursorPositionChanged()),
        this, SLOT(highlightCurrentLine()));

    highlightCurrentLine();

    setDocument(NULL, UNDEFINED);
}


GLSLTextEdit::~GLSLTextEdit()
{
    delete m_highlighter;
    delete m_completer;

    delete m_lineNumberArea;

    delete m_font;
}



void GLSLTextEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    const QRect cr(contentsRect());

    m_lineNumberArea->setGeometry(
        QRect(cr.left(), cr.top(), m_lineNumberArea->lineNumberAreaWidth(), cr.height()));
}


void GLSLTextEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if(!isReadOnly() && isEnabled()) 
    {
        QTextEdit::ExtraSelection selection;

        selection.format.setBackground(QColor("#f8f8f8"));

        selection.format.setProperty(QTextFormat::FullWidthSelection, true);

        selection.cursor = textCursor();
        selection.cursor.clearSelection();

        extraSelections.append(selection);
    }
    setExtraSelections(extraSelections);
}


void GLSLTextEdit::setDocument(
    QTextDocument *textdoc
,   const e_GlslLanguageType language)
{
    clear();

    if(textdoc)
    {
        textdoc->setDefaultFont(*m_font);
 
        QPlainTextDocumentLayout* layout = new QPlainTextDocumentLayout(textdoc);
        textdoc->setDocumentLayout(layout);
    }

    QPlainTextEdit::setDocument(textdoc);
    setTabStopWidth(QFontMetrics(*m_font).width(QLatin1Char('0')) * 4);

    // setup highlighter

    assert(m_highlighter);
    m_highlighter->setDocument(document(), language);

    // setup completer

    switch(language)
    {
    case GLSL_VERTEX:
        setupCompleter(glsl_complete_common + glsl_complete_v);
        break;
    case GLSL_GEOMETRY:
        setupCompleter(glsl_complete_common + glsl_complete_g);
        break;
    case GLSL_FRAGMENT:
        setupCompleter(glsl_complete_common + glsl_complete_f);
        break;
    default:
        setupCompleter();
    }

    // setup status

    m_gotDocumentAssigned = NULL != document();

    setEnabled(m_gotDocumentAssigned);

    if(!m_gotDocumentAssigned)
        textCursor().insertText("No document assigned.");
}


void GLSLTextEdit::setupCompleter(const QStringList &strings)
{
    delete m_completer;
    m_completer = NULL;

    if(strings.isEmpty())
        return;

    m_completer = new QCompleter(strings, this);

    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseSensitive);

    QObject::connect(m_completer, SIGNAL(activated(const QString&)),
        this, SLOT(insertCompletion(const QString&)));
}


void GLSLTextEdit::insertCompletion(const QString &completion)
{
    assert(m_completer);
    assert(m_completer->widget() == this);

    QTextCursor tc = textCursor();

    int extra = completion.length() - m_completer->completionPrefix().length();

    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);

    tc.insertText(completion.right(extra));

    setTextCursor(tc);
}


void GLSLTextEdit::focusInEvent(QFocusEvent *event)
{
    if (m_completer)
        m_completer->setWidget(this);

    QPlainTextEdit::focusInEvent(event);
}


QString GLSLTextEdit::textUnderCursor() const
{
    QTextCursor tc(textCursor());

    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}


// modified from  http://www.trinitydesktop.org/docs/qt4/tools-customcompleter.html

void GLSLTextEdit::keyPressEvent(QKeyEvent *event)
{
    if(m_completer && m_completer->popup()->isVisible()) 
    {
        // The following keys are forwarded by the completer to the widget.
        switch(event->key()) 
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            event->ignore();
            // Let the completer do default behavior.
            return; 
        default:
            break;
        }
    }

    // [ctrl + e]
    const bool ctrlE(
        (event->modifiers() & Qt::ControlModifier) && Qt::Key_E == event->key());

    // Don't process the shortcut when we have a completer.
    if(!m_completer || !ctrlE) 
        QPlainTextEdit::keyPressEvent(event);

    const bool tab((event->modifiers() == Qt::NoModifier) && Qt::Key_Tab == event->key());
    const bool ctrlOrShift(event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier));

    if(!m_completer || tab || (ctrlOrShift && event->text().isEmpty()))
        return;


    static const QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // End of word.

    const bool modifier((event->modifiers() != Qt::NoModifier) && !ctrlOrShift);

    const QString prefix(textUnderCursor());

    if(!ctrlE && (modifier || event->text().isEmpty() 
    || prefix.length() < 2 || eow.contains(event->text().right(1)))) 
    {
        m_completer->popup()->hide();
        return;
    }

    if(prefix != m_completer->completionPrefix()) 
    {
        m_completer->setCompletionPrefix(prefix);
        m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
    }

    QRect cr(cursorRect());
    cr.setWidth(
        m_completer->popup()->sizeHintForColumn(0) + m_completer->popup()->verticalScrollBar()->sizeHint().width());

    m_completer->popup()->setFont(*m_font);
    m_completer->popup()->setFrameShadow(QFrame::Plain);

    m_completer->complete(cr); // popup it up!
}