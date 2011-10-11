
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
#ifndef __ABSTRACTPROPERTYSUPPORT_H__
#define __ABSTRACTPROPERTYSUPPORT_H__


#include "utils/abstractfactory.hpp"

#include <QLabel>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QComboBox>

class QtProperty;
class QtAbstractPropertyBrowser;

class QtGroupPropertyManager;
class QtIntPropertyManager;
class QtDoublePropertyManager;
class QtSizeFPropertyManager;
class QtBoolPropertyManager;
class QtEnumPropertyManager;

class QtEnumEditorFactory;
class QtCheckBoxFactory;
class QtSpinBoxFactory;
class QtDoubleSpinBoxFactory;
class QtDoubleSpinBoxFactory;

class QSvgRenderer;


class AbstractPropertySupport : public QObject
{
    Q_OBJECT

public:
    AbstractPropertySupport();
    ~AbstractPropertySupport();

    // Properties

    void floodPropertyBrowser(QtAbstractPropertyBrowser *propertyBrowser);
    void clearPropertyBrowser(QtAbstractPropertyBrowser *propertyBrowser);

    // Special "fast-access" properties

    const QList<QtProperty*> &fastAccessProperties() const;

    QtIntPropertyManager *propertyIntManager() const;
    QtDoublePropertyManager *propertyDoubleManager() const;
    QtBoolPropertyManager *propertyBoolManager() const;

protected slots:
    void on_propertyChanged(QtProperty*);

protected:
    void initializeProperties();

    virtual void propertyChanged(QtProperty *p) = 0;
    virtual void registerProperties() = 0;

    QtProperty *property(const QString &name) const;

    
    // Groups

    QtProperty *createGroup(
        const QString &groupName);

    QtProperty *createGroup(
        QtProperty &group
    ,   const QString &groupName);


    // Properties

    QtProperty *createProperty(
        QtProperty &group
    ,   const QString &propertyName
    ,   const int value
    ,   const QStringList &values);

    QtProperty *createProperty(
        QtProperty &group
    ,   const QString &propertyName
    ,   const bool value);

    QtProperty *createProperty(
        QtProperty &group
    ,   const QString &propertyName
    ,   const int value
    ,   const int minValue
    ,   const int maxValue
    ,   const int stepValue);

    QtProperty *createProperty(
        QtProperty &group
    ,   const QString &propertyName
    ,   const double value
    ,   const double minValue
    ,   const double maxValue
    ,   const double stepValue);

    QtProperty *createProperty(
        QtProperty &group
    ,   const QString &propertyName 
    ,   const QSizeF &value
    ,   const QSizeF &minValue
    ,   const QSizeF &maxValue
    ,   const QSizeF &stepValue);


    void registerForFastAccess(const QString &name);


    // Helper

    const QSizeF sizeFValue(const QString &name) const;
    void setSizeFValue(
        const QString &name
    ,   const QSizeF &value);

    const double doubleValue(const QString &name) const;
    void setDoubleValue(
        const QString &name
    ,   const double value);

    const int intValue(const QString &name) const;
    void setIntValue(
        const QString &name
    ,   const int value);

    const bool boolValue(const QString &name) const;
    void setBoolValue(
        const QString &name
    ,   const bool value);

    const int enumValue(const QString &name) const;
    void setEnumValue(
        const QString &name
    ,   const int value);


protected:

    bool m_propertiesInitialized;

    QtGroupPropertyManager  *m_groupManager;

    QtIntPropertyManager    *m_intManager;
    QtDoublePropertyManager *m_doubleManager;
    QtSizeFPropertyManager  *m_sizeFManager;
    QtBoolPropertyManager   *m_boolManager;
    QtEnumPropertyManager   *m_enumManager;

    QtEnumEditorFactory     *m_enumFactory;
    QtCheckBoxFactory       *m_boolFactory;
    QtSpinBoxFactory        *m_intFactory;
    QtDoubleSpinBoxFactory  *m_doubleFactory;
    QtDoubleSpinBoxFactory  *m_sizeFactory;

    QList<QtProperty*> m_rootGroups;
    QHash<QString, QtProperty*> m_properties;

    QList<QtProperty*> m_fastAccessProperties;
};

#endif // __ABSTRACTPROPERTYSUPPORT_H__