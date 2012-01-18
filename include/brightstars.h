
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
#ifndef __BRIGHTSTARS_H__
#define __BRIGHTSTARS_H__

#include <vector>

// NOTE: Enabling this, slows down compilation a lot!
//#define BRIGHTSTARS_WRITETOFILE

// Structure based on the extended bright star catalogue.

struct s_BrightStar
{
    float Vmag;   // visual magnitude (mag)
    float RA;     // right ascension (decimal hours)
    float DE;     // declination (decimal degrees)
    float pmRA;   // proper annual motion in right ascension (decimal hours)
    float pmDE;   // proper annual motion in declination (decimal degrees)
    float sRGB_R; // approximated color, red value   ]0;1[
    float sRGB_G; // approximated color, green value ]0;1[
    float sRGB_B; // approximated color, blue value  ]0;1[
};


unsigned int brightstars_readFromFile(
    const char *fileName
,   std::vector<s_BrightStar> &brightStars);


#ifdef BRIGHTSTARS_WRITETOFILE

unsigned int brightstars_writeToFile(const char *fileName);

#endif // BRIGHTSTARS_WRITETOFILE

#endif // __STARS_H__