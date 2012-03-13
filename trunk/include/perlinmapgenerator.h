
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
#ifndef __PERLIN_MAP_GENERATOR_H__
#define __PERLIN_MAP_GENERATOR_H__


class PerlinMapGenerator
{
public:

    enum e_NoiseType
    {
        NT_Standard
    ,   NT_Cloud
    ,   NT_CloudAbs
    ,   NT_Wood
    ,   NT_Paper
    };

public:

  static void generate1f(
        const int width
    ,   const int height
    ,   float *dest
    ,   const e_NoiseType type = NT_Standard
    ,   const int startFrequency = 3
    ,   const int octaves = 5
    ,   const bool normalize = true);

  static void generate1(
        const int width
    ,   const int height
    ,   unsigned char *dest
    ,   const e_NoiseType type = NT_Standard
    ,   const int startFrequency = 3
    ,   const int octaves = 5
    ,   const bool normalize = true);

protected:

    template<typename T, int N>
    static void generate(
        const int width
    ,   const int height
    ,   T *dest
    ,   const float scale
    ,   const e_NoiseType type
    ,   const int startFrequency
    ,   const int octaves
    ,   const bool normalize);
};

#endif // __PERLIN_MAP_GENERATOR_H__