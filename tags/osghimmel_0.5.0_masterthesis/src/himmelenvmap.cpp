
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

#include "himmelenvmap.h"

#include <osg/TextureCubeMap>


namespace osgHimmel
{

class EnvMapCallback : public osg::NodeCallback
{
public:
    EnvMapCallback(HimmelEnvMap::t_cameras &cameras)
    :   m_cameras(cameras)
    {
    }

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
    {
        traverse(node, nv);

        const osg::Vec3 position = node->getBound().center();

        typedef std::pair<osg::Vec3, osg::Vec3> t_planeBase;
        const t_planeBase pb[6] =
        {
            t_planeBase(osg::Vec3( 1, 0, 0), osg::Vec3( 0,-1, 0))
        ,   t_planeBase(osg::Vec3(-1, 0, 0), osg::Vec3( 0,-1, 0))
        ,   t_planeBase(osg::Vec3( 0, 1, 0), osg::Vec3( 0, 0, 1))
        ,   t_planeBase(osg::Vec3( 0,-1, 0), osg::Vec3( 0, 0,-1))
        ,   t_planeBase(osg::Vec3( 0, 0, 1), osg::Vec3( 0,-1, 0))
        ,   t_planeBase(osg::Vec3( 0, 0,-1), osg::Vec3( 0,-1, 0)) 
        };

        for(unsigned int i = 0;  i < 6 && i < m_cameras.size(); ++i)
        {
            osg::Matrix localOffset;
            localOffset.makeLookAt(position, position + pb[i].first, pb[i].second);

            osg::Matrix viewMatrix = node->getWorldMatrices().front() * localOffset;

            m_cameras[i]->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
            m_cameras[i]->setProjectionMatrixAsFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, node->getBound().radius());
            m_cameras[i]->setViewMatrix(viewMatrix);
        }
    }

protected:
    virtual ~EnvMapCallback() 
    { 
    }

protected:
    HimmelEnvMap::t_cameras m_cameras;
};




HimmelEnvMap::HimmelEnvMap(const unsigned int texSize)
:   osg::Group()
,   m_group(new osg::Group())
{
    setupCubeMap(texSize);
    setupCameras();

    setUpdateCallback(new EnvMapCallback(m_cameras));
}


HimmelEnvMap::~HimmelEnvMap()
{
}


void HimmelEnvMap::setupCubeMap(const unsigned int texSize)
{
    m_cubeMap = new osg::TextureCubeMap;
    m_cubeMap->setTextureSize(texSize, texSize);

    m_cubeMap->setInternalFormat(GL_RGB);

    m_cubeMap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    m_cubeMap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    m_cubeMap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

    m_cubeMap->setFilter(osg::TextureCubeMap::MIN_FILTER, osg::TextureCubeMap::LINEAR);
    m_cubeMap->setFilter(osg::TextureCubeMap::MAG_FILTER, osg::TextureCubeMap::LINEAR);
}


void HimmelEnvMap::setupCameras()
{
    for(unsigned int i = 0; i < 6; ++i)
    {
        osg::Camera *camera = new osg::Camera;

        camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera->setViewport(0, 0, m_cubeMap->getTextureWidth(), m_cubeMap->getTextureHeight());
        camera->setRenderOrder(osg::Camera::PRE_RENDER);

        // Tell the camera to use OpenGL frame buffer object where supported.
        camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);

        // Attach the texture and use it as the color buffer.
        camera->attach(osg::Camera::COLOR_BUFFER, m_cubeMap, 0, i);

        camera->addChild(m_group.get());
        osg::Group::addChild(camera);

        m_cameras.push_back(camera);
    }
}


// osg::Group child manipulation wrapper:

bool HimmelEnvMap::addChild(osg::Node *child)
{
    osg::Group::addChild(child);
    return m_group->addChild(child);
}


bool HimmelEnvMap::insertChild(unsigned int index, Node *child)
{
    osg::Group::insertChild(index + 6, child);
    return m_group->insertChild(index, child);
}


bool HimmelEnvMap::removeChild(Node *child)
{
    osg::Group::removeChild(child);
    return m_group->removeChild(child);
}


bool HimmelEnvMap::removeChild(unsigned int pos, unsigned int numChildrenToRemove)
{
    osg::Group::removeChild(pos + 6, numChildrenToRemove);
    return m_group->removeChild(pos, numChildrenToRemove);
}


bool HimmelEnvMap::removeChildren(unsigned int pos, unsigned int numChildrenToRemove)
{
    osg::Group::removeChildren(pos + 6, numChildrenToRemove);
    return m_group->removeChildren(pos, numChildrenToRemove);
}


bool HimmelEnvMap::replaceChild(Node *origChild, Node *newChild)
{
    osg::Group::replaceChild(origChild, newChild);
    return m_group->replaceChild(origChild, newChild);
}


unsigned int HimmelEnvMap::getNumChildren() const
{
    return m_group->getNumChildren();
}


bool HimmelEnvMap::setChild(unsigned int i, Node *node)
{
    osg::Group::setChild(i + 6, node);
    return m_group->setChild(i, node);
}


osg::Node *HimmelEnvMap::getChild(unsigned int i)
{
    return m_group->getChild(i);
}


const osg::Node *HimmelEnvMap::getChild(unsigned int i) const
{
    return m_group->getChild(i);
}


bool HimmelEnvMap::containsNode(const Node *node) const
{
    return m_group->containsNode(node);
}


unsigned int HimmelEnvMap::getChildIndex(const Node *node) const
{
    return m_group->getChildIndex(node);
}

} // namespace osgHimmel