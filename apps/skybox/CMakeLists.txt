
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

SET(QT_USE_QTOPENGL TRUE)
SET(QT_USE_UI TRUE)
SET(QT_USE_CORE TRUE)

SET(SKYBOX_SOURCES
    pragmanote.h
    mainlogdispatcher.h
    mainlogdispatcher.cpp
    skyboxmain.cpp
    stable.h
    stable.cpp
    gui/collapsibledockwidget.h
    gui/collapsibledockwidget.cpp
    gui/docktitlebarwidget.h
    gui/docktitlebarwidget.cpp
    gui/docktitlebarwidget.ui
    gui/logoutputlabel.h
    gui/logoutputlabel.cpp
    gui/logoutputwidget.h
    gui/logoutputwidget.cpp
    gui/mainwindow.h
    gui/mainwindow.cpp
    gui/mainwindow.ui
    gui/propertysupport.h
    gui/propertysupport.cpp
    gui/qosgviewer.h
    gui/qosgviewer.cpp
    gui/qosgwidget.h
    gui/qosgwidget.cpp
    utils/abstractfactory.hpp
    utils/import.h
    utils/import.cpp
    utils/log.h
    utils/log.cpp)

SOURCE_GROUP_BY_PATH(${CMAKE_CURRENT_SOURCE_DIR} ${SKYBOX_SOURCES})

QT4_AUTO_WRAP(QT4_SOURCES ${SKYBOX_SOURCES})
SETUP_STABLE(${SKYBOX_SOURCES} ${QT4_SOURCES})

INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QT_INCLUDES})
    
IF(MSVC)
    ADD_EXECUTABLE(skybox #WIN32
        ${QT4_SOURCES}
        ${SKYBOX_SOURCES})
ELSE(MSVC)
    ADD_EXECUTABLE(skybox
        ${QT4_SOURCES}
        ${SKYBOX_SOURCES})
ENDIF(MSVC)

TARGET_LINK_LIBRARIES(skybox
    osghimmel
    qtpropertybrowser
    ${QT_QTMAIN_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${OPENSCENEGRAPH_LIBRARIES}
    ${OPENGL_gl_LIBRARY}
    ${OPENGL_glu_LIBRARY})


IF(OPTION_MAKE_PACKAGE)

    INSTALL(TARGETS skybox
        DESTINATION . 
        CONFIGURATIONS Debug Release)

    INSTALL(FILES 
        "${QT_BINARY_DIR}/QtCored${QT_VERSION_MAJOR}.dll" 
        "${QT_BINARY_DIR}/QtGuid${QT_VERSION_MAJOR}.dll" 
        DESTINATION . 
        CONFIGURATIONS Debug)

    INSTALL(FILES 
        "${QT_BINARY_DIR}/QtCore${QT_VERSION_MAJOR}.dll" 
        "${QT_BINARY_DIR}/QtGui${QT_VERSION_MAJOR}.dll" 
        DESTINATION . 
        CONFIGURATIONS Release)

ENDIF(OPTION_MAKE_PACKAGE)

SET_FAST_COMPILE    (skybox)
ADD_POSTFIX_TO_DEBUG(skybox)
