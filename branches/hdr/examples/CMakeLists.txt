
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
     
set(RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../resources" CACHE PATH "Resources for osgHimmel binaries.")

if(NOT EXISTS "${RESOURCE_DIR}/")
	message(WARNING "Resource dir not found, please specify RESOURCE_DIR.")
else()
	install(DIRECTORY "${RESOURCE_DIR}" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

if(OPTION_MAKE_DEMOS)
    add_subdirectory("demo_computed")
    add_subdirectory("demo_textured")
endif()
    
if(OPTION_MAKE_SKYBOX)
    add_subdirectory("skybox")
endif()

# install

install(FILES 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgd.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgUtild.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgDBd.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgGAd.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgTextd.dll" 
    "${OSG_BINARY_DIR}/osg${OSG_SOVERSION}-osgViewerd.dll" 
    "${OSG_BINARY_DIR}/ot12-OpenThreadsd.dll" 
    "${OSG_BINARY_DIR}/libpng13d.dll" 
    "${OSG_BINARY_DIR}/zlib1d.dll" 
    "${OSG_PLUGIN_DIR}/osgdb_3dsd.dll"
    "${OSG_PLUGIN_DIR}/osgdb_lwod.dll"
    "${OSG_PLUGIN_DIR}/osgdb_objd.dll"
    "${OSG_PLUGIN_DIR}/osgdb_osgd.dll"
    "${OSG_PLUGIN_DIR}/osgdb_xd.dll"
    "${OSG_PLUGIN_DIR}/osgdb_tgad.dll"
    "${OSG_PLUGIN_DIR}/osgdb_ddsd.dll"
    "${OSG_PLUGIN_DIR}/osgdb_jpegd.dll"
    "${OSG_PLUGIN_DIR}/osgdb_pngd.dll"
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
    "${OSG_BINARY_DIR}/libpng13.dll" 
    "${OSG_BINARY_DIR}/zlib1.dll" 
    "${OSG_PLUGIN_DIR}/osgdb_3ds.dll"
    "${OSG_PLUGIN_DIR}/osgdb_lwo.dll"
    "${OSG_PLUGIN_DIR}/osgdb_obj.dll"
    "${OSG_PLUGIN_DIR}/osgdb_osg.dll"
    "${OSG_PLUGIN_DIR}/osgdb_x.dll"
    "${OSG_PLUGIN_DIR}/osgdb_tga.dll"
    "${OSG_PLUGIN_DIR}/osgdb_dds.dll"
    "${OSG_PLUGIN_DIR}/osgdb_jpeg.dll"
    "${OSG_PLUGIN_DIR}/osgdb_png.dll"
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    CONFIGURATIONS Release)