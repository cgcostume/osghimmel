
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
#ifndef __HORIZONBAND_H__
#define __HORIZONBAND_H__

#include "declspec.h"

#include <osg/StateSet>


namespace osgHimmel
{

// Some texture projections (polar mapping with MM_Half and paraboloid mapping)
// discard the lower hemisphere, resulting in a hard edge to gl clear color. 
// To avoid this, these himmels offer a gradient called HorizonBand. This is 
// given by scale, width, and offset as well as a color and a bottom color. 
// The scale is in `[`0;1`[`, with 1.0 overlapping the skies vertical 180 degrees. 
// Width specifies the vertical region uniformly filled with color. Currently 
// smoothstep blending is used for the gradient, but inv_squared interpolation 
// yields good results too.

class OSGH_API HorizonBand
{
public:

    HorizonBand();
    virtual ~HorizonBand();

    void initialize(osg::StateSet *stateSet);

    const float setScale(const float scale);
    const float getScale() const;
    static const float defaultScale();

    const float setWidth(const float thickness);
    const float getWidth() const;
    static const float defaultWidth();

    const float setOffset(const float offset);
    const float getOffset() const;
    static const float defaultOffset();

    const osg::Vec4f setColor(const osg::Vec4f &color);
    const osg::Vec4f getColor() const;
    static const osg::Vec4f defaultColor();

    const osg::Vec4f setBottomColor(const osg::Vec4f &color);
    const osg::Vec4f getBottomColor() const;
    static const osg::Vec4f defaultBottomColor();

protected:
    const osg::Vec3f getParams() const;

protected:

    osg::ref_ptr<osg::Uniform> u_params; // Vec3 { 0: Scale, 1: Width, 2: Offset }

    osg::ref_ptr<osg::Uniform> u_color; // Vec4
    osg::ref_ptr<osg::Uniform> u_bottomColor; // Vec4
};

} // namespace osgHimmel

#endif // __HORIZONBAND_H__