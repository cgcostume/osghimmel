
# Copyright (c) 2011, Daniel Müller <dm@g4t3.de>
# Computer Graphics Systems Group at the Hasso-Plattner-Institute, Germany
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are met:
#   * Redistributions of source code must retain the above copyright notice, 
#     this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright 
#     notice, this list of conditions and the following disclaimer in the 
#     documentation and/or other materials provided with the distribution.
#   * Neither the name of the Computer Graphics Systems Group at the 
#     Hasso-Plattner-Institute (HPI), Germany nor the names of its 
#     contributors may be used to endorse or promote products derived from 
#     this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
# POSSIBILITY OF SUCH DAMAGE.
  
# project QtPropertyBrowser
# http://qt.gitorious.org/qt-solutions/qt-solutions/trees/master/qtpropertybrowser
# cmake modified from: http://bugreports.qt.nokia.com/browse/QTSOLBUG-80

SET(QT_USE_UI TRUE)
SET(QT_USE_CORE TRUE)
SET(QT_USE_GUI TRUE)

INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(
    ${QT_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR})

SET(sources
    qtpropertybrowser.cpp
    qtpropertymanager.cpp
    qteditorfactory.cpp
    qtvariantproperty.cpp
    qttreepropertybrowser.cpp
    qtbuttonpropertybrowser.cpp
    qtgroupboxpropertybrowser.cpp
    qtpropertybrowserutils.cpp)

QT4_GENERATE_MOC(qtpropertybrowser.h         ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertybrowser.cpp)
QT4_GENERATE_MOC(qtpropertymanager.h         ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertymanager.cpp)
QT4_GENERATE_MOC(qteditorfactory.h           ${CMAKE_CURRENT_BINARY_DIR}/moc_qteditorfactory.cpp)
QT4_GENERATE_MOC(qtvariantproperty.h         ${CMAKE_CURRENT_BINARY_DIR}/moc_qtvariantproperty.cpp)
QT4_GENERATE_MOC(qttreepropertybrowser.h     ${CMAKE_CURRENT_BINARY_DIR}/moc_qttreepropertybrowser.cpp)
QT4_GENERATE_MOC(qtbuttonpropertybrowser.h   ${CMAKE_CURRENT_BINARY_DIR}/moc_qtbuttonpropertybrowser.cpp)
QT4_GENERATE_MOC(qtgroupboxpropertybrowser.h ${CMAKE_CURRENT_BINARY_DIR}/moc_qtgroupboxpropertybrowser.cpp)
QT4_GENERATE_MOC(qtpropertymanager.cpp       ${CMAKE_CURRENT_BINARY_DIR}/qtpropertymanager.moc)
QT4_GENERATE_MOC(qteditorfactory.cpp         ${CMAKE_CURRENT_BINARY_DIR}/qteditorfactory.moc)
QT4_GENERATE_MOC(qttreepropertybrowser.cpp   ${CMAKE_CURRENT_BINARY_DIR}/qttreepropertybrowser.moc)

SET(headers_MOC
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertybrowser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertymanager.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qteditorfactory.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qtvariantproperty.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qttreepropertybrowser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qtbuttonpropertybrowser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qtgroupboxpropertybrowser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/qtpropertymanager.moc
    ${CMAKE_CURRENT_BINARY_DIR}/qteditorfactory.moc
    ${CMAKE_CURRENT_BINARY_DIR}/qttreepropertybrowser.moc
)

SET_SOURCE_FILES_PROPERTIES(${headers_MOC} 
    PROPERTIES
    HEADER_FILE_ONLY true)

QT4_GENERATE_MOC(qtpropertybrowserutils_p.h  
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertybrowserutils_p.cpp)

QT4_ADD_RESOURCES(resources
    qtpropertybrowser.qrc)

ADD_LIBRARY(qtpropertybrowser STATIC
    ${extension_MOC}
    ${headers_MOC}
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertybrowserutils_p.cpp
    ${sources}
    ${resources})

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_DLL)
ADD_DEFINITIONS(-DQT_QTPROPERTYBROWSER_EXPORT)
ADD_DEFINITIONS(-DQT_PLUGIN)
#ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_STATIC)
