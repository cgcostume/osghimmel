
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

message(STATUS "add executable: skybox")

find_package(Qt5 CONFIG REQUIRED Core OpenGL Gui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory("qtpropertybrowser")

set(SKYBOX_SOURCES
    abstracthimmelscene.h
    abstracthimmelscene.cpp
    mainlogdispatcher.h
    mainlogdispatcher.cpp
    skyboxmain.cpp
    gui/collapsibledockwidget.h
    gui/collapsibledockwidget.cpp
    gui/datetimewidget.h
    gui/datetimewidget.cpp
    gui/datetimewidget.ui
    gui/docktitlebarwidget.h
    gui/docktitlebarwidget.cpp
    gui/docktitlebarwidget.ui
    gui/glsleditor.h
    gui/glsleditor.cpp
    gui/glsleditor.ui
    gui/glsltextedit.h
    gui/glsltextedit.cpp
    gui/glslhighlighter.h
    gui/glslhighlighter.cpp
    gui/linenumberarea.h
    gui/linenumberarea.cpp
    gui/logoutputlabel.h
    gui/logoutputlabel.cpp
    gui/logoutputwidget.h
    gui/logoutputwidget.cpp
    gui/mainwindow.h
    gui/mainwindow.cpp
    gui/mainwindow.ui
    gui/plaintextwithlinenumberareasupport.h
    gui/propertywidget.h
    gui/propertywidget.cpp
    gui/qosgeventhandler.h
    gui/qosgeventhandler.cpp    
    gui/qosgviewer.h
    gui/qosgviewer.cpp
    gui/qosgwidget.h
    gui/qosgwidget.cpp
    scenes/scene_cubemappedhimmel.h
    scenes/scene_cubemappedhimmel.cpp
    scenes/scene_paraboloidmappedhimmel.h
    scenes/scene_paraboloidmappedhimmel.cpp
    scenes/scene_polarmappedhimmel.h
    scenes/scene_polarmappedhimmel.cpp
    scenes/scene_proceduralhimmel.h
    scenes/scene_proceduralhimmel.cpp
    scenes/scene_spheremappedhimmel.h
    scenes/scene_spheremappedhimmel.cpp
    utils/abstractfactory.hpp
    utils/abstractpropertysupport.h
    utils/abstractpropertysupport.cpp
    utils/glslsyntax.hpp
    utils/glsltypes.h
    utils/import.h
    utils/import.cpp
    utils/log.h
    utils/log.cpp
    utils/qt2osg.h
    utils/qt2osg.cpp
    utils/shadermodifier.h
    utils/shadermodifier.cpp
    utils/tr.h)

source_group_by_path(${CMAKE_CURRENT_SOURCE_DIR} ${SKYBOX_SOURCES})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QT_INCLUDES})
    
add_executable(skybox ${QT4_SOURCES} ${SKYBOX_SOURCES})

target_link_libraries(skybox
    osgHimmel
    qtpropertybrowser
    Qt5::Core
	Qt5::Gui
	Qt5::OpenGL
    ${OPENSCENEGRAPH_LIBRARIES}
    ${OPENGL_gl_LIBRARY}
    ${OPENGL_glu_LIBRARY})

set_target_properties(skybox
	PROPERTIES
	DEBUG_POSTFIX "d${DEBUG_POSTFIX}")	
	
install(TARGETS skybox
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    CONFIGURATIONS Debug Release)

add_definitions("-DOSGHIMMEL_EXPOSE_SHADERS")

add_definitions("-D_CRT_SECURE_NO_WARNINGS")