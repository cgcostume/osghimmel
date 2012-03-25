
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


#include "noise.h"

#include "mathmacros.h"
#include "strutils.h"

#include "shaderfragment/noise.h"

#include <osg/Vec2f>
#include <osg/Vec3f>
#include <osg/Vec4f>


namespace osgHimmel
{

// From // JAVA REFERENCE IMPLEMENTATION OF IMPROVED NOISE - COPYRIGHT 2002 KEN PERLIN. (http://mrl.nyu.edu/~perlin/noise/)
// and (Improving Noise - Perlin - 2002) - http://mrl.nyu.edu/~perlin/paper445.pdf

const unsigned char Noise::m_perm[PERMSIZE] = 
{ 
    151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};


const float Noise::m_grad[16][3] =
{
        { 1.f, 1.f, 0.f}, {-1.f, 1.f, 0.f}, { 1.f,-1.f, 0.f}, {-1.f,-1.f, 0.f}
    ,   { 1.f, 0.f, 1.f}, {-1.f, 0.f, 1.f}, { 1.f, 0.f,-1.f}, {-1.f, 0.f,-1.f}
    ,   { 0.f, 1.f, 1.f}, { 0.f,-1.f, 1.f}, { 0.f, 1.f,-1.f}, { 0.f,-1.f,-1.f}
    ,   { 1.f, 1.f, 0.f}, {-1.f, 1.f, 0.f}, { 0.f,-1.f, 0.f}, { 0.f,-1.f,-1.f}
};


Noise::Noise(const unsigned int size)
:   m_size(size)
{
}


const unsigned int Noise::hash(
    const unsigned int x
,   const unsigned int y) const
{
    return m_perm[(m_perm[x & MAXPERMINDEX] + y) & MAXPERMINDEX];
}


const osg::Vec2f Noise::grad2(
    const unsigned int x
,   const unsigned int y) const
{
    const unsigned char p = hash(x, y);
    return osg::Vec2f(
        m_grad[p & 0xf][0]
    ,   m_grad[p & 0xf][1]);
}


const osg::Vec3f Noise::grad3(
    const unsigned int x
,   const unsigned int y) const
{
    const unsigned char p = hash(x, y);
    return osg::Vec3f(
        m_grad[p & 0xf][0]
    ,   m_grad[p & 0xf][1]
    ,   m_grad[p & 0xf][2]);
}


const osg::Vec4f Noise::grad3h(
    const unsigned int x
,   const unsigned int y) const
{
    const unsigned char p = hash(x, y);
    return osg::Vec4f(
        m_grad[p & 0xf][0]
    ,   m_grad[p & 0xf][1]
    ,   m_grad[p & 0xf][2]
    ,   p);
}


void Noise::generatePermutationMap(unsigned char *dest) const
{
    const unsigned int size2 = m_size * m_size;

    if(size2 < 1) 
        return;

    for(unsigned int x = 0; x < m_size; ++x)
        for(unsigned int y = 0; y < m_size; ++y)
        {
            const unsigned int o = 4 * (y * m_size + x);
            osg::Vec4f  g(grad3h(x, y));
            dest[o + 0] = static_cast<unsigned char>(g[0] + 1);
            dest[o + 1] = static_cast<unsigned char>(g[1] + 1);
            dest[o + 2] = static_cast<unsigned char>(g[2] + 1);
            dest[o + 3] = static_cast<unsigned char>(g[3]);
        }
}


const float Noise::fade(const float t)
{
    return t * t * t * (t * (t * 6.f - 15.f) + 10.f);
}


const float Noise::mix(
    const float x
,   const float y
,   const float a)
{
    return x * (1.f - a) + y * a;
}


const osg::Vec2f Noise::mix(
    const osg::Vec2f &x
,   const osg::Vec2f &y
,   const float a)
{
    return x * (1.f - a) + y * a;
}


const float Noise::noise2(
    const float s
,   const float t) const
{
    //const float o10 = 1.0 / static_cast<float>(m_size);
    //const float o05 = 0.5 / static_cast<float>(m_size);

    const float is = floor(s);
    const float it = floor(t);

    const osg::Vec2f f(_frac(s), _frac(t));

    // range [-1;+1]

    const osg::Vec2f aa = grad2(is + 0, it + 0);
    const osg::Vec2f ba = grad2(is + 1, it + 0);
    const osg::Vec2f ab = grad2(is + 0, it + 1);
    const osg::Vec2f bb = grad2(is + 1, it + 1);

    const float daa = osg::Vec2f(aa[0], aa[1]) * (f                       );
    const float dba = osg::Vec2f(ba[0], ba[1]) * (f - osg::Vec2f(1.f, 0.f));
    const float dab = osg::Vec2f(ab[0], ab[1]) * (f - osg::Vec2f(0.f, 1.f));
    const float dbb = osg::Vec2f(bb[0], bb[1]) * (f - osg::Vec2f(1.f, 1.f));

    const osg::Vec2f i = mix(osg::Vec2f(daa, dab), osg::Vec2f(dba, dbb), fade(f[0]));

    return mix(i[0], i[1], fade(f[1]));
}




const char* Noise::fadeGlslSource()
{
    return glsl_fade.c_str();
}


const char* Noise::noise2GlslSource(const unsigned int size)
{
    std::string glsl = glsl_noise2;
    replace(glsl, "%SIZE%", static_cast<float>(size));

    return glsl.c_str();
}

} // namespace osgHimmel