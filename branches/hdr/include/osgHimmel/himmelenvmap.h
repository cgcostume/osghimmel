
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
#ifndef __HIMMELENVMAP_H__
#define __HIMMELENVMAP_H__

#include "declspec.h"

#include <osg/Group>
#include <osg/Camera>

namespace osg
{
    class TextureCubeMap;
}


namespace osgHimmel
{

class OSGH_API HimmelEnvMap : public osg::Group
{
public:

    HimmelEnvMap(const unsigned int texSize);
    virtual ~HimmelEnvMap();

    inline osg::TextureCubeMap *cubeMap()
    {
        return m_cubeMap;
    }

    // Redirect all Child Manipulators to this and m_group;

    virtual bool addChild(osg::Node *child);

    // untested...

    virtual bool insertChild(unsigned int index, Node *child);
    virtual bool removeChild(Node *child);
    virtual bool removeChild(unsigned int pos, unsigned int numChildrenToRemove = 1);
    virtual bool removeChildren(unsigned int pos, unsigned int numChildrenToRemove);
    virtual bool replaceChild(Node *origChild, Node *newChild);
    virtual unsigned int getNumChildren() const;
    virtual bool setChild(unsigned int i, Node *node);
    virtual Node *getChild(unsigned int i);
    virtual const Node *getChild(unsigned int i) const;
    virtual bool containsNode(const Node *node) const;
    virtual unsigned int getChildIndex(const Node *node) const;


protected:

    void setupCubeMap(const unsigned int texSize);
    void setupCameras();

public:
    typedef std::vector<osg::ref_ptr<osg::Camera> > t_cameras;

protected:

    osg::ref_ptr<osg::Group> m_group;
    osg::ref_ptr<osg::TextureCubeMap> m_cubeMap;

    t_cameras m_cameras;
};

} // namespace osgHimmel

#endif // __HIMMELENVMAP_H__