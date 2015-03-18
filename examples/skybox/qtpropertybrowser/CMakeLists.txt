
# Copyright (c) 2011-2012, Daniel Müller <dm@g4t3.de>
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

include_directories(
    ${Qt5Core_INCLUDE_DIRS}
	${Qt5OpenGL_INCLUDE_DIRS}
	${Qt5Gui_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR})

set(sources
    qtbuttonpropertybrowser.h
    qteditorfactory.h
    qtgroupboxpropertybrowser.h
    qtpropertybrowser.h
    qtpropertybrowserutils_p.h
    qtpropertymanager.h
    qttreepropertybrowser.h
    qtvariantproperty.h

    qtpropertybrowser.cpp
    qtpropertymanager.cpp
    qteditorfactory.cpp
    qtvariantproperty.cpp
    qttreepropertybrowser.cpp
    qtbuttonpropertybrowser.cpp
    qtgroupboxpropertybrowser.cpp
    qtpropertybrowserutils.cpp)

qt5_generate_moc(qtpropertybrowser.h         ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertybrowser.cpp)
qt5_generate_moc(qtpropertymanager.h         ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertymanager.cpp)
qt5_generate_moc(qteditorfactory.h           ${CMAKE_CURRENT_BINARY_DIR}/moc_qteditorfactory.cpp)
qt5_generate_moc(qtvariantproperty.h         ${CMAKE_CURRENT_BINARY_DIR}/moc_qtvariantproperty.cpp)
qt5_generate_moc(qttreepropertybrowser.h     ${CMAKE_CURRENT_BINARY_DIR}/moc_qttreepropertybrowser.cpp)
qt5_generate_moc(qtbuttonpropertybrowser.h   ${CMAKE_CURRENT_BINARY_DIR}/moc_qtbuttonpropertybrowser.cpp)
qt5_generate_moc(qtgroupboxpropertybrowser.h ${CMAKE_CURRENT_BINARY_DIR}/moc_qtgroupboxpropertybrowser.cpp)
qt5_generate_moc(qtpropertymanager.cpp       ${CMAKE_CURRENT_BINARY_DIR}/qtpropertymanager.moc)
qt5_generate_moc(qteditorfactory.cpp         ${CMAKE_CURRENT_BINARY_DIR}/qteditorfactory.moc)
qt5_generate_moc(qttreepropertybrowser.cpp   ${CMAKE_CURRENT_BINARY_DIR}/qttreepropertybrowser.moc)

set(headers_MOC
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

set_source_files_properties(${headers_MOC} 
    PROPERTIES
    HEADER_FILE_ONLY true)

qt5_generate_moc(qtpropertybrowserutils_p.h  
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertybrowserutils_p.cpp)

add_library(qtpropertybrowser STATIC
    ${extension_MOC}
    ${headers_MOC}
    ${CMAKE_CURRENT_BINARY_DIR}/moc_qtpropertybrowserutils_p.cpp
    ${sources}
    ${resources})

set_target_properties(qtpropertybrowser
	PROPERTIES
	DEBUG_POSTFIX "d${DEBUG_POSTFIX}")		
	
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_DLL)
add_definitions(-DQT_QTPROPERTYBROWSER_EXPORT)
add_definitions(-DQT_PLUGIN)
#add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_STATIC)
