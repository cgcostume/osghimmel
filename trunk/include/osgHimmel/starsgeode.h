
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
#ifndef __STARSGEODE_H__
#define __STARSGEODE_H__

#include "declspec.h"
#include "brightstars.h"

#include <osg/Geode>


namespace osgHimmel
{

class Himmel;


class OSGH_API StarsGeode : public osg::Geode
{
public:

    StarsGeode(const char *brightStarsFilePath);
    virtual ~StarsGeode();

    void update(const Himmel &himmel);

    const float setApparentMagnitude(const float vMag);
    const float getApparentMagnitude() const;
    static const float defaultApparentMagnitude();

    const osg::Vec3 setColor(const osg::Vec3 color);
    const osg::Vec3 getColor() const;
    static const osg::Vec3 defaultColor();

    const float setColorRatio(const float ratio);
    const float getColorRatio() const;
    static const float defaultColorRatio();
    

    const float setGlareIntensity(const float intensity);
    const float getGlareIntensity() const;

    const float setGlareScale(const float scale);
    const float getGlareScale() const;
    static const float defaultGlareScale();

    const float setScintillation(const float scintillation);
    const float getScintillation() const;
    static const float defaultScintillation();

    const float setScattering(const float scattering);
    const float getScattering() const;
    static const float defaultScattering();


protected:

    void setupUniforms(osg::StateSet* stateSet);

    void setupNode(
        osg::StateSet* stateSet
    ,   const char *brightStarsFilePath);

    void setupTextures(osg::StateSet* stateSet);
    void setupShader  (osg::StateSet* stateSet);

    void createAndAddDrawable(
        const char *brightStarsFilePath);

    const std::string getVertexShaderSource();
    const std::string getGeometryShaderSource();
    const std::string getFragmentShaderSource();

protected:

    osg::Program *m_program;
    osg::Shader *m_vShader;
    osg::Shader *m_gShader;
    osg::Shader *m_fShader;

    osg::ref_ptr<osg::Uniform> u_R;
    osg::ref_ptr<osg::Uniform> u_q;
    osg::ref_ptr<osg::Uniform> u_noise1;

    osg::ref_ptr<osg::Uniform> u_color;
    osg::ref_ptr<osg::Uniform> u_glareIntensity;
    osg::ref_ptr<osg::Uniform> u_glareScale;
    osg::ref_ptr<osg::Uniform> u_apparentMagnitude;
    osg::ref_ptr<osg::Uniform> u_scattering;
    osg::ref_ptr<osg::Uniform> u_scintillations;

    // TODO: replace with VMag intensity function of procedural himmel
    osg::ref_ptr<osg::Uniform> u_sun;


#ifdef OSGHIMMEL_EXPOSE_SHADERS
public:
    osg::Shader *getVertexShader();
    osg::Shader *getGeometryShader();
    osg::Shader *getFragmentShader();
#endif // OSGHIMMEL_EXPOSE_SHADERS
};

} // namespace osgHimmel

#endif // __STARSGEODE_H__