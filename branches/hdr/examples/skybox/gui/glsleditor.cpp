
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

#include "glsleditor.h"
#include "examples/skybox/ui_glsleditor.h"

#include "utils/shadermodifier.h"

#include "osgHimmel/abstracthimmel.h"

#include <assert.h>


using namespace osgHimmel;


GlslEditor::GlslEditor(QWidget* parent)
:   QWidget(parent)
,   m_ui(new Ui_GlslEditor)
,   m_shaderModifier(NULL)
,   m_activeDocument(NULL)
{
    m_ui->setupUi(this);
}


GlslEditor::~GlslEditor()
{    
    //m_ui->glslTextEdit->setDocument(NULL); -> crash in QSyntaxHighlighter
    foreach(QTextDocument *textdoc, m_documentsByIdentifier)
        delete textdoc;

    m_documentsByIdentifier.clear();
    m_sourcesByIdentifier.clear();
}


void GlslEditor::assign(ShaderModifier *shaderModifier)
{
    if(m_shaderModifier)
        m_shaderModifier->unregisterIdentifiersChangedCallback(this);

    m_shaderModifier = shaderModifier;

    if(m_shaderModifier)
    {
        m_shaderModifier->registerIdentifiersChangedCallback(
            this, GlslEditor::wrapped_identifiersChanged);

        identifiersChanged();
    }
}


void GlslEditor::wrapped_identifiersChanged(void *object)
{
    GlslEditor *glsle(static_cast<GlslEditor*>(object));
    if(glsle)
        glsle->identifiersChanged();
}


void GlslEditor::identifiersChanged()
{
    const ShaderModifier::t_identifiers identifiers(m_shaderModifier->getIdentifiers());

    ShaderModifier::t_identifiers::const_iterator i(identifiers.begin());
    const ShaderModifier::t_identifiers::const_iterator iEnd(identifiers.end());

    // Use this list to retrieve missing identifiers for removal.
    QSet<QString> missingInAction;
    foreach(const QString &key, m_sourcesByIdentifier.keys())
        missingInAction << key;

    for(; i != iEnd; ++i)
    {
        const QString identifier(QString::fromStdString(*i));

        if(!isRegistered(identifier))
        {
            const QString source(QString::fromStdString(m_shaderModifier->getSource(*i)));
            registerIdentifierWithSource(identifier, source);

            // if this is the first shader, load it!
            if(1 == m_sourcesByIdentifier.size())
                loadShaderFromIdentifier(identifier);
        }
        missingInAction.remove(identifier);
    }

    // Remove all shaders without active identifier.
    foreach(const QString &identifier, missingInAction)
        unregisterIdentiferAndSource(identifier);
}


const bool GlslEditor::isRegistered(const QString &identifier) const
{
    return m_sourcesByIdentifier.contains(identifier);
}


void GlslEditor::registerIdentifierWithSource(
    const QString &identifier
,   const QString &source)
{
    if(isRegistered(identifier))
        return;

    // Save a copy of each NEW shaders source for restore.
    m_sourcesByIdentifier[identifier] = source;

    // Only add shaders with NEW (unknown) identifiers.
    // On source modification, text will be change (identifier*), 
    // so remember original identifier in user data.

    m_ui->shaderComboBox->blockSignals(true);
    m_ui->shaderComboBox->addItem(identifier, identifier);
    m_ui->shaderComboBox->blockSignals(false);
}


void GlslEditor::unregisterIdentiferAndSource(const QString &identifier)
{
    const int index(m_ui->shaderComboBox->findData(identifier));

    assert(index != -1);

    if(m_activeDocument == m_documentsByIdentifier[identifier])
        loadNullDocument();

    m_sourcesByIdentifier.remove(identifier);

    delete m_documentsByIdentifier[identifier];
    m_documentsByIdentifier.remove(identifier);

    m_ui->shaderComboBox->removeItem(index);
}


void GlslEditor::loadShaderFromIdentifier(const QString &identifier)
{
    if(identifier.isEmpty())
    {
        loadNullDocument();
        return;
    }

    if(m_documentsByIdentifier.contains(identifier)
    && m_activeDocument == m_documentsByIdentifier[identifier])
        return;

    assert(m_shaderModifier);
    assert(m_sourcesByIdentifier.contains(identifier));

    if(!m_documentsByIdentifier.contains(identifier))
    {
        // create new TextDocument for GlslTextEdit
        QTextDocument *textdoc(m_ui->glslTextEdit->createDocument());
        textdoc->setPlainText(m_sourcesByIdentifier[identifier]);

        m_documentsByIdentifier[identifier] = textdoc;
    }

    // get notified on content changes

    m_activeDocument = m_documentsByIdentifier[identifier];

    connect(m_activeDocument, SIGNAL(contentsChanged()), 
        this, SLOT(documentContentsChanged()));

    // setup glslTextEdit

    const e_GlslLanguageType t(osgShaderTypeToLanguageType(
        m_shaderModifier->getType(identifier.toStdString())));

    m_ui->glslTextEdit->setDocument(m_activeDocument, t);
}


void GlslEditor::on_shaderComboBox_currentIndexChanged(const int index)
{
    const QString identifier(m_ui->shaderComboBox->itemData(index).toString());
    loadShaderFromIdentifier(identifier);
}


const e_GlslLanguageType GlslEditor::osgShaderTypeToLanguageType(const osg::Shader::Type &type)
{
    switch(type)
    {
    case osg::Shader::VERTEX:
        return GLSLT_Vertex;
    case osg::Shader::GEOMETRY:
        return GLSLT_Geometry;
    case osg::Shader::FRAGMENT:
        return GLSLT_Fragment;
    case osg::Shader::UNDEFINED:
    default:
        return GLSLT_Undefined;
    }
}


void GlslEditor::documentContentsChanged()
{
    assert(m_activeDocument);

    const QString identifier(m_documentsByIdentifier.key(m_activeDocument));

    const bool modified = QString::fromStdString(m_shaderModifier->getSource(identifier.toStdString()))
        != m_activeDocument->toPlainText();
    const bool restorable = m_sourcesByIdentifier[identifier]
        != m_activeDocument->toPlainText();

    m_ui->restorePushButton->setEnabled(restorable);
    m_ui->applyPushButton->setEnabled(modified);

    if(modified != m_modifiedByIdentifier[identifier])
    {
        const int index(m_ui->shaderComboBox->findData(identifier));
        m_ui->shaderComboBox->setItemText(index, identifier + (modified ? "*" : ""));

        m_modifiedByIdentifier[identifier] = modified;
    }
}


void GlslEditor::loadNullDocument()
{
    m_ui->glslTextEdit->setDocument(NULL);

    m_ui->restorePushButton->setEnabled(false);
    m_ui->applyPushButton->setEnabled(false);

    if(m_activeDocument)
        disconnect(m_activeDocument, SIGNAL(contentsChanged()), 
            this, SLOT(documentContentsChanged()));

    m_activeDocument = NULL;
}


void GlslEditor::on_applyPushButton_clicked(bool)
{
    assert(m_shaderModifier);
    assert(m_activeDocument);

    const QString identifier(m_documentsByIdentifier.key(m_activeDocument));
    const QString source(m_activeDocument->toPlainText());

    m_shaderModifier->setSource(identifier.toStdString(), source.toStdString());

    documentContentsChanged();
}


void GlslEditor::on_restorePushButton_clicked(bool)
{
    assert(m_activeDocument);
    assert(m_shaderModifier);

    const QString identifier(m_documentsByIdentifier.key(m_activeDocument));
    const QString source(m_sourcesByIdentifier[identifier]);

    m_activeDocument->setPlainText(source);
    m_shaderModifier->setSource(identifier.toStdString(), source.toStdString());

    documentContentsChanged();
}
