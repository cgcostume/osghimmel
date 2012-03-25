
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

#include "perlinmapgenerator.h"

#include "noise.h"

#include <stdlib.h>
#include <math.h>
#include <assert.h>


namespace osgHimmel
{

template<typename T, int N>
void PerlinMapGenerator::generate(
    const int /*width*/
,   const int /*height*/
,   T * /*dest*/
,   const float /*scale*/
,   const e_NoiseType /*type*/
,   const int /*startFrequency*/
,   const int /*octaves*/
,   const bool /*normalize*/)
{
    //const unsigned int size = width * height;

    //if(size < 1) 
    //    return;

    //float *temp = new float[size * octaves];
    //float *temp2 = new float[size];

    //int frequency = 1 << startFrequency;

    //// Generate noise data for each octave.
    //for(int f = 0; f < octaves; ++f, frequency *= 2)
    //{
    //    SetNoiseFrequency(frequency);

    //    for(int y = 0; y < height; ++y) 
    //        for(int x = 0; x < width; ++x) 
    //        {
    //            float xf = static_cast<float>(x) / width * frequency;
    //            float yf = static_cast<float>(y) / height * frequency;

    //            temp[(y * width  + x) + size * f] = static_cast<float>(noise(xf, yf));
    //        }
    //}

    //// Generate image using noise data.

    //float *fo = new float[octaves];

    //for(int o = 0; o < octaves; ++o) 
    //    fo[o] = 1.f / static_cast<float>(1 << o);


    //float p, f, po, pf, minp = scale, maxp = 0.;

    //for(int i = 0; i < width * height; ++i) 
    //{
    //    p = 0.f;

    //    for(int o = 0; o < octaves; ++o) 
    //    {
    //        f = fo[o];

    //        po = temp[i + size * o];
    //        pf = f * po;

    //        switch(type)
    //        {
    //        case NT_Standard:
    //            p += o > 0 ? 0.f : po; 
    //            break;
    //        case NT_Cloud:
    //            p += pf; 
    //            break;
    //        case NT_CloudAbs:
    //            p += abs(static_cast<float>(pf)); 
    //            break;
    //        case NT_Wood:
    //            p += (pf * 8.f) - int(pf * 8.f); 
    //            break;
    //        case NT_Paper:
    //            p += po * po * (pf > 0 ? 1.f : -1.f); 
    //            break;
    //        };
    //    }

    //    p += 0.5;

    //    if(p > maxp) 
    //        maxp = p;
    //    if(p < minp) 
    //        minp = p;

    //    temp2[i] = p;
    //}

    //if(normalize)
    //{
    //    const float invF = scale / (maxp - minp);
    //    for(int i = 0; i < width * height; ++i) 
    //    {
    //        p = temp2[i];

    //        p -= minp;
    //        p *= invF;

    //        for(unsigned int n = 0; n < N; ++n)
    //            dest[i * N + n] = static_cast<T>(p);
    //    }
    //}
    //else
    //{
    //    for(int i = 0; i < width * height; ++i) 
    //    {
    //        p = temp2[i];
    //        p *= scale;

    //        for(unsigned int n = 0; n < N; ++n)
    //            dest[i * N + n] = static_cast<T>(p);
    //    }
    //}

    //delete[] temp;
    //delete[] temp2;
}


void PerlinMapGenerator::generate1f(
    const int width
,   const int height
,   float *dest
,   const e_NoiseType type
,   const int startFrequency
,   const int octaves
,   const bool normalize)
{
    generate<float, 1>(width, height, dest, 1.f, type, startFrequency, octaves, normalize);
}


void PerlinMapGenerator::generate1(
    const int width
,   const int height
,   unsigned char *dest
,   const e_NoiseType type
,   const int startFrequency
,   const int octaves
,   const bool normalize)
{
    generate<unsigned char, 1>(width, height, dest, 255.f, type, startFrequency, octaves, normalize);
}

} // namespace osgHimmel