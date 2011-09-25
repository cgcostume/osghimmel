
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
#ifndef __GLSL_HIGHLIGHTER_H__
#define __GLSL_HIGHLIGHTER_H__

#include <QSyntaxHighlighter>

#include <QMap>
#include <QVector>
#include <QTextCharFormat>

#include "utils/glsltypes.h"

class QTextDocument;
class QStringListModel;
class QColor;
class QStringList;


class GLSLHighlighter : public QSyntaxHighlighter
{
public:

    GLSLHighlighter();
    virtual ~GLSLHighlighter();

    void setDocument(
        QTextDocument *textdoc
    ,   const e_GlslLanguageType language);

protected:

    void setLanguage(const e_GlslLanguageType language);

    void createAndAppendRules(
        const QString &pattern
    ,   QTextCharFormat *format);

    // QSyntaxHighlighter interface

    virtual void highlightBlock(const QString &text);

protected:

    typedef QVector<QRegExp> t_regExps;
    typedef QMap<QTextCharFormat*, t_regExps> t_patternsByFormat;

    QVector<QTextCharFormat*> m_formats;
    t_patternsByFormat m_patternsByFormat;    

    e_GlslLanguageType m_language;
};

#endif // __GLSL_HIGHLIGHTER_H__