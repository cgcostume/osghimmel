
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

#include "glslhighlighter.h"

#include "utils/glslsyntax.hpp"

#include <QTextDocument>
#include <QStringListModel>
#include <QColor>


GlslHighlighter::GlslHighlighter()
:   QSyntaxHighlighter(static_cast<QWidget*>(NULL))
,   m_language(GLSLT_Common)
{
}


GlslHighlighter::~GlslHighlighter()
{
}


void GlslHighlighter::createAndAppendRules(
    const QString &pattern
,   QTextCharFormat *format)
{
    m_patternsByFormat[format] << QRegExp(pattern);
}


void GlslHighlighter::setDocument(
    QTextDocument *textdoc
,   const e_GlslLanguageType language)
{
    QSyntaxHighlighter::setDocument(textdoc);

    setLanguage(language);
};


void GlslHighlighter::setLanguage(const e_GlslLanguageType language)
{
    m_language = language;

    qDeleteAll(m_formats);

    m_formats.clear();
    m_patternsByFormat.clear();

    if(GLSLT_Undefined == m_language)
        return;

    QTextCharFormat *f0(new QTextCharFormat());
    f0->setForeground(QColor("#0000ff"));

    QTextCharFormat *f1(new QTextCharFormat());
    f1->setForeground(QColor("#888888"));

    QTextCharFormat *f2(new QTextCharFormat());
    f2->setForeground(QColor("#a000a0"));

    QTextCharFormat *f3(new QTextCharFormat());
    f3->setForeground(QColor("#880000"));

    createAndAppendRules(glsl_syntax_TransparentTypes,              f0);
    createAndAppendRules(glsl_syntax_SamplerTypes,                  f0);
    createAndAppendRules(glsl_syntax_PreprocessorDirectives,        f1);
    createAndAppendRules(glsl_syntax_PredefinedMacros,              f0);
    createAndAppendRules(glsl_syntax_Qualifiers,                    f0);
    createAndAppendRules(glsl_syntax_InterpolationQualifier,        f0);
    createAndAppendRules(glsl_syntax_CompatibilityUniforms,         f2);
    createAndAppendRules(glsl_syntax_ConstantsBuiltIn,              f2);
    createAndAppendRules(glsl_syntax_IterationAndJumps,             f0);
    createAndAppendRules(glsl_syntax_Functions,                     f3);
    createAndAppendRules(glsl_syntax_AngleAndTrigonometryFunctions, f3);
    createAndAppendRules(glsl_syntax_ExponentialFunctions,          f3);
    createAndAppendRules(glsl_syntax_CommonFunctions,               f3);
    createAndAppendRules(glsl_syntax_GeometricFunctions,            f3);
    createAndAppendRules(glsl_syntax_MatrixFunctions,               f3);
    createAndAppendRules(glsl_syntax_VectorRelationalFunctions,     f3);
    createAndAppendRules(glsl_syntax_NoiseFunctions,                f3);
    createAndAppendRules(glsl_syntax_TextureLookupFunctions,        f3);

    switch(m_language)
    {
    case GLSLT_Vertex:
        createAndAppendRules(glsl_syntax_BuiltIn_v,                 f2);
        break;
    case GLSLT_Geometry:
        createAndAppendRules(glsl_syntax_BuiltIn_g,                 f2);
        createAndAppendRules(glsl_syntax_Functions_g,               f3);
        break;
    case GLSLT_Fragment:
        createAndAppendRules(glsl_syntax_BuiltIn_f,                 f2);
        createAndAppendRules(glsl_syntax_IterationAndJumps_f,       f0);
        createAndAppendRules(glsl_syntax_DerivativeFunctions_f,     f3);
        break;
    default:
        break;
    }
}


void GlslHighlighter::highlightBlock(const QString &text)
{
    t_patternsByFormat::const_iterator f(m_patternsByFormat.begin());
    const t_patternsByFormat::const_iterator fe(m_patternsByFormat.end());

    for(; f != fe; ++f)
    {
        const QTextCharFormat &format(*f.key());

        foreach(QRegExp pattern, f.value())
        {
            int index(text.indexOf(pattern));
            while(-1 != index)
            {
                const int length(pattern.matchedLength());
                setFormat(index, length, format);

                index = text.indexOf(pattern, index + length);
            }
        }
    }
}