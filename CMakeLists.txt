#
#     _  _ _ |_ . _ _  _ _  _ |
#    (_)_\(_|| ||| | || | |(/_|
#          _|  
#
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
  
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# macros

macro(QT4_AUTO_WRAP outfiles)

    set(moc_headers "")
    set(ui_files "")
    set(qrc_files "")

    foreach(fileName ${ARGV})

        if(fileName MATCHES "\\.h$")
            file(STRINGS ${fileName} lines REGEX Q_OBJECT)
            IF(lines)
                list(APPEND moc_headers ${fileName})
                message(STATUS "moc: ${fileName}")
            endif()
        endif()
        
        if(fileName MATCHES "\\.ui$")
            list(APPEND ui_files ${fileName})
            message(STATUS "uic: ${fileName}")
        endif()

        if(fileName MATCHES "\\.qrc$")
            list(APPEND qrc_files ${fileName})
            message(STATUS "qrc: ${fileName}")
        endif()

    endforeach()

    qt4_wrap_cpp(mocGeneratedByHeader ${moc_headers})
    qt4_wrap_ui(uicGenerated ${ui_files})

    qt4_add_resources(mocGeneratedByRessources ${qrc_files})

    list(APPEND ${outfiles}
        ${uicGenerated}
        ${mocGeneratedByHeader} 
        ${mocGeneratedByRessources})

    source_group("_ Qt Generated" FILES 
        ${uicGenerated}
        ${mocGeneratedByHeader}
		${mocGeneratedByRessources})

	source_group("_ CMake Rules" REGULAR_EXPRESSION "rule$")

endmacro()


macro(SOURCE_GROUP_BY_PATH parent_path)

    foreach(filename ${ARGV})
	
        get_filename_component(path "${filename}" REALPATH)
        file(RELATIVE_PATH path ${parent_path} ${path})
        get_filename_component(path "${path}" PATH)
        string(REPLACE "/" "\\" path "${path}")

        if(${filename} MATCHES "h$|hpp$|cpp$|c$|ui$|qrc$")
            source_group("${path}" FILES ${filename})
        endif()

    endforeach()

endmacro()


macro(SET_DEFAULT_TARGET_PROPERTIES target w4)

    if(MSVC)

        if(w4)
            set_target_properties(${target}
                PROPERTIES
                COMPILE_FLAGS "/W4 /fp:fast /Ot /GS-"
                LINK_FLAGS_DEBUG "/NODEFAULTLIB:msvcrt"
                DEBUG_POSTFIX "d${DEBUG_POSTFIX}")
        else(w4)
            set_target_properties(${target}
                PROPERTIES
                COMPILE_FLAGS "/fp:fast /Ot /GS-"
                LINK_FLAGS_DEBUG "/NODEFAULTLIB:msvcrt"
                DEBUG_POSTFIX "d${DEBUG_POSTFIX}")
        endif()

    else(MSVC)

        set_target_properties(${target}
            PROPERTIES
            DEBUG_POSTFIX "d${DEBUG_POSTFIX}")

    endif()

endmacro()

# config

set(PROJECT_NAME osghimmel)
set(PROJECT_NAME osghimmel)

project(${PROJECT_NAME} C CXX)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited configs" FORCE)

set(VERSION_MAJOR "0" CACHE STRING "${PROJECT_NAME} Major Version")
set(VERSION_MINOR "0" CACHE STRING "${PROJECT_NAME} Minor Version")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

option(OPTION_MAKE_DEMOS "Make Demos" ON)
option(OPTION_MAKE_SKYBOX "Make SkyBox - Sandbox for osgHimmel (requires Qt)" ON)
option(OPTION_MAKE_TESTS "Make Tests" ON)

# 3rdp and resources

find_package(OpenGL REQUIRED)
find_package(OpenSceneGraph REQUIRED 
    osgViewer 
    osgUtil 
    osgDB 
    osgGA)

if(OPTION_MAKE_DEMOS OR OPTION_MAKE_SKYBOX)
    
    set(RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../resources" CACHE PATH "Resources for osghimmel binaries.")

    if(NOT EXISTS "${RESOURCE_DIR}/")
        message(WARNING "Resource dir not found, please specify RESOURCE_DIR.")
    endif()

    string(REPLACE "include" "bin" OSG_BINARY_DIR ${OSG_INCLUDE_DIR})

    set(OSG_BINARY_DIR ${OSG_BINARY_DIR} CACHE PATH "Binary directory containing OSG dlls.")
    set(OSG_SOVERSION "80" CACHE STRING "OSG binaries version (osg##.dll).")
    
endif()

if(OPTION_MAKE_SKYBOX)

    set(QT_MIN_VERSION "4.7.0")
    find_package(Qt4 COMPONENTS QtCore QtOpenGL QtGui REQUIRED)
    include(${QT_USE_FILE})

    if(NOT QT4_FOUND)
        message(WARNING "Skybox project will be ignored.")
    endif(NOT QT4_FOUND)

endif()


# lib/bin copy

set(OSG_PLUGIN_DIR "${OSG_BINARY_DIR}/osgPlugins-${OPENSCENEGRAPH_VERSION}")

if(NOT EXISTS ${OSG_PLUGIN_DIR}/)
    message(WARNING "osgPlugins dir not found, please specify OSG_PLUGIN_DIR.")
endif()


# install
   
install(DIRECTORY "${RESOURCE_DIR}" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

if(NOT EXISTS ${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgd.dll)
    message(WARNING "OSG_SOVERSION (${OSG_SOVERSION}) seems wrong. ${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgd.dll not found.")
endif()

install(FILES 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgd.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgUtild.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgDBd.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgGAd.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgTextd.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgViewerd.dll" 
    "${OSG_BINARY_DIR}/ot11-OpenThreadsd.dll" 
    "${OSG_PLUGIN_DIR}/osgdb_3dsd.dll"
    "${OSG_PLUGIN_DIR}/osgdb_tgad.dll"
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    CONFIGURATIONS Debug)

install(FILES 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osg.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgUtil.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgDB.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgGA.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgText.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgViewer.dll" 
    "${OSG_BINARY_DIR}/ot11-OpenThreads.dll" 
    "${OSG_PLUGIN_DIR}/osgdb_3ds.dll"
    "${OSG_PLUGIN_DIR}/osgdb_tga.dll"
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    CONFIGURATIONS Release)


# qt is required for apps only, so appropriate find_package is called there

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/3rdp
    ${PROJECT_SOURCE_DIR}/include
    ${OSG_INCLUDE_DIR})

add_subdirectory("3rdp")
add_subdirectory("src")
add_subdirectory("apps")


# cpack deploy

set(CPACK_GENERATOR "ZIP")
set(CPACK_SOURCE_GENERATOR "ZIP")

set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH 0)

set(CPACK_INSTALL_DIRECTORIES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CPACK_IGNORE_FILES "/.pdb/;/.ilk/;/\\.svn/;/\\.hg/;/\\.git/;\\.swp$;\\.#;/#")
set(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_SOURCE_DIR})

include(CPack)