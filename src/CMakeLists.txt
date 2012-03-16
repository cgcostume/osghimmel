
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
    ${HEADER_PATH}/earth.h
	${HEADER_PATH}/earth2.h
    ${HEADER_PATH}/milkywaygeode.h
    ${HEADER_PATH}/gaussianmapgenerator.h
    ${HEADER_PATH}/himmelenvmap.h
    ${HEADER_PATH}/himmelquad.h
    ${HEADER_PATH}/horizonband.h
    ${HEADER_PATH}/interpolate.h
    ${HEADER_PATH}/julianday.h
    ${HEADER_PATH}/mathmacros.h
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
    ${HEADER_PATH}/twounitschanger.h


#   shader fragments

    shaderfragment/blend_difference.hpp
    shaderfragment/blend_lineardodge.hpp
    shaderfragment/blend_multiply.hpp
    shaderfragment/blend_normal.hpp
    shaderfragment/blend_screen.hpp
    shaderfragment/bruneton_common.hpp
    shaderfragment/bruneton_inscatter.hpp
    shaderfragment/bruneton_irradiance.hpp
    shaderfragment/bruneton_transmittance.hpp
    shaderfragment/cloudlayer.hpp
    shaderfragment/common.hpp
    shaderfragment/compose.hpp
    shaderfragment/dither.hpp
    shaderfragment/hband.hpp
    shaderfragment/noise.hpp
    shaderfragment/pseudo_rand.hpp
    shaderfragment/quadretrieveray.hpp
    shaderfragment/quadtransform.hpp
    shaderfragment/fakesun.hpp
    shaderfragment/version.hpp)

source_group_by_path(${PROJECT_SOURCE_DIR} ${LIB_SOURCES})    

message(STATUS "add static library: osghimmel")

add_library(osghimmel STATIC ${LIB_SOURCES})
set_default_target_properties(osghimmel ON)

if(OPTION_MAKE_SKYBOX)
    ADD_DEFINITIONS("-DOSGHIMMEL_ENABLE_SHADERMODIFIER")
endif()