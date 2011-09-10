#
#     _  _ _ |_ . _ _  _ _  _ |
#    (_)_\(_|| ||| | || | |(/_|
#          _|  
#
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
  
CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)

# macros

MACRO(QT4_AUTO_WRAP outfiles)
    FOREACH(fileName ${ARGN})
        IF(fileName MATCHES "\\.h$")
            FILE(STRINGS ${fileName} lines REGEX Q_OBJECT)
            IF(lines)
                SET(moc_headers ${moc_headers} ${fileName})
                MESSAGE(STATUS "moc: ${fileName}")
            ENDIF()
        ENDIF()
        IF(fileName MATCHES "\\.ui$")
            SET(ui_files ${ui_files} ${fileName})
            MESSAGE(STATUS "uic: ${fileName}")
        ENDIF()
        IF(fileName MATCHES "\\.qrc$")
            SET(qrc_files ${qrc_files} ${fileName})
            MESSAGE(STATUS "qrc: ${fileName}")
        ENDIF()
    ENDFOREACH()
    QT4_WRAP_CPP(${outfiles} ${moc_headers})
    QT4_WRAP_UI(${outfiles} ${ui_files})
    QT4_ADD_RESOURCES(${outfiles} ${qrc_files})
ENDMACRO()


MACRO(SOURCE_GROUP_BY_PATH RELATIVE_PARENT_PATH)
	FOREACH(filename ${ARGV})
		GET_FILENAME_COMPONENT(path "${filename}" REALPATH)
		FILE(RELATIVE_PATH path ${RELATIVE_PARENT_PATH} ${path})
		GET_FILENAME_COMPONENT(path "${path}" PATH)
		string(REPLACE "/" "\\" path "${path}")
		IF(${filename} MATCHES "ui_|cxx$")
			SOURCE_GROUP("generated" FILES ${filename})
		ELSEIF(${filename} MATCHES "h$|hpp$|cpp$|c$|cu$|ui$|qrc$")
			SOURCE_GROUP("${path}" FILES ${filename})
		ENDIF()
	ENDFOREACH()
ENDMACRO(SOURCE_GROUP_BY_PATH)


MACRO(SET_WARNING_LEVEL4 target)

	IF(MSVC)
		SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_FLAGS "/W4")
	ENDIF(MSVC)

ENDMACRO(SET_WARNING_LEVEL4 target)

# config

SET(PROJECT_NAME osghimmel)
SET(PROJECT_NAME osghimmel)

PROJECT(${PROJECT_NAME} C CXX)

SET(VERSION_MAJOR "0" CACHE STRING "${PROJECT_NAME} Major Version")
SET(VERSION_MINOR "0" CACHE STRING "${PROJECT_NAME} Minor Version")

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)


OPTION(OPTION_MAKE_DEMOS "Make Demos" ON)
OPTION(OPTION_MAKE_SKYBOX "Make SkyBox - Sandbox for osgHimmel (requires Qt)" OFF)

# packages

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(OpenSceneGraph REQUIRED 
	osgViewer 
    osgUtil 
    osgDB 
    osgGA)

# qt is required for apps only, so appropriate find_package is called there

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/include
    ${OSG_INCLUDE_DIR})

ADD_SUBDIRECTORY("apps")
ADD_SUBDIRECTORY("src")
