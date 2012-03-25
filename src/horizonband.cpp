
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

#include "horizonband.h"


namespace osgHimmel
{

HorizonBand::HorizonBand()
:   u_params(NULL)
,   u_color(NULL)
,   u_bottomColor(NULL)
{
}


HorizonBand::~HorizonBand()
{
};


void HorizonBand::initialize(osg::StateSet *stateSet)
{
    if(!stateSet)
        return;

    u_params = new osg::Uniform("hbandParams", 
        osg::Vec3(defaultScale(), defaultWidth(), defaultOffset()));

    u_color       = new osg::Uniform("hbandColor", defaultColor());
    u_bottomColor = new osg::Uniform("hbandBackground", defaultBottomColor());

    stateSet->addUniform(u_params);
    stateSet->addUniform(u_color);
    stateSet->addUniform(u_bottomColor);
};


const osg::Vec3 HorizonBand::getParams() const
{
    osg::Vec3 params;
    u_params->get(params);

    return params;
}


const float HorizonBand::setScale(const float scale)
{
    osg::Vec3 params = getParams();
    params[0] = scale;

    u_params->set(params);
    return getScale();
}

const float HorizonBand::getScale() const
{
    osg::Vec3 params = getParams();
    return params[0];
}
const float HorizonBand::defaultScale()
{
    return 0.1f;
}


const float HorizonBand::setWidth(const float thickness)
{
    osg::Vec3 params = getParams();
    params[1] = thickness;

    u_params->set(params);
    return getWidth();
}

const float HorizonBand::getWidth() const
{
    osg::Vec3 params = getParams();
    return params[1];
}
const float HorizonBand::defaultWidth()
{
    return 0.0f;
}


const float HorizonBand::setOffset(const float offset)
{
    osg::Vec3 params = getParams();
    params[2] = offset;

    u_params->set(params);
    return getOffset();
}

const float HorizonBand::getOffset() const
{
    osg::Vec3 params = getParams();
    return params[2];
}
const float HorizonBand::defaultOffset()
{
    return 0.f;
}


const osg::Vec4 HorizonBand::setColor(const osg::Vec4 &color)
{
    u_color->set(color);
    return getColor();
}

const osg::Vec4 HorizonBand::getColor() const
{
    osg::Vec4 color;
    u_color->get(color);

    return color;
}
const osg::Vec4 HorizonBand::defaultColor()
{
    return osg::Vec4(0.25f, 0.25f, 0.25f, 1.f);
}


const osg::Vec4 HorizonBand::setBottomColor(const osg::Vec4 &color)
{
    u_bottomColor->set(color);
    return getBottomColor();
}

const osg::Vec4 HorizonBand::getBottomColor() const
{
    osg::Vec4 bottomColor;
    u_bottomColor->get(bottomColor);

    return bottomColor;
}
const osg::Vec4 HorizonBand::defaultBottomColor()
{
    return osg::Vec4(0.08f, 0.08f, 0.08f, 1.f);
}

} // namespace osgHimmel