
// Copyright (c) 2011-2012, Daniel Müller <dm@g4t3.de>
// Computer Graphics Systems Group at the Hasso-Plattner-Institute, Germany
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright notice, 
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright 
//     notice, this list of conditions and the following disclaimer in the 
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of the Computer Graphics Systems Group at the 
//     Hasso-Plattner-Institute (HPI), Germany nor the names of its 
//     contributors may be used to endorse or promote products derived from 
//     this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.

#pragma once
#ifndef __CLOUDLAYERHIGHGEODE_H__
#define __CLOUDLAYERHIGHGEODE_H__

#include "declspec.h"

#include <osg/Group>


namespace osg
{
    class Texture2D;
}

namespace osgHimmel
{

class Himmel;
class HimmelQuad;


class OSGH_API CloudLayerHighGeode : public osg::Group
{
public:

    CloudLayerHighGeode();
    virtual ~CloudLayerHighGeode();

    void update(const Himmel &himmel);

protected:

    void precompute();

    void setupUniforms(osg::StateSet* stateSet);
    void setupNode    (osg::StateSet* stateSet);
    void setupTextures(osg::StateSet* stateSet);
    void setupShader  (osg::StateSet* stateSet);

    const char* getVertexShaderSource();
    const char* getFragmentShaderSource();

protected:

    HimmelQuad *m_hquad;

    //osg::Texture2D *m_transmittance;

    osg::Program *m_program;
    osg::Shader *m_vShader;
    osg::Shader *m_fShader;

    osg::ref_ptr<osg::Uniform> u_perm;
    osg::ref_ptr<osg::Uniform> u_perlin;


#ifdef OSGHIMMEL_EXPOSE_SHADERS
public:
    osg::Shader *vertexShader();
    osg::Shader *geometryShader();
    osg::Shader *fragmentShader();
#endif // OSGHIMMEL_EXPOSE_SHADERS
};

} // namespace osgHimmel

#endif // __CLOUDLAYERHIGHGEODE_H__