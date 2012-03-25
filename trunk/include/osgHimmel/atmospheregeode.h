
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
#ifndef __ATMOSPHEREGEODE_H__
#define __ATMOSPHEREGEODE_H__

#include "declspec.h"

#include <osg/Group>

namespace osg
{
    class Texture2D;
    class Texture3D;
}


namespace osgHimmel
{

class AtmospherePrecompute;
class Himmel;
class HimmelQuad;


class OSGH_API AtmosphereGeode : public osg::Group
{
public:

    AtmosphereGeode();
    virtual ~AtmosphereGeode();

    void update(const Himmel &himmel);
    //void precompute();

    const float setSunScale(const float scale);
    const float getSunScale() const;
    static const float defaultSunScale();

    const float setExposure(const float exposure);
    const float getExposure() const;
    static const float defaultExposure();

    const osg::Vec3 setLHeureBleueColor(const osg::Vec3 &color);
    const osg::Vec3 getLHeureBleueColor() const;
    static const osg::Vec3 defaultLHeureBleueColor();
    
    const float setLHeureBleueIntensity(const float intensity);
    const float getLHeureBleueIntensity() const;
    static const float defaultLHeureBleueIntensity();


    void setAverageGroundReflectance(const float reflectance); // [0;1]
    void setThicknessRayleigh(const float thickness); 
    void setScatteringRayleigh(const osg::Vec3 &coefficients); 
    void setThicknessMie(const float thickness); 
    void setScatteringMie(const float coefficient); 
    void setPhaseG(const float g);  // [-1;+1]

protected:

    void precompute();

    void setupUniforms(osg::StateSet* stateSet);
    void setupNode    (osg::StateSet* stateSet);
    void setupTextures(osg::StateSet* stateSet);
    void setupShader  (osg::StateSet* stateSet);
    void updateShader (osg::StateSet* stateSet);

    const char* getVertexShaderSource();
    const char* getFragmentShaderSource();

protected:

    HimmelQuad *m_hquad;

    AtmospherePrecompute *m_precompute;

    osg::Texture2D *m_transmittance;
    osg::Texture2D *m_irradiance;
    osg::Texture3D *m_inscatter;

    osg::Program *m_program;
    osg::Shader *m_vShader;
    osg::Shader *m_fShader;

    osg::ref_ptr<osg::Uniform> u_sunScale;
    osg::ref_ptr<osg::Uniform> u_exposure;
    osg::ref_ptr<osg::Uniform> u_lheurebleue;

    float m_scale;


#ifdef OSGHIMMEL_EXPOSE_SHADERS
public:
    osg::Shader *vertexShader();
    osg::Shader *geometryShader();
    osg::Shader *fragmentShader();
#endif // OSGHIMMEL_EXPOSE_SHADERS
};

} // namespace osgHimmel

#endif // __ATMOSPHEREGEODE_H__