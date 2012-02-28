
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
#ifndef __ATMOSPHEREPRECOMPUTE_H__
#define __ATMOSPHEREPRECOMPUTE_H__

#include <osg/Referenced>
#include <osg/GL>
#include <osg/ref_ptr>
#include <osg/Vec3f>

#include <map>
#include <vector>
#include <string>


namespace osg
{
    class Texture2D;
    class Texture3D;
    class Image;
    class Uniform;
    class GraphicsContext;
    class Geode;
    class StateSet;
    class Program;
    class Camera;
    class Group;
}

namespace osgViewer
{
    class CompositeViewer;
}


class AtmospherePrecompute : public osg::Referenced
{
public:
    
    typedef struct PrecomputedTextureConfig
    {
        int transmittanceWidth;
        int transmittanceHeight;
    
        int skyHeight;
        int skyWidth;

        int resR;
        int resMu;
        int resMuS;
        int resNu;

        int transmittanceIntegralSamples;
        int inscatterIntegralSamples;
        int irradianceIntegralSamples;
        int inscatterSphericalIntegralSamples;

    } t_preTexCfg;

    typedef struct PhysicalModelConfig
    {
        float avgGroundReflectance;

        float HR; // Reighley const denisty Atmosphere height in km
        osg::Vec3f betaR;

        float HM; // Mie const denisty Atmosphere height in km
        osg::Vec3f betaMSca;
        osg::Vec3f betaMEx;
        float mieG;

    } t_modelCfg;

    t_modelCfg &getModelConfig()
    {
        return m_modelCfg;
    }

protected:

    t_preTexCfg &getTextureConfig()
    {
        return m_preTexCfg;
    }

protected:

    typedef std::map<GLint, osg::Texture2D*> t_tex2DsByUnit;
    typedef std::map<GLint, osg::Texture3D*> t_tex3DsByUnit;

    typedef std::vector<osg::Uniform*> t_uniforms;

public:

    AtmospherePrecompute();
    virtual ~AtmospherePrecompute();

    osg::Texture2D *getTransmittanceTexture();
    osg::Texture2D *getIrradianceTexture();
    osg::Texture3D *getInscatterTexture();

    const bool compute(const bool ifDirtyOnly = true);
    void dirty();

    void substituteMacros(std::string &source);

protected:

    osg::Texture2D *getDeltaETexture();
    osg::Texture3D *getDeltaSRTexture();
    osg::Texture3D *getDeltaSMTexture();
    osg::Texture3D *getDeltaJTexture();

    osg::GraphicsContext *setupContext();

    osg::Geode *genQuad() const;

    osg::Texture2D *setupTexture2D(
        const char *name // used as sampler identifier
    ,   const GLenum internalFormat
    ,   const GLenum pixelFormat
    ,   const GLenum dataType
    ,   const int width
    ,   const int height
    ,   osg::Image *image = NULL);

    osg::Texture3D *setupTexture3D(
        const char *name // used as sampler identifier
    ,   const GLenum internalFormat
    ,   const GLenum pixelFormat
    ,   const GLenum dataType
    ,   const int width
    ,   const int height
    ,   const int depth
    ,   osg::Image *image = NULL);

    osg::Image *getLayerFrom3DImage(
        osg::Image *source
    ,   const int layer);

    void setupLayerUniforms(
        osg::StateSet *stateSet
    ,   const int depth
    ,   const int layer);

    osg::Program *setupProgram(
        std::string fragmentShaderSource);

    osg::Camera *setupCamera(
        const int viewportWidth
    ,   const int viewportHeight
    ,   osg::Geode *geode
    ,   const int orderNum);

    osg::Group *setupGroup(
        osgViewer::CompositeViewer *viewer);

    void cleanUp(
        osgViewer::CompositeViewer *viewer);

    void assignUniforms(
        osg::StateSet *stateSet
    ,   t_uniforms &uniforms);

    void assignSamplers(
        osg::StateSet *stateSet
    ,   t_tex2DsByUnit &samplers2D
    ,   t_tex3DsByUnit &samplers3D);

    void dirtyTargets(t_tex2DsByUnit &targets2D);
    void dirtyTargets(t_tex3DsByUnit &targets3D);

    void render2D(
        osgViewer::CompositeViewer *viewer
    ,   osg::Geode *geode
    ,   t_tex2DsByUnit &targets2D
    ,   t_tex2DsByUnit &samplers2D
    ,   t_tex3DsByUnit &samplers3D
    ,   t_uniforms &uniforms
    ,   const std::string &fragmentShaderSource);

    void render3D(
        osgViewer::CompositeViewer *viewer
    ,   osg::Geode *geode
    ,   t_tex3DsByUnit &targets3D
    ,   t_tex2DsByUnit &samplers2D
    ,   t_tex3DsByUnit &samplers3D
    ,   t_uniforms &uniforms
    ,   const std::string &fragmentShaderSource);


    // str utils

    void replace(
        std::string& string
    ,   const std::string &search
    ,   const int value);

    void replace(
        std::string& string
    ,   const std::string &search
    ,   const float value);

    void replace(
        std::string& string
    ,   const std::string &search
    ,   const osg::Vec3f value);

    static void replace(
        std::string& string
    ,   const std::string &search
    ,   const std::string &replace);

protected:

    bool m_dirty;

    t_preTexCfg m_preTexCfg;
    t_modelCfg m_modelCfg;

    osg::ref_ptr<osg::Texture2D> m_transmittanceTexture;
    osg::ref_ptr<osg::Texture2D> m_deltaETexture;
    osg::ref_ptr<osg::Texture3D> m_deltaSRTexture;
    osg::ref_ptr<osg::Texture3D> m_deltaSMTexture;
    osg::ref_ptr<osg::Texture2D> m_irradianceTexture;
    osg::ref_ptr<osg::Texture3D> m_inscatterTexture;
    osg::ref_ptr<osg::Texture3D> m_deltaJTexture;

    osg::ref_ptr<osg::Image> m_transmittanceImage;
    osg::ref_ptr<osg::Image> m_irradianceImage;
    osg::ref_ptr<osg::Image> m_inscatterImage;
};

#endif // __ATMOSPHEREPRECOMPUTE_H__