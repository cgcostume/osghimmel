
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

#include "common.h"


namespace osgHimmel
{

// From // JAVA REFERENCE IMPLEMENTATION OF IMPROVED NOISE - COPYRIGHT 2002 KEN PERLIN. (http://mrl.nyu.edu/~perlin/noise/)
// and (Improving Noise - Perlin - 2002) - http://mrl.nyu.edu/~perlin/paper445.pdf

// Noise on GPU: http://http.developer.nvidia.com/GPUGems/gpugems_ch05.html
// (Chapter 5. Implementing Improved Perlin Noise - Ken Perlin - GPU Gems - 2004) 
// and http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter26.html
// (Chapter 26. Implementing Improved Perlin Noise - Simon Green - GPU Gems 2 - 2005)

// Implementation taken and modified from: Stefan Gustavson:
/*
This code was irrevocably released into the public domain
by its original author, Stefan Gustavson, in January 2011.
Please feel free to use it for whatever you want.
Credit is appreciated where appropriate, and I also
appreciate being told where this code finds any use,
but you may do as you like. Alternatively, if you want
to have a familiar OSI-approved license, you may use
This code under the terms of the MIT license:

Copyright (C) 2004 by Stefan Gustavson. All rights reserved.
This code is licensed to you under the terms of the MIT license:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

const std::string glsl_fade
(
    PRAGMA_ONCE(fade, 

    "float fade(const float t)\n"
    "{\n"
    "    return t * t * t * (t * (t * 6 - 15) + 10);\n"
    "}")
);

const std::string glsl_noise2
(
    PRAGMA_ONCE(noise2,

    "float noise2(\n"
    "    const sampler2D perm\n"
    ",   const vec2 st\n"
    ",   const float fade)\n"
    "{\n"
    "    const float o  = 1.0 / %SIZE%;\n"
    "\n"
    "    vec2 i = o * floor(st);\n"
    "    vec2 f = fract(st);\n"
    "\n"
    "    vec2 AA = texture2D(perm, i).xy               * 256.0 - 1.0;\n"
    "    vec2 BA = texture2D(perm, i + vec2( o, 0)).xy * 256.0 - 1.0;\n"
    "    vec2 AB = texture2D(perm, i + vec2( 0, o)).xy * 256.0 - 1.0;\n"
    "    vec2 BB = texture2D(perm, i + vec2( o, o)).xy * 256.0 - 1.0;\n"
    "\n"
    "    float dAA = dot(AA, f              );\n"
    "    float dBA = dot(BA, f - vec2( 1, 0));\n"
    "    float dAB = dot(AB, f - vec2( 0, 1));\n"
    "    float dBB = dot(BB, f - vec2( 1, 1));\n"
    "\n"
    "    vec2 t = mix(vec2(dAA, dAB), vec2(dBA, dBB), fade(f.x));\n"
    "\n"
    "    return mix(t.x, t.y, fade(f.y));\n"
    "}")
);

} // namespace osgHimmel