
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

#include "starsgeode.h"

#include "proceduralhimmel.h"
#include "shadermodifier.h"
#include "abstractastronomy.h"
#include "brightstars.h"

#include "mathmacros.h"

#include <osg/Geometry>
#include <osg/Point>
#include <osg/BlendFunc>


StarsGeode::StarsGeode(const ProceduralHimmel &himmel)
:   osg::Geode()
,   m_himmel(himmel)

,   m_program(new osg::Program)
,   m_vShader(new osg::Shader(osg::Shader::VERTEX))
,   m_gShader(new osg::Shader(osg::Shader::GEOMETRY))
,   m_fShader(new osg::Shader(osg::Shader::FRAGMENT))

,   u_starWidth(NULL)
{
    setName("Stars");

    osg::StateSet* stateSet = getOrCreateStateSet();

    setupNode(stateSet);
    setupUniforms(stateSet);
    setupShader(stateSet);
    setupTextures(stateSet);
};


StarsGeode::~StarsGeode()
{
};



void StarsGeode::update()
{
    float fov = m_himmel.getCameraFovHint();
    float height = m_himmel.getViewSizeHeightHint();

    u_starWidth->set(static_cast<float>(tan(_rad(fov) / height) * 2.f));
}


void StarsGeode::setupUniforms(osg::StateSet* stateSet)
{
    u_starWidth = new osg::Uniform("starWidth", 0.0004f); // TODO: change to zero! -> can cause graphics driver to crash if > 0.1
    stateSet->addUniform(u_starWidth);
}


#include "coords.h"
#include "earth.h"
#include "sideraltime.h"
#include "stars.h"


void StarsGeode::createAndAddDrawable()
{
    osg::Geometry *g = new osg::Geometry;
    addDrawable(g);

    std::vector<s_BrightStar> bss;
    brightstars_readFromFile("resources/brightstars", bss);

    // TODO: use actual time
    t_aTime aTime(2012, 17, 1, 12, 0, 0);

    // TODO: replace this by the equ to horizontal matrix
    float o = earth_trueObliquity(jd(aTime));
    float s = siderealTime(aTime);
    float la = 52.5000;
    float lo = 13.4167;


    osg::Vec4Array* cAry = new osg::Vec4Array;
    osg::Vec4Array* vAry = new osg::Vec4Array;

    for(int i = 0; i < bss.size(); ++i)
    {
        t_equf equ;
        equ.right_ascension = _rightascd(bss[i].RA, 0, 0);
        equ.declination = bss[i].DE;

        osg::Vec3f vec = equ.toHorizontal(s, la, lo).toEuclidean();
        vAry->push_back(osg::Vec4(vec, bss[i].Vmag));
        cAry->push_back(osg::Vec4(bss[i].sRGB_R, bss[i].sRGB_G, bss[i].sRGB_B, 1.0));
    }

    g->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    g->setColorArray(cAry);
    g->setVertexArray(vAry);

    g->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vAry->size()));


    // If things go wrong, fall back to big points.
    g->getOrCreateStateSet()->setAttribute(new osg::Point(2));
}


void StarsGeode::setupNode(osg::StateSet* stateSet)
{
    createAndAddDrawable();


    osg::BlendFunc *blend  = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE);
    stateSet->setAttributeAndModes(blend, osg::StateAttribute::ON);

    stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);

    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
}


void StarsGeode::setupShader(osg::StateSet* stateSet)
{
    m_vShader->loadShaderSourceFromFile("D:/p/osghimmel/tempsv.txt");
    m_gShader->loadShaderSourceFromFile("D:/p/osghimmel/tempsg.txt");
    m_fShader->loadShaderSourceFromFile("D:/p/osghimmel/tempsf.txt");

    m_program->addShader(m_vShader);
    m_program->addShader(m_gShader);
    m_program->addShader(m_fShader);

    stateSet->setAttributeAndModes(m_program, osg::StateAttribute::ON);

#ifdef OSGHIMMEL_ENABLE_SHADERMODIFIER
    if(m_himmel.shaderModifier())
    {
        m_himmel.shaderModifier()->registerShader(getName(), m_fShader);
        m_himmel.shaderModifier()->registerShader(getName(), m_gShader);
        m_himmel.shaderModifier()->registerShader(getName(), m_vShader);
    }
#endif // OSGHIMMEL_ENABLE_SHADERMODIFIER
}


void StarsGeode::setupTextures(osg::StateSet* stateSet)
{

}