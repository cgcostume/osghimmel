
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


// based on Brunetons free code (http://www-evasion.imag.fr/Members/Eric.Bruneton/PrecomputedAtmosphericScattering2.zip)

// The mapping to a scenegraph was inspired by various forum entries
// (e.g. http://comments.gmane.org/gmane.comp.graphics.openscenegraph.user/60641,
//  http://forum.openscenegraph.org/viewtopic.php?t=9184, ...) with
// and partially ported from Emmanuel Roches' lua implementation which 
// he kindly provided me on request. (roche.emmanuel@gmail.com)


#include "atmosphereprecompute.h"

#include "himmel.h"
#include "earth.h"
#include "strutils.h"

#include <osg/Image>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/Texture3D>
#include <osgViewer/CompositeViewer>

#include "shaderfragment/bruneton_common.h"
#include "shaderfragment/bruneton_inscatter.h"
#include "shaderfragment/bruneton_irradiance.h"
#include "shaderfragment/bruneton_transmittance.h"

#include <assert.h>


namespace osgHimmel
{

AtmospherePrecompute::AtmospherePrecompute()
:   m_transmittanceImage(new osg::Image)
,   m_irradianceImage(new osg::Image)
,   m_inscatterImage(new osg::Image)
,   m_dirty(true)
{
    m_preTexCfg.transmittanceWidth  = 256;
    m_preTexCfg.transmittanceHeight =  64;

    m_preTexCfg.skyWidth  =  64;
    m_preTexCfg.skyHeight =  16;

    m_preTexCfg.resR   =  32;
    m_preTexCfg.resMu  = 128;
    m_preTexCfg.resMuS =  32;
    m_preTexCfg.resNu  =   8;

    m_preTexCfg.transmittanceIntegralSamples      = 500;
    m_preTexCfg.inscatterIntegralSamples          =  50;
    m_preTexCfg.irradianceIntegralSamples         =  32;
    m_preTexCfg.inscatterSphericalIntegralSamples =  16;

    m_modelCfg.avgGroundReflectance = 0.1f;

    m_modelCfg.HR = 8.f;
    m_modelCfg.betaR = osg::Vec3f(5.8e-3, 1.35e-2, 3.31e-2);
    
    m_modelCfg.HM = 6.f; //1.2f;
    m_modelCfg.betaMSca = osg::Vec3f(1.f, 1.f, 1.f) * 20e-3, //8e-3; 
    m_modelCfg.betaMEx = m_modelCfg.betaMSca / 0.9f;
    m_modelCfg.mieG = 0.6; //0.76;


    // Setup Textures

    m_transmittanceTexture = getTransmittanceTexture();
    m_deltaETexture        = getDeltaETexture();
    m_deltaSRTexture       = getDeltaSRTexture();
    m_deltaSMTexture       = getDeltaSMTexture();
    m_irradianceTexture    = getIrradianceTexture();
    m_inscatterTexture     = getInscatterTexture();
    m_deltaJTexture        = getDeltaJTexture();
}


AtmospherePrecompute::~AtmospherePrecompute()
{
}


osg::Texture2D *AtmospherePrecompute::getTransmittanceTexture()
{
    return setupTexture2D("transmittance", GL_RGB16F_ARB, GL_RGB, GL_FLOAT
        , getTextureConfig().transmittanceWidth, getTextureConfig().transmittanceHeight, m_transmittanceImage);
}
osg::Texture2D *AtmospherePrecompute::getIrradianceTexture()
{
    return setupTexture2D("irradiance", GL_RGB16F_ARB, GL_RGB, GL_FLOAT
        , getTextureConfig().skyWidth, getTextureConfig().skyHeight, m_irradianceImage);
}
osg::Texture3D *AtmospherePrecompute::getInscatterTexture()
{
    return setupTexture3D("inscatter", GL_RGBA16F_ARB, GL_RGBA, GL_FLOAT
        , getTextureConfig().resMuS * getTextureConfig().resNu, getTextureConfig().resMu, getTextureConfig().resR, m_inscatterImage);
}


void AtmospherePrecompute::dirty()
{
    if(!m_dirty)
        m_dirty = true;
}


const bool AtmospherePrecompute::compute(const bool ifDirtyOnly)
{
    if(ifDirtyOnly && !m_dirty)
        return false;

    m_dirty = false;


    osg::Timer_t t = osg::Timer::instance()->tick();

    // Setup Viewer

    osgViewer::CompositeViewer *viewer = new osgViewer::CompositeViewer;
    osgViewer::View *view = new osgViewer::View();

    viewer->addView(view);

    // Setup Context and Camera

    osg::GraphicsContext *gc(setupContext());

    if(!gc->valid())
    {
        OSG_FATAL << "Initialize PBuffer graphics context failed" << std::endl;
        return false;
    }


    osg::Camera *camera = view->getCamera();
    camera->setGraphicsContext(gc);
    camera->setClearColor(osg::Vec4(0.f, 0.f, 0.f, 0.f));
    camera->setViewport(0, 0, 1, 1);

    osg::Group *group(new osg::Group);
    view->setSceneData(group);

    osg::Geode *quad = genQuad();

    osg::ref_ptr<osg::Uniform> u_common = Himmel::cmnUniform();
    group->getOrCreateStateSet()->addUniform(u_common);


    // Render

    t_tex2DsByUnit targets2D, samplers2D;
    t_tex3DsByUnit targets3D, samplers3D;

    t_uniforms uniforms;

    // computes transmittance texture T (line 1 in algorithm 4.1)
        
    targets2D[0]  = m_transmittanceTexture;

    render2D(viewer, quad, targets2D, samplers2D, samplers3D, uniforms, glsl_bruneton_f_transmittance.c_str());

    // computes irradiance texture deltaE (line 2 in algorithm 4.1)

    targets2D[0]  = m_deltaETexture;
    samplers2D[0] = m_transmittanceTexture;

    render2D(viewer, quad, targets2D, samplers2D, samplers3D, uniforms, glsl_bruneton_f_irradiance1.c_str());

    // computes single scattering texture deltaS (line 3 in algorithm 4.1)

    targets3D[0]  = m_deltaSRTexture;
    targets3D[1]  = m_deltaSMTexture;
    samplers2D[0] = m_transmittanceTexture;

    render3D(viewer, quad, targets3D, samplers2D, samplers3D, uniforms, glsl_bruneton_f_inscatter1.c_str());

    // copies deltaE into irradiance texture E (line 4 in algorithm 4.1)

    // THIS PATH SEEMS UNREQUIRED - since k = 0 nothing gets copied? At least it would zero the texture...

    targets2D[0]  = m_irradianceTexture;
    samplers2D[0] = m_deltaETexture;
    uniforms.push_back(new osg::Uniform("k", 0.f));

    render2D(viewer, quad, targets2D, samplers2D, samplers3D, uniforms, glsl_bruneton_f_copyIrradiance.c_str());

    // copies deltaS into inscatter texture S (line 5 in algorithm 4.1)

    targets3D[0]  = m_inscatterTexture;
    samplers3D[0] = m_deltaSRTexture;
    samplers3D[1] = m_deltaSMTexture;

    render3D(viewer, quad, targets3D, samplers2D, samplers3D, uniforms, glsl_bruneton_f_copyInscatter1.c_str());
     
    // loop for each scattering order (line 6 in algorithm 4.1)

    for(int order = 2; order <= 4; ++order)
    {
        const float first = order == 2 ? 1.f : 0.f;

        //// computes deltaJ (line 7 in algorithm 4.1)

        targets3D[0]  = m_deltaJTexture;
        samplers2D[0] = m_transmittanceTexture;
        samplers2D[1] = m_deltaETexture;
        samplers3D[2] = m_deltaSRTexture;
        samplers3D[3] = m_deltaSMTexture;
        uniforms.push_back(new osg::Uniform("first", first));

        render3D(viewer, quad, targets3D, samplers2D, samplers3D, uniforms, glsl_bruneton_f_inscatterS.c_str());

        // computes deltaE (line 8 in algorithm 4.1)

        targets2D[0]  = m_deltaETexture;
        samplers2D[0] = m_transmittanceTexture;
        samplers3D[1] = m_deltaSRTexture;
        samplers3D[2] = m_deltaSMTexture;
        uniforms.push_back(new osg::Uniform("first", first));

        render2D(viewer, quad, targets2D, samplers2D, samplers3D, uniforms, glsl_bruneton_f_irradianceN.c_str());

        // computes deltaS (line 9 in algorithm 4.1)

        targets3D[0]  = m_deltaSRTexture;
        samplers2D[0] = m_transmittanceTexture;
        samplers3D[1] = m_deltaJTexture;
        uniforms.push_back(new osg::Uniform("first", first));

        render3D(viewer, quad, targets3D, samplers2D, samplers3D, uniforms, glsl_bruneton_f_inscatterN.c_str());


        // NOTE: http://www.opengl.org/wiki/GLSL_:_common_mistakes#Sampling_and_Rendering_to_the_Same_Texture

        // adds deltaE into irradiance texture E (line 10 in algorithm 4.1)

        targets2D[0]  = m_irradianceTexture;
        samplers2D[0] = m_deltaETexture;
        samplers2D[1] = m_irradianceTexture;
        uniforms.push_back(new osg::Uniform("k", 1.f));

        render2D(viewer, quad, targets2D, samplers2D, samplers3D, uniforms, glsl_bruneton_f_copyIrradiance.c_str());

        // adds deltaS into inscatter texture S (line 11 in algorithm 4.1)
      
        targets3D[0]  = m_inscatterTexture;
        samplers3D[0] = m_deltaSRTexture;
        samplers3D[1] = m_inscatterTexture;

        render3D(viewer, quad, targets3D, samplers2D, samplers3D, uniforms, glsl_bruneton_f_copyInscatterN.c_str());
    }

    // Unref

    delete viewer;

    OSG_NOTICE << "Atmopshere Precomputed (took " 
        << osg::Timer::instance()->delta_s(t,  osg::Timer::instance()->tick()) << " s)" << std::endl;

    return true;
}


osg::Texture2D *AtmospherePrecompute::getDeltaETexture()
{
    return setupTexture2D("deltaE", GL_RGB16F_ARB, GL_RGB, GL_FLOAT
        , getTextureConfig().skyWidth, getTextureConfig().skyHeight);
}
osg::Texture3D *AtmospherePrecompute::getDeltaSRTexture()
{
    return setupTexture3D("deltaSR", GL_RGB16F_ARB, GL_RGB, GL_FLOAT
        , getTextureConfig().resMuS * getTextureConfig().resNu, getTextureConfig().resMu, getTextureConfig().resR);
}
osg::Texture3D *AtmospherePrecompute::getDeltaSMTexture()
{
    return setupTexture3D("deltaSM", GL_RGB16F_ARB, GL_RGB, GL_FLOAT
        , getTextureConfig().resMuS * getTextureConfig().resNu, getTextureConfig().resMu, getTextureConfig().resR);
}
osg::Texture3D *AtmospherePrecompute::getDeltaJTexture()
{
    return setupTexture3D("deltaJ", GL_RGB16F_ARB, GL_RGB, GL_FLOAT
        , getTextureConfig().resMuS * getTextureConfig().resNu, getTextureConfig().resMu, getTextureConfig().resR);
}


osg::GraphicsContext *AtmospherePrecompute::setupContext()
{
    // (pbuffer by http://forum.openscenegraph.org/viewtopic.php?t=9025)

    osg::GraphicsContext::Traits *traits = new osg::GraphicsContext::Traits;

    traits->screenNum = 0;
    traits->x = 0;
    traits->y = 0;
    traits->width = 1;
    traits->height = 1;
    traits->windowDecoration = false;
    traits->doubleBuffer = false;
    traits->sharedContext = NULL;
    traits->pbuffer = true;
    //traits->samples = 16;


    return osg::GraphicsContext::createGraphicsContext(traits);
}


osg::Geode *AtmospherePrecompute::genQuad() const
{
    osg::Geometry *quad(new osg::Geometry);

    osg::Vec3Array *vertices = new osg::Vec3Array();
    vertices->push_back(osg::Vec3(-1.f, -1.f, 0.f));
    vertices->push_back(osg::Vec3(-1.f,  1.f, 0.f));
    vertices->push_back(osg::Vec3( 1.f,  1.f, 0.f));
    vertices->push_back(osg::Vec3( 1.f, -1.f, 0.f));

    quad->setVertexArray(vertices);   
    quad->addPrimitiveSet(
        new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

    osg::Geode *geode(new osg::Geode);
    geode->addDrawable(quad);

    return geode;
}


osg::Texture2D *AtmospherePrecompute::setupTexture2D(
    const char *name // used as sampler identifier
,   const GLenum internalFormat
,   const GLenum pixelFormat
,   const GLenum dataType
,   const int width
,   const int height
,   osg::Image *image)
{
    osg::Texture2D *texture(new osg::Texture2D);

    texture->setName(name);
    texture->setTextureSize(width, height);

    if(image)
    {
        image->setInternalTextureFormat(internalFormat);
        image->allocateImage(width, height, 1, pixelFormat, dataType);

        texture->setImage(image);
    }
    else
    {
        texture->setInternalFormat(internalFormat);
        texture->setSourceFormat(pixelFormat);
        texture->setSourceType(dataType);
    }

    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

    return texture;
}


osg::Texture3D *AtmospherePrecompute::setupTexture3D(
    const char *name // used as sampler identifier
,   const GLenum internalFormat
,   const GLenum pixelFormat
,   const GLenum dataType
,   const int width
,   const int height
,   const int depth
,   osg::Image *image)
{
    osg::Texture3D *texture(new osg::Texture3D);

    texture->setName(name);
    texture->setTextureSize(width, height, depth);

    if(image)
    {
        image->setInternalTextureFormat(internalFormat);
        image->allocateImage(width, height, depth, pixelFormat, dataType);

        texture->setImage(image);
    }
    else
    {
        texture->setInternalFormat(internalFormat);
        texture->setSourceFormat(pixelFormat);
        texture->setSourceType(dataType);
    }

    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

    return texture;
}
    

// (from Emmanuel Roche e.g. here http://www.mail-archive.com/osg-users@lists.openscenegraph.org/msg42832.html)
// Creates a "view" to a single layer of an 3d image. This is 
// required for rendering into a Texture3D based on its image
// (at least when using one camera per layer...).

osg::Image *AtmospherePrecompute::getLayerFrom3DImage(
    osg::Image *source
,   const int layer)
{
    assert(source);

    osg::Image *image(new osg::Image());

    unsigned char *data = source->data(0, 0, layer);

    image->setImage(source->s(), source->t(), 1
        , source->getInternalTextureFormat()
        , source ->getPixelFormat()
        , source->getDataType()
        , data, osg::Image::NO_DELETE, source->getPacking());

    return image;
}


void AtmospherePrecompute::setupLayerUniforms(
    osg::StateSet *stateSet
,   const int depth
,   const int layer)
{
    const double Rg = Earth::meanRadius();
    const double Rt = Earth::meanRadius() + Earth::atmosphereThicknessNonUniform();

    const double Rg2 = Rg * Rg;
    const double Rt2 = Rt * Rt;

    double r = layer / (depth - 1.0);
    r *= r;
    r = sqrt(Rg2 + r * (Rt2 - Rg2)) + (layer == 0 ? 0.01 : (layer == getTextureConfig().resR - 1 ? -0.001 : 0.0));

    double dmin  = Rt - r;
    double dmax  = sqrt(r * r - Rg2) + sqrt(Rt2 - Rg2);
    double dminp = r - Rg;
    double dmaxp = sqrt(r * r - Rg2);

    if(stateSet->getUniform("r"))
        stateSet->getUniform("r")->set(static_cast<float>(r));
    else
        stateSet->addUniform(new osg::Uniform("r", static_cast<float>(r)));

    if(stateSet->getUniform("dhdH"))
        stateSet->getUniform("dhdH")->set(osg::Vec4(dmin, dmax, dminp, dmaxp));
    else
        stateSet->addUniform(new osg::Uniform("dhdH", osg::Vec4(dmin, dmax, dminp, dmaxp)));
}


osg::Program *AtmospherePrecompute::setupProgram(
    const char* fragmentShaderSource)
{
    assert(strlen(fragmentShaderSource) > 0);

    osg::Program *program(new osg::Program);

    program->addShader(new osg::Shader(osg::Shader::VERTEX,   glsl_bruneton_v_default));

    if(strlen(fragmentShaderSource) > 0)
    {
        const char *source = substituteMacros(fragmentShaderSource);
        program->addShader(new osg::Shader(osg::Shader::FRAGMENT, source));
    }

    return program;
}


osg::Camera *AtmospherePrecompute::setupCamera(
    const int viewportWidth
,   const int viewportHeight
,   osg::Geode *geode
,   const int orderNum)
{
    osg::Camera *camera(new osg::Camera);

    camera->setViewport(0, 0, viewportWidth, viewportHeight);
    camera->setRenderOrder(osg::Camera::POST_RENDER, orderNum);

    camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    camera->addChild(geode);

    return camera;
}


osg::Group *AtmospherePrecompute::setupGroup(
    osgViewer::CompositeViewer *viewer)
{
    osg::Group *root(dynamic_cast<osg::Group*>(viewer->getView(0)->getSceneData()));
        
    assert(root->getNumChildren() == 0);

    osg::ref_ptr<osg::Group> group(new osg::Group);
    root->addChild(group.get());

    return group;
}


void AtmospherePrecompute::dirtyTargets(t_tex2DsByUnit &targets2D)
{
    t_tex2DsByUnit::const_iterator i2 = targets2D.begin();
    const t_tex2DsByUnit::const_iterator t2End = targets2D.end();

    for(; i2 != t2End; ++i2)
    {
        if(i2->second->getImage())
            i2->second->getImage()->dirty();
    }
}


void AtmospherePrecompute::dirtyTargets(t_tex3DsByUnit &targets3D)
{
    t_tex3DsByUnit::const_iterator i3 = targets3D.begin();
    const t_tex3DsByUnit::const_iterator t3End = targets3D.end();

    for(; i3 != t3End; ++i3)
    {
        if(i3->second->getImage())
            i3->second->getImage()->dirty();
    }
}


void AtmospherePrecompute::cleanUp(
    osgViewer::CompositeViewer *viewer)
{
    osg::Group *root(dynamic_cast<osg::Group*>(viewer->getView(0)->getSceneData()));
    root->removeChildren(0, root->getNumChildren());
    assert(root->getNumChildren() == 0);
}


void AtmospherePrecompute::assignUniforms(
    osg::StateSet *stateSet
,   t_uniforms &uniforms)
{
    assert(stateSet);

    t_uniforms::const_iterator u = uniforms.begin();
    const t_uniforms::const_iterator uEnd = uniforms.end();

    for(; u != uEnd; ++u)
        stateSet->addUniform(*u);
    uniforms.clear();
}

    
void AtmospherePrecompute::assignSamplers(
    osg::StateSet *stateSet
,   t_tex2DsByUnit &samplers2D
,   t_tex3DsByUnit &samplers3D)
{
    t_tex2DsByUnit::const_iterator i2;
    const t_tex2DsByUnit::const_iterator s2End = samplers2D.end();

    for(i2 = samplers2D.begin(); i2 != s2End; ++i2)
    {
        const GLint unit(i2->first);
        osg::Texture2D *texture(i2->second);

        stateSet->setTextureAttributeAndModes(unit, texture, osg::StateAttribute::ON);
        stateSet->addUniform(new osg::Uniform((texture->getName() + "Sampler").c_str(), unit));
    }
    samplers2D.clear();

    t_tex3DsByUnit::const_iterator i3;
    const t_tex3DsByUnit::const_iterator s3End = samplers3D.end();

    for(i3 = samplers3D.begin(); i3 != s3End; ++i3)
    {
        const GLint unit(i3->first);
        osg::Texture3D *texture(i3->second);

        stateSet->setTextureAttributeAndModes(unit, texture, osg::StateAttribute::ON);
        stateSet->addUniform(new osg::Uniform((texture->getName() + "Sampler").c_str(), unit));
    }
    samplers3D.clear();
}


// Note: The first targets size is used to setup the camera, and 
// it is required that all targets have the same dimensions.
    
void AtmospherePrecompute::render2D(
    osgViewer::CompositeViewer *viewer
,   osg::Geode *geode
,   t_tex2DsByUnit &targets2D
,   t_tex2DsByUnit &samplers2D
,   t_tex3DsByUnit &samplers3D
,   t_uniforms &uniforms
,   const char *fragmentShaderSource)
{
    assert(targets2D.size() > 0);

    t_tex2DsByUnit::const_iterator i2;
    const t_tex2DsByUnit::const_iterator t2End = targets2D.end();

    i2 = targets2D.begin();
    const int width  = i2->second->getTextureWidth();
    const int height = i2->second->getTextureHeight();

    for(i2 = targets2D.begin(); i2 != t2End; ++i2)
    {
        assert(i2->second->getTextureWidth()  == width);
        assert(i2->second->getTextureHeight() == height);
    }
        
    // Setup graph

    osg::Group *group = setupGroup(viewer);
    osg::Program *program(setupProgram(fragmentShaderSource));

    osg::StateSet *ss(group->getOrCreateStateSet());
    ss->setAttributeAndModes(program);

    assignSamplers(ss, samplers2D, samplers3D);
    assignUniforms(ss, uniforms);

    // Setup local camera

    osg::ref_ptr<osg::Camera> camera = setupCamera(width, height, geode, 0);
    group->addChild(camera.get());
        

    // Assign Textures and Samplers

    for(i2 = targets2D.begin(); i2 != t2End; ++i2)
    {
        if(i2->second->getImage())
            camera->attach(static_cast<osg::Camera::BufferComponent>(osg::Camera::COLOR_BUFFER0 + i2->first), i2->second->getImage());
        else
            camera->attach(static_cast<osg::Camera::BufferComponent>(osg::Camera::COLOR_BUFFER0 + i2->first), i2->second);
    }

    //

    viewer->frame(); // Render single frame
    cleanUp(viewer);

    dirtyTargets(targets2D);
    targets2D.clear();
}


void AtmospherePrecompute::render3D(
    osgViewer::CompositeViewer *viewer
,   osg::Geode *geode
,   t_tex3DsByUnit &targets3D
,   t_tex2DsByUnit &samplers2D
,   t_tex3DsByUnit &samplers3D
,   t_uniforms &uniforms
,   const char *fragmentShaderSource)
{
    assert(targets3D.size() > 0);

    t_tex3DsByUnit::const_iterator i3;
    const t_tex3DsByUnit::const_iterator t3End = targets3D.end();

    i3 = targets3D.begin();
    const int width  = i3->second->getTextureWidth();
    const int height = i3->second->getTextureHeight();
    const int depth  = i3->second->getTextureDepth();

    for(i3 = targets3D.begin(); i3 != t3End; ++i3)
    {
        assert(i3->second->getTextureWidth()  == width);
        assert(i3->second->getTextureHeight() == height);
        assert(i3->second->getTextureDepth()  == depth);
    }

    // Setup graph

    osg::Group *group = setupGroup(viewer);
    osg::Program *program(setupProgram(fragmentShaderSource));

    osg::StateSet *ss(group->getOrCreateStateSet());
    ss->setAttributeAndModes(program);

    assignSamplers(ss, samplers2D, samplers3D);
    assignUniforms(ss, uniforms);

    // 

    for(int layer = 0; layer < depth; ++layer)
    {
        // Setup local camera

        osg::ref_ptr<osg::Camera> camera = setupCamera(width, height, geode, layer);
        group->addChild(camera.get());

        setupLayerUniforms(camera->getOrCreateStateSet(), depth, layer);

        // Assign Textures and Samplers

        for(i3 = targets3D.begin(); i3 != t3End; ++i3)
        {
            if(i3->second->getImage())
            {
                // workaround: use a slice here instead of the whole image, since osg does not support this directly...
                osg::Image *slice = getLayerFrom3DImage(i3->second->getImage(), layer);
                camera->attach(static_cast<osg::Camera::BufferComponent>(osg::Camera::COLOR_BUFFER0 + i3->first), slice);
            }
            else
                camera->attach(static_cast<osg::Camera::BufferComponent>(osg::Camera::COLOR_BUFFER0 + i3->first), i3->second, 0U, layer);
        }
    }
    viewer->frame(); // Render single frame
    cleanUp(viewer);

    dirtyTargets(targets3D);
    targets3D.clear();
}


const char *AtmospherePrecompute::substituteMacros(const char *source)
{
    // Replace Precomputed Texture Config "MACROS"

    const t_preTexCfg &tc(getTextureConfig());

    std::string temp(source);

    replace(temp, "%TRANSMITTANCE_W%", tc.transmittanceWidth);
    replace(temp, "%TRANSMITTANCE_H%", tc.transmittanceHeight);

    replace(temp, "%SKY_W%", tc.skyWidth);
    replace(temp, "%SKY_H%", tc.skyHeight);

    replace(temp, "%RES_R%", tc.resR);
    replace(temp, "%RES_MU%", tc.resMu);
    replace(temp, "%RES_MU_S%", tc.resMuS);
    replace(temp, "%RES_NU%", tc.resNu);

    replace(temp, "%TRANSMITTANCE_INTEGRAL_SAMPLES%", tc.transmittanceIntegralSamples);
    replace(temp, "%INSCATTER_INTEGRAL_SAMPLES%", tc.inscatterIntegralSamples);
    replace(temp, "%IRRADIANCE_INTEGRAL_SAMPLES%", tc.irradianceIntegralSamples);
    replace(temp, "%INSCATTER_SPHERICAL_INTEGRAL_SAMPLES%", tc.inscatterSphericalIntegralSamples);

    // Replace Physical Model Config "MACROS"

    const t_modelCfg &mc(getModelConfig());

    replace(temp, "%AVERAGE_GROUND_REFLECTANCE%", mc.avgGroundReflectance);

    replace(temp, "%HR%", mc.HR);
    replace(temp, "%betaR%", mc.betaR);
        
    replace(temp, "%HM%", mc.HM);
    replace(temp, "%betaMSca%", mc.betaMSca);
    replace(temp, "%betaMEx%", mc.betaMEx);
    replace(temp, "%mieG%", mc.mieG);

    return temp.c_str();
}

} // namespace osgHimmel