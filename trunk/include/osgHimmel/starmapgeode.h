
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
#ifndef __STARMAPGEODE_H__
#define __STARMAPGEODE_H__

#include "declspec.h"

#include <osg/Geode>


namespace osgHimmel
{

class Himmel;
class HimmelQuad;


class OSGH_API StarMapGeode : public osg::Geode
{
public:

    // cubeMapFilePath should contain a questionmark '?' that is replaced
    // by cubemap extensions '_px', '_nx', '_py', etc. 
    // e.g. "resources/starmap?.png" points to "resources/starmap_px.png" etc.

    StarMapGeode(const char* cubeMapFilePath);
    virtual ~StarMapGeode();

    void update(const Himmel &himmel);

    const float setColorRatio(const float ratio);
    const float getColorRatio() const;
    static const float defaultColorRatio();

    const osg::Vec3 setColor(const osg::Vec3 &color);
    const osg::Vec3 getColor() const;
    static const osg::Vec3 defaultColor();

    const float setApparentMagnitude(const float vMag);
    const float getApparentMagnitude() const;
    static const float defaultApparentMagnitude();

    const float setScattering(const float scattering);
    const float getScattering() const;
    static const float defaultScattering();

protected:

    void setupNode(osg::StateSet* stateSet);
    void setupUniforms(osg::StateSet* stateSet);
    void setupTextures(
        osg::StateSet* stateSet
    ,   const char* cubeMapFilePath);

    void setupShader  (osg::StateSet* stateSet);

    const std::string getVertexShaderSource();
    const std::string getGeometryShaderSource();
    const std::string getFragmentShaderSource();

    void updateScaledB();

protected:

    HimmelQuad *m_hquad;

    osg::Program *m_program;
    osg::Shader *m_vShader;
    osg::Shader *m_fShader;

    float m_apparentMagnitude;

    osg::ref_ptr<osg::Uniform> u_R;
    osg::ref_ptr<osg::Uniform> u_q;
    osg::ref_ptr<osg::Uniform> u_starmapCube;
    osg::ref_ptr<osg::Uniform> u_color;
    osg::ref_ptr<osg::Uniform> u_deltaM;
    osg::ref_ptr<osg::Uniform> u_scattering;


#ifdef OSGHIMMEL_EXPOSE_SHADERS
public:
    osg::Shader *getVertexShader();
    osg::Shader *getGeometryShader();
    osg::Shader *getFragmentShader();
#endif // OSGHIMMEL_EXPOSE_SHADERS
};

} // namespace osgHimmel

#endif // __STARMAPGEODE_H__