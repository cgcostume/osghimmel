
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
#ifndef __ABSTRACTFACTORY_HPP__
#define __ABSTRACTFACTORY_HPP__

#include <QStringList>

template<class T>
class AbstractFactory
{
public:
	// factory interface
    static const bool reg(T *object, const QString &name);

    static const QStringList identifiers();
    static T* create(const QString &identifier);

    static const bool knows(const QString &identifier);

protected:
    static QHash<QString, T*> m_registeredObjectsByIdentifier;
};




// AbstractFactory

template<class T>
QHash<QString, T*> AbstractFactory<T>::m_registeredObjectsByIdentifier;


template<class T>
const bool AbstractFactory<T>::reg(T* object, const QString &name)
{
    assert(object);
    assert(!m_registeredObjectsByIdentifier.contains(name));

    m_registeredObjectsByIdentifier.insert(name, object);
    return true;
}


template<class T>
T *AbstractFactory<T>::create(const QString &identifier)
{
    if(m_registeredObjectsByIdentifier.contains(identifier))
	    return m_registeredObjectsByIdentifier[identifier]->copy();
    return NULL;
}


template<class T>
const bool AbstractFactory<T>::knows(const QString &identifier)
{
    return m_registeredObjectsByIdentifier.contains(identifier);
}


template<class T>
const QStringList AbstractFactory<T>::identifiers()
{
    return m_registeredObjectsByIdentifier.keys();
}

#endif __ABSTRACTFACTORY_HPP__