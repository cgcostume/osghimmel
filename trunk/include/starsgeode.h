
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

#include <osg/Geode>


class ProceduralHimmel;


class StarsGeode : public osg::Geode
{
public:

   StarsGeode(const ProceduralHimmel &himmel);
    virtual ~StarsGeode();

    void update();

    const float setGlareIntensity(const float intensity);
    const float getGlareIntensity() const;

    const float setGlareScale(const float scale);
    const float getGlareScale() const;

    const float setMaxVMag(const float vMag);
    const float getMaxVMag() const;
    static const float defaultMaxVMag();

protected:

    void setupUniforms(osg::StateSet* stateSet);
    void setupNode    (osg::StateSet* stateSet);
    void setupTextures(osg::StateSet* stateSet);
    void setupShader  (osg::StateSet* stateSet);

    void createAndAddDrawable();

    const std::string getVertexShaderSource();
    const std::string getGeometryShaderSource();
    const std::string getFragmentShaderSource();

protected:

    const ProceduralHimmel &m_himmel;

    osg::Program *m_program;
    osg::Shader *m_vShader;
    osg::Shader *m_gShader;
    osg::Shader *m_fShader;

    osg::ref_ptr<osg::Uniform> u_starWidth;

    osg::ref_ptr<osg::Uniform> u_maxVMag;
    osg::ref_ptr<osg::Uniform> u_glareIntensity;
    osg::ref_ptr<osg::Uniform> u_glareScale;
};

#endif // __STARSGEODE_H__