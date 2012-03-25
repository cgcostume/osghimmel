
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

message(STATUS "add static library: osgHimmel")

set(HEADER_PATH ${PROJECT_SOURCE_DIR}/include)

set(LIB_SOURCES

    abstracthimmel.cpp
    abstractmappedhimmel.cpp
    abstractastronomy.cpp
    astronomy.cpp
    astronomy2.cpp
    atime.cpp
    atmospheregeode.cpp
    atmosphereprecompute.cpp
    brightstars.cpp
    cloudlayerhighgeode.cpp
    coords.cpp
    cubemappedhimmel.cpp
    earth.cpp
    earth2.cpp
    milkywaygeode.cpp
    gaussianmapgenerator.cpp
    himmelenvmap.cpp
    himmelquad.cpp
    horizonband.cpp
    julianday.cpp
    moon.cpp
    moon2.cpp
    moongeode.cpp
    noise.cpp
    paraboloidmappedhimmel.cpp
    perlinmapgenerator.cpp
    polarmappedhimmel.cpp
    himmel.cpp
    randommapgenerator.cpp
    shadermodifier.cpp
    sideraltime.cpp
    spheremappedhimmel.cpp
    stars.cpp
    starsgeode.cpp
    strutils.cpp
    sun.cpp
    sun2.cpp
    timef.cpp
    twounitschanger.cpp

    ${HEADER_PATH}/abstracthimmel.h
    ${HEADER_PATH}/abstractmappedhimmel.h
    ${HEADER_PATH}/abstractastronomy.h
    ${HEADER_PATH}/astronomy.h
    ${HEADER_PATH}/astronomy2.h
    ${HEADER_PATH}/atime.h
    ${HEADER_PATH}/atmospheregeode.h
    ${HEADER_PATH}/atmosphereprecompute.h
    ${HEADER_PATH}/brightstars.h
    ${HEADER_PATH}/cloudlayerhighgeode.h
    ${HEADER_PATH}/coords.h
    ${HEADER_PATH}/cubemappedhimmel.h
	${HEADER_PATH}/declspec.h
    ${HEADER_PATH}/earth.h
    ${HEADER_PATH}/earth2.h
    ${HEADER_PATH}/gaussianmapgenerator.h
    ${HEADER_PATH}/himmelenvmap.h
    ${HEADER_PATH}/himmelquad.h
    ${HEADER_PATH}/horizonband.h
	${HEADER_PATH}/interpolate.h
    ${HEADER_PATH}/julianday.h
    ${HEADER_PATH}/mathmacros.h
    ${HEADER_PATH}/milkywaygeode.h
    ${HEADER_PATH}/moon.h
    ${HEADER_PATH}/moon2.h
    ${HEADER_PATH}/moongeode.h
    ${HEADER_PATH}/noise.h
    ${HEADER_PATH}/paraboloidmappedhimmel.h
    ${HEADER_PATH}/perlinmapgenerator.h
    ${HEADER_PATH}/polarmappedhimmel.h
	${HEADER_PATH}/pragmanote.h
    ${HEADER_PATH}/himmel.h
    ${HEADER_PATH}/randommapgenerator.h
    ${HEADER_PATH}/shadermodifier.h
    ${HEADER_PATH}/sideraltime.h
    ${HEADER_PATH}/spheremappedhimmel.h
    ${HEADER_PATH}/stars.h
    ${HEADER_PATH}/starsgeode.h
	${HEADER_PATH}/strutils.h
    ${HEADER_PATH}/sun.h
    ${HEADER_PATH}/sun2.h
    ${HEADER_PATH}/timef.h
    ${HEADER_PATH}/twounitschanger.h)

set(SHADER_FRAGMENTS

    shaderfragment/blend_difference.h
    shaderfragment/blend_difference.cpp
    shaderfragment/blend_lineardodge.h
    shaderfragment/blend_lineardodge.cpp
    shaderfragment/blend_multiply.h
    shaderfragment/blend_multiply.cpp
    shaderfragment/blend_normal.h
    shaderfragment/blend_normal.cpp
    shaderfragment/blend_screen.h
    shaderfragment/blend_screen.cpp
    shaderfragment/bruneton_common.h
    shaderfragment/bruneton_common.cpp
    shaderfragment/bruneton_inscatter.h
    shaderfragment/bruneton_inscatter.cpp
    shaderfragment/bruneton_irradiance.h
    shaderfragment/bruneton_irradiance.cpp
    shaderfragment/bruneton_transmittance.h
    shaderfragment/bruneton_transmittance.cpp
    shaderfragment/cloudlayer.h
    shaderfragment/cloudlayer.cpp
    shaderfragment/common.h
    shaderfragment/common.cpp
    shaderfragment/compose.h
    shaderfragment/dither.h
    shaderfragment/dither.cpp
    shaderfragment/fakesun.h
    shaderfragment/fakesun.cpp
    shaderfragment/flags.h
    shaderfragment/hband.h
    shaderfragment/hband.cpp
    shaderfragment/noise.h
    shaderfragment/noise.cpp
    shaderfragment/pragma_once.h
    shaderfragment/pseudo_rand.h
    shaderfragment/pseudo_rand.cpp
    shaderfragment/quadretrieveray.h
    shaderfragment/quadretrieveray.cpp
    shaderfragment/quadtransform.h
    shaderfragment/quadtransform.cpp
    shaderfragment/version.h
    shaderfragment/version.cpp)

source_group("" FILES ${LIB_SOURCES})    
source_group("Shader Fragments" FILES ${SHADER_FRAGMENTS})

add_library(osgHimmel SHARED ${LIB_SOURCES} ${SHADER_FRAGMENTS})

target_link_libraries(osgHimmel
    ${OPENSCENEGRAPH_LIBRARIES}
    ${OPENGL_gl_LIBRARY}
    ${OPENGL_glu_LIBRARY})

if(MSVC)

	set_target_properties(osgHimmel
		PROPERTIES
		COMPILE_FLAGS "/W4 /WX /fp:fast /Ot /GS-"
		LINK_FLAGS_DEBUG "/NODEFAULTLIB:msvcrt"
		PREFIX "osg${OSG_SOVERSION}-"
		DEBUG_POSTFIX "d${DEBUG_POSTFIX}")

	add_definitions("-DOSGHIMMEL_EXPORTS")

else(MSVC)
	set_target_properties(osgHimmel
		PROPERTIES
		DEBUG_POSTFIX "d${DEBUG_POSTFIX}")
endif()

if(OPTION_MAKE_SKYBOX)
    #ADD_DEFINITIONS("-DOSGHIMMEL_ENABLE_SHADERMODIFIER")
endif()