
// Copyright (c) 2011, Daniel Müller <dm@g4t3.de>
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
#ifndef __PROCEDURALHIMMEL_H__
#define __PROCEDURALHIMMEL_H__

#include "abstracthimmel.h"


class ProceduralHimmel : public AbstractHimmel
{
public:

    ProceduralHimmel();
    virtual ~ProceduralHimmel();

    const long double setLatitude(const long double latitude);
    const long double getLatitude() const;

    const long double setLongitude(const long double longitude);
    const long double getLongitude() const;

    const float setDitheringMultiplier(const float multiplier);
    const float getDitheringMultiplier() const;

protected:

    virtual void update();

    // abstract interface

    virtual const std::string getVertexShaderSource();
    virtual const std::string getFragmentShaderSource();

protected:

    long double m_latitude;
    long double m_longitude;

    osg::ref_ptr<osg::Uniform> u_sun;
    osg::ref_ptr<osg::Uniform> u_moon;

    osg::ref_ptr<osg::Uniform> u_ditheringMultiplier;
};

#endif // __PROCEDURALHIMMEL_H__