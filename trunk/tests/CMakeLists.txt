
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

message(STATUS "add executable: tests")

set(TESTS_SOURCES
    tests.cpp
    test.cpp
    test.h
    test_astronomy.cpp
    test_astronomy.h
    test_astronomy2.cpp
    test_astronomy2.h
    test_math.cpp
    test_math.h
    test_time.cpp
    test_time.h
    test_twounitschanger.cpp
    test_twounitschanger.h)

source_group_by_path(${CMAKE_CURRENT_SOURCE_DIR} ${TESTS_SOURCES})

add_executable(tests ${TESTS_SOURCES})

target_link_libraries(tests
    osgHimmel
    ${OPENSCENEGRAPH_LIBRARIES})

install(TARGETS tests
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
	
# install

install(FILES 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgd.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgUtild.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgDBd.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgGAd.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgTextd.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgViewerd.dll" 
    "${OSG_BINARY_DIR}/ot12-OpenThreadsd.dll" 
    "${OSG_BINARY_DIR}/zlib1d.dll" 
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    CONFIGURATIONS Debug)

install(FILES 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osg.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgUtil.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgDB.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgGA.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgText.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgViewer.dll" 
    "${OSG_BINARY_DIR}/ot12-OpenThreads.dll" 
    "${OSG_BINARY_DIR}/zlib1.dll" 
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    CONFIGURATIONS Release)	