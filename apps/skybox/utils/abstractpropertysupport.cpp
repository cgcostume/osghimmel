
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

#include "abstractpropertysupport.h"

#include <qtpropertybrowser/QtProperty>
#include <qtpropertybrowser/QtAbstractPropertyBrowser>

#include <qtpropertybrowser/QtGroupPropertyManager>
#include <qtpropertybrowser/QtDoublePropertyManager>
#include <qtpropertybrowser/QtSizeFPropertyManager>
#include <qtpropertybrowser/QtBoolPropertyManager>
#include <qtpropertybrowser/QtIntPropertyManager>
#include <qtpropertybrowser/QtEnumPropertyManager>
#include <qtpropertybrowser/QtColorPropertyManager>

#include <qtpropertybrowser/QtEnumEditorFactory>
#include <qtpropertybrowser/QtDoubleSpinBoxFactory>
#include <qtpropertybrowser/QtColorEditorFactory>


AbstractPropertySupport::AbstractPropertySupport()
:   m_propertiesInitialized(false)
,   m_groupManager(NULL)
,   m_intManager(NULL)
,   m_doubleManager(NULL)
,   m_sizeFManager(NULL)
,   m_boolManager(NULL)
,   m_enumManager(NULL)
,   m_enumFactory(NULL)
,   m_boolFactory(NULL)
,   m_intFactory(NULL)
,   m_doubleFactory(NULL)
,   m_sizeFactory(NULL)
{
}


AbstractPropertySupport::~AbstractPropertySupport()
{
    QSet<QtAbstractPropertyBrowser*> pbs(m_propertyBrowsers);   // copy, since clear removes it from set

    foreach(QtAbstractPropertyBrowser *propertyBrowser, pbs)
        clearPropertyBrowser(propertyBrowser);

    assert(m_propertyBrowsers.isEmpty());

    delete m_groupManager;
    delete m_intManager;
    delete m_doubleManager;
    delete m_sizeFManager;
    delete m_boolManager;
    delete m_enumManager;

    delete m_enumFactory;
    delete m_boolFactory;
    delete m_intFactory;
    delete m_doubleFactory;
    delete m_sizeFactory;
}



QtProperty *AbstractPropertySupport::property(const QString &name) const
{
    assert(m_properties.contains(name));
    return m_properties[name];
}


QtIntPropertyManager *AbstractPropertySupport::propertyIntManager() const
{
    return m_intManager;
}


QtDoublePropertyManager *AbstractPropertySupport::propertyDoubleManager() const
{
    return m_doubleManager;
}


QtBoolPropertyManager *AbstractPropertySupport::propertyBoolManager() const
{
    return m_boolManager;
}


QtProperty *AbstractPropertySupport::createGroup(const QString &groupName)
{
    assert(m_groupManager);

    QtProperty *group = m_groupManager->addProperty(groupName);
    m_rootGroups.append(group);

    return group;
}


QtProperty *AbstractPropertySupport::createGroup(QtProperty &group, const QString &groupName)
{
    assert(m_groupManager);

    QtProperty *subGroup = m_groupManager->addProperty(groupName);
    group.addSubProperty(subGroup);

    return subGroup;
}


QtProperty *AbstractPropertySupport::createProperty(QtProperty &group, const QString &propertyName, 
    const int value, const QStringList &values)
{
    m_properties.insert(propertyName, m_enumManager->addProperty(propertyName));

    QtProperty *p = property(propertyName);
    m_enumManager->setEnumNames(p, values);
    m_enumManager->setValue(p, value);

    group.addSubProperty(p);

    return p;
}


QtProperty *AbstractPropertySupport::createProperty(QtProperty &group, const QString &propertyName, 
    const bool value)
{
    m_properties.insert(propertyName, m_boolManager->addProperty(propertyName));

    QtProperty *p = property(propertyName);
    m_boolManager->setValue(p, value);

    group.addSubProperty(p);

    return p;
}


QtProperty *AbstractPropertySupport::createProperty(QtProperty &group, const QString &propertyName, 
    const double value, const double minValue, const double maxValue, const double stepValue)
{
    m_properties.insert(propertyName, m_doubleManager->addProperty(propertyName));

    QtProperty *p = property(propertyName);
    m_doubleManager->setValue(p, value);
    m_doubleManager->setRange(p, minValue, maxValue);
    m_doubleManager->setSingleStep(p, stepValue);

    group.addSubProperty(p);

    return p;
}


QtProperty *AbstractPropertySupport::createProperty(QtProperty &group, const QString &propertyName, 
    const QSizeF &value, const QSizeF &minValue, const QSizeF &maxValue, const QSizeF &stepValue)
{
    m_properties.insert(propertyName, m_sizeFManager->addProperty(propertyName));

    QtProperty *p = property(propertyName);
    m_sizeFManager->setValue(p, value);
    m_sizeFManager->setRange(p, minValue, maxValue);

    m_sizeFManager->subDoublePropertyManager()->setSingleStep(p->subProperties()[0], stepValue.width());
    m_sizeFManager->subDoublePropertyManager()->setSingleStep(p->subProperties()[1], stepValue.height());

    group.addSubProperty(p);

    return p;
}


QtProperty *AbstractPropertySupport::createProperty(
    QtProperty &group
,    const QString &propertyName
,    const int value
,    const int minValue
,    const int maxValue
,    const int stepValue)
{
    m_properties.insert(propertyName, m_intManager->addProperty(propertyName));

    QtProperty *p = property(propertyName);
    m_intManager->setValue(p, value);
    m_intManager->setRange(p, minValue, maxValue);
    m_intManager->setSingleStep(p, stepValue);

    group.addSubProperty(p);

    return p;
}


QtProperty *AbstractPropertySupport::createProperty(
    QtProperty &group
,   const QString &propertyName
,   const QColor &value)
{
    m_properties.insert(propertyName, m_colorManager->addProperty(propertyName));

    QtProperty *p = property(propertyName);
    m_colorManager->setValue(p, value);

    QtIntPropertyManager *intManager(m_colorManager->subIntPropertyManager());
    intManager->setRange(p, 0, 255);
    intManager->setSingleStep(p, 1);

    group.addSubProperty(p);

    return p;
}


void AbstractPropertySupport::registerForFastAccess(const QString &name)
{
    m_fastAccessProperties.append(property(name));
}


void AbstractPropertySupport::initializeProperties()
{
    if(!m_propertiesInitialized)
    {
        m_propertiesInitialized = true;

        m_groupManager = new QtGroupPropertyManager();
        m_intManager = new QtIntPropertyManager();
        m_doubleManager = new QtDoublePropertyManager();
        m_sizeFManager = new QtSizeFPropertyManager();
        m_boolManager = new QtBoolPropertyManager();
        m_enumManager = new QtEnumPropertyManager();
        m_colorManager = new QtColorPropertyManager();

        m_enumFactory = new QtEnumEditorFactory();
        m_boolFactory = new QtCheckBoxFactory();
        m_intFactory = new QtSpinBoxFactory();
        m_doubleFactory = new QtDoubleSpinBoxFactory();
        m_sizeFactory = new QtDoubleSpinBoxFactory();
        m_colorFactory = new QtColorEditorFactory();

        registerProperties();
    }
}


void AbstractPropertySupport::floodPropertyBrowser(QtAbstractPropertyBrowser *propertyBrowser)
{
    assert(m_propertiesInitialized);

    if(!propertyBrowser)
        return;

    m_propertyBrowsers.insert(propertyBrowser);

    connect(m_enumManager, SIGNAL(valueChanged(QtProperty*, int)),
        this, SLOT(on_propertyChanged(QtProperty*)));
    connect(m_boolManager, SIGNAL(valueChanged(QtProperty*, bool)),
        this, SLOT(on_propertyChanged(QtProperty*)));
    connect(m_intManager, SIGNAL(valueChanged(QtProperty*, int)),
        this, SLOT(on_propertyChanged(QtProperty*)));
    connect(m_doubleManager, SIGNAL(valueChanged(QtProperty*, double)),
        this, SLOT(on_propertyChanged(QtProperty*)));
    connect(m_sizeFManager, SIGNAL(valueChanged(QtProperty*, QSizeF)),
        this, SLOT(on_propertyChanged(QtProperty*)));
    connect(m_colorManager, SIGNAL(valueChanged(QtProperty*, QColor)),
        this, SLOT(on_propertyChanged(QtProperty*)));

    propertyBrowser->setFactoryForManager(m_enumManager, m_enumFactory);
    propertyBrowser->setFactoryForManager(m_boolManager, m_boolFactory);
    propertyBrowser->setFactoryForManager(m_intManager, m_intFactory);
    propertyBrowser->setFactoryForManager(m_doubleManager, m_doubleFactory);
    propertyBrowser->setFactoryForManager(m_sizeFManager->subDoublePropertyManager(), m_sizeFactory);
    propertyBrowser->setFactoryForManager(m_colorManager, m_colorFactory);

    // populate
    foreach(QtProperty *p, m_rootGroups)
        propertyBrowser->addProperty(p);
}


void AbstractPropertySupport::clearPropertyBrowser(QtAbstractPropertyBrowser *propertyBrowser)
{
    assert(m_propertiesInitialized);

    if(!propertyBrowser)
        return;

    m_propertyBrowsers.remove(propertyBrowser);

    propertyBrowser->unsetFactoryForManager(m_enumManager);
    propertyBrowser->unsetFactoryForManager(m_boolManager);
    propertyBrowser->unsetFactoryForManager(m_intManager);
    propertyBrowser->unsetFactoryForManager(m_doubleManager);
    propertyBrowser->unsetFactoryForManager(m_sizeFManager->subDoublePropertyManager());
    propertyBrowser->unsetFactoryForManager(m_colorManager->subIntPropertyManager());

    propertyBrowser->clear();
}


const QList<QtProperty*> &AbstractPropertySupport::fastAccessProperties() const
{
    return m_fastAccessProperties;
}


void AbstractPropertySupport::on_propertyChanged(QtProperty *p)
{
    propertyChanged(p, p->propertyName());
}


const double AbstractPropertySupport::doubleValue(const QString &name) const
{
    assert(m_properties.contains(name));
    return m_doubleManager->value(m_properties[name]);
}

void AbstractPropertySupport::setDoubleValue(
    const QString &name
,   const double value)
{
    assert(m_properties.contains(name));
    m_doubleManager->setValue(m_properties[name], value);
}


const int AbstractPropertySupport::intValue(const QString &name) const
{
    assert(m_properties.contains(name));
    return m_intManager->value(m_properties[name]);
}

void AbstractPropertySupport::setIntValue(
    const QString &name
,   const int value)
{
    assert(m_properties.contains(name));
    m_intManager->setValue(m_properties[name], value);
}


const int AbstractPropertySupport::enumValue(const QString &name) const
{
    assert(m_properties.contains(name));
    return m_enumManager->value(m_properties[name]);
}

void AbstractPropertySupport::setEnumValue(
    const QString &name
,   const int value)
{
    assert(m_properties.contains(name));
    m_enumManager->setValue(m_properties[name], value);
}


const bool AbstractPropertySupport::boolValue(const QString &name) const
{
    assert(m_properties.contains(name));
    return m_boolManager->value(m_properties[name]);
}

void AbstractPropertySupport::setBoolValue(
    const QString &name
,   const bool value)
{
    assert(m_properties.contains(name));
    m_boolManager->setValue(m_properties[name], value);
}


const QSizeF AbstractPropertySupport::sizeFValue(const QString &name) const
{
    assert(m_properties.contains(name));
    return m_sizeFManager->value(m_properties[name]);
}

void AbstractPropertySupport::setSizeFValue(
    const QString &name
,   const QSizeF &value)
{
    assert(m_properties.contains(name));
    m_sizeFManager->setValue(m_properties[name], value);
}


const QColor AbstractPropertySupport::colorValue(const QString &name) const
{
    assert(m_properties.contains(name));
    return m_colorManager->value(m_properties[name]);
}

void AbstractPropertySupport::setColorValue(
    const QString &name
,   const QColor &value)
{
    assert(m_properties.contains(name));
    m_colorManager->setValue(m_properties[name], value);
}