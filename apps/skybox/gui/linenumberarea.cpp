
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

#include "linenumberarea.h"
#include "plaintextwithlinenumberareasupport.h"

#include <QPainter>


LineNumberArea::LineNumberArea(PlainTextWithLineNumberAreaSupport &editor)
:   QWidget(&editor) 
,   m_editor(editor)
{
    connect(&m_editor, SIGNAL(blockCountChanged(int)), 
        this, SLOT(updateLineNumberAreaWidth(int)));
    connect(&m_editor, SIGNAL(updateRequest(QRect, int)), 
        this, SLOT(updateLineNumberArea(QRect, int)));
}


QSize LineNumberArea::sizeHint() const 
{
    return QSize(lineNumberAreaWidth(), 0);
}


void LineNumberArea::paintEvent(QPaintEvent *event) 
{
    QPainter painter(this);

    if(m_editor.isEnabled())
        painter.fillRect(event->rect(), Qt::white);

    painter.setPen(QPen(QColor("#f0f0f0"), 1));
    painter.setFont(font());

    painter.drawLine(
        event->rect().right() - 1, event->rect().top()
    ,   event->rect().right() - 1, event->rect().bottom());

    QTextBlock block = m_editor.firstVisibleBlock();
    int blockNumber(block.blockNumber());

    int top = static_cast<int>(m_editor.blockBoundingGeometry(block)
        .translated(m_editor.contentOffset()).top());

    int bottom = top + static_cast<int>(m_editor.blockBoundingRect(block).height());

    painter.setPen(QColor("#2b91af"));

    while(block.isValid() && top <= event->rect().bottom()) 
    {
        if(block.isVisible() && bottom >= event->rect().top()) 
        {
            const QString number(QString::number(blockNumber + 1));

            painter.drawText(
                    0
                ,   top
                ,   width() - 8
                ,   fontMetrics().height()
                ,   Qt::AlignRight
                ,   number);
        }

        block  = block.next();
        top    = bottom;
        bottom = top + static_cast<int>(m_editor.blockBoundingRect(block).height());

        ++blockNumber;
    }
}


const int LineNumberArea::lineNumberAreaWidth() const
{
    const int digits(log10(std::max<float>(1.f, m_editor.blockCount())));

    return 24 + m_editor.fontMetrics().width(QLatin1Char('0')) * digits;
}


void LineNumberArea::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    m_editor.setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}


void LineNumberArea::updateLineNumberArea(
    const QRect &rect
,   const int dy)
{
    if(dy)
        scroll(0, dy);
    else
        m_editor.update(0, rect.y(), width(), rect.height());

    if(rect.contains(m_editor.viewport()->rect()))
        m_editor.setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}
