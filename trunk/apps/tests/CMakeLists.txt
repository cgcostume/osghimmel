
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

set(TESTS_SOURCES
    stable.h
    stable.cpp
	tests.cpp
	test.cpp
	test.h
	test_astronomy.cpp
	test_astronomy.h
	test_astronomy_la.cpp
	test_astronomy_la.h
	test_math.cpp
	test_math.h
	test_time.cpp
	test_time.h
	test_twounitschanger.cpp
	test_twounitschanger.h)

source_group_by_path(${CMAKE_CURRENT_SOURCE_DIR} ${TESTS_SOURCES})

setup_stable(${TESTS_SOURCES})

message(STATUS "add executable: tests")

add_executable(tests ${TESTS_SOURCES})
set_default_target_properties(tests OFF)

target_link_libraries(tests
    osghimmel
	${OPENSCENEGRAPH_LIBRARIES})

install(TARGETS tests
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")