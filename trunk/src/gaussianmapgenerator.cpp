
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

#include "gaussianmapgenerator.h"

#include "mathmacros.h"

#include <stdlib.h>
#include <assert.h>


template<typename T, int N>
void GaussianMapGenerator::generate(
    const int width
,   const int height
,   T *dest
,   const float scale
,   const float amount
,   const float probability
,   const int seed)
{
    const unsigned int size = width * height;

    if(size < 1) 
        return;

    for(unsigned int i = 0; i < size * N; ++i)
        dest[i] = static_cast<T>(1.f * scale);

    srand(seed);


    const float p = probability * 2.f;

    for(unsigned int j = 0; j < p * size; ++j)
    {
        const int k(_rand(0, width  - 1));
        const int l(_rand(0, height - 1));

        const int i(l * width + k);

        const T v = static_cast<T>(_randf(scale - scale * amount, scale));

        for(unsigned int n = 0; n < N; ++n)
            dest[i * N + n] = v;
    }
}


void GaussianMapGenerator::generate1f(
    const int width
,   const int height
,   float *dest
,   const float amount
,   const float probability
,   const int seed)
{
    generate<float, 1>(width, height, dest, 1.f, amount, probability, seed);
}


void GaussianMapGenerator::generate3f(
    const int width
,   const int height
,   float *dest
,   const float amount
,   const float probability
,   const int seed)
{
    generate<float, 3>(width, height, dest, 1.f, amount, probability, seed);
}


void GaussianMapGenerator::generate4f(
    const int width
,   const int height
,   float *dest
,   const float amount
,   const float probability
,   const int seed)
{
    generate<float, 4>(width, height, dest, 1.f, amount, probability, seed);
}


void GaussianMapGenerator::generate1(
    const int width
,   const int height
,   unsigned char *dest
,   const float amount
,   const float probability
,   const int seed)
{
    generate<unsigned char, 1>(width, height, dest, 255.f, amount, probability, seed);
}


void GaussianMapGenerator::generate3(
    const int width
,   const int height
,   unsigned char *dest
,   const float amount
,   const float probability
,   const int seed)
{
    generate<unsigned char, 3>(width, height, dest, 255.f, amount, probability, seed);
}


void GaussianMapGenerator::generate4(
    const int width
,   const int height
,   unsigned char *dest
,   const float amount
,   const float probability
,   const int seed)
{
    generate<unsigned char, 4>(width, height, dest, 255.f, amount, probability, seed);
}