
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

#include "pseudo_rand.h"

#include "common.h"


namespace osgHimmel
{

// The generated pseudo random number is good for frame-to-frame 
// coherent dithering.

// Two static, pseudo random signals, one moving vertically and one
// horizontally over time, are overlapped. If frame-to-frame coherence
// is required, the seed should be based on time or frame count.

// The algorithm was inspired and partially relies on techniques 
// described in: http://zfx.info/viewtopic.php?f=11&t=8#p19
// and http://www.geeks3d.com/20100831/shader-library-noise-and-pseudo-random-number-generator-in-glsl/

// The exact value of most of the used constant values where gathered 
// by trial and error and benchmarked by subjective perception.

const std::string glsl_pseudo_rand
(
    PRAGMA_ONCE(pseudo_rand,

    "float pseudo_rand(\n"
    "    const vec2 i\n"
    ",   const int seed)\n"
    "{\n"
    "    int i1 = int(i.x + i.y * 1733);\n"
    "    i1 = (i1 << 7) ^ i1 + seed;\n"  // seed
    "\n"
    "    int i2 = int(i.y + i.x * 1103);\n"
    "    i2 = (i2 << 7) ^ i2 + seed;\n"  // seed
    "\n"
    "    i1 ^= i2;\n"
    "    return 1.0 - float((i1 * (i1 * i1 * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;\n"
    "}")
);

} // namespace osgHimmel