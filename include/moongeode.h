
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
#ifndef __MOONGEODE_H__
#define __MOONGEODE_H__

#include <osg/Geode>


class ProceduralHimmel;
class HimmelQuad;


class MoonGeode : public osg::Geode
{
public:

    MoonGeode(const ProceduralHimmel &himmel);
    virtual ~MoonGeode();

    void update();


    const float setScale(const float scale);
    const float getScale() const;
    static const float defaultScale();

    const osg::Vec3 setSunShineColor(const osg::Vec3 &color);
    const osg::Vec3 getSunShineColor() const;
    static const osg::Vec3 defaultSunShineColor();

    const float setSunShineIntensity(const float intensity);
    const float getSunShineIntensity() const;
    static const float defaultSunShineIntensity();

    const osg::Vec3 setEarthShineColor(const osg::Vec3 &color);
    const osg::Vec3 getEarthShineColor() const;
    static const osg::Vec3 defaultEarthShineColor();

    const float setEarthShineIntensity(const float intensity);
    const float getEarthShineIntensity() const;
    static const float defaultEarthShineIntensity();

protected:

    void setupUniforms(osg::StateSet* stateSet);
    void setupNode    (osg::StateSet* stateSet);
    void setupTextures(osg::StateSet* stateSet);
    void setupShader  (osg::StateSet* stateSet);

    const std::string getVertexShaderSource();
    const std::string getFragmentShaderSource();

protected:

    const ProceduralHimmel &m_himmel;

    HimmelQuad *m_hquad;

    osg::Program *m_program;
    osg::Shader *m_vShader;
    osg::Shader *m_fShader;

    osg::ref_ptr<osg::Uniform> u_sun;
    osg::ref_ptr<osg::Uniform> u_moon;

    osg::ref_ptr<osg::Uniform> u_moonCube;
    osg::ref_ptr<osg::Uniform> u_R;
    osg::ref_ptr<osg::Uniform> u_sunShine;
    osg::ref_ptr<osg::Uniform> u_earthShine;

    float m_scale;
};

#endif // __MOONGEODE_H__