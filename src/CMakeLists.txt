
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

set(HEADER_PATH ${PROJECT_SOURCE_DIR}/include)

set(LIB_SOURCES

    abstracthimmel.cpp
    abstractmappedhimmel.cpp
    cubemappedhimmel.cpp
    himmelquad.cpp
    paraboloidmappedhimmel.cpp
    polarmappedhimmel.cpp
    proceduralhimmel.cpp
    shadermodifier.cpp
    timef.cpp
    twounitschanger.cpp
    
    ${HEADER_PATH}/abstracthimmel.h
    ${HEADER_PATH}/abstractmappedhimmel.h
    ${HEADER_PATH}/cubemappedhimmel.h
    ${HEADER_PATH}/himmelquad.h
    ${HEADER_PATH}/interpolate.h
    ${HEADER_PATH}/paraboloidmappedhimmel.h
    ${HEADER_PATH}/polarmappedhimmel.h
    ${HEADER_PATH}/pragmanote.h
    ${HEADER_PATH}/proceduralhimmel.h
    ${HEADER_PATH}/shadermodifier.h
    ${HEADER_PATH}/timef.h
    ${HEADER_PATH}/twounitschanger.h


#   shader fragments

    shaderfragment/blend_difference.fsf
    shaderfragment/blend_lineardodge.fsf
    shaderfragment/blend_multiply.fsf
    shaderfragment/blend_normal.fsf
    shaderfragment/blend_screen.fsf
    shaderfragment/compose.fsf
    shaderfragment/quadretrieveray.vsf
    shaderfragment/quadtransform.vsf
    shaderfragment/version.fsf
    shaderfragment/version.vsf)

source_group_by_path(${PROJECT_SOURCE_DIR} ${LIB_SOURCES})    

message(STATUS "add static library: osghimmel")

add_library(osghimmel STATIC ${LIB_SOURCES})
set_default_target_properties(osghimmel ON)

if(OPTION_MAKE_SKYBOX)
    ADD_DEFINITIONS("-DOSGHIMMEL_ENABLE_SHADERMODIFIER")
endif()