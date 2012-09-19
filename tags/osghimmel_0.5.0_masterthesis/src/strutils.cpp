
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

#include "strutils.h"

#include <stdio.h>
#include <stdarg.h>


namespace osgHimmel
{

void replace(
    std::string& string
,   const std::string &search
,   const int value)
{
    // length of value for char buffer:

    const int n = __max(1, ceil(log10(static_cast<float>(value))));
    char *buffer = new char[n + 1];

    __itoa(value, buffer, n + 1);

    replace(string, search, buffer);

    delete[] buffer;
}


void replace(
    std::string& string
,   const std::string &search
,   const float value)
{
    char buffer[64];

    if(sprintf_s(buffer, 64, "%f", value))
        replace(string, search, buffer);
}


void replace(
    std::string& string
,   const std::string &search
,   const osg::Vec3f value)
{
    char buffer[202];

    if(sprintf_s(buffer, 202, "vec3(%f, %f, %f)", value[0], value[1], value[2]))
        replace(string, search, buffer);
}


void replace(
    std::string& string
,   const std::string &search
,   const std::string &replace)
{
    std::string::size_type next;

    for(next = string.find(search); next != std::string::npos; next = string.find(search, next))
    {
        string.replace(next, search.length(), replace);
        next += replace.length();
    }
    
}


int __itoa(
    int value
,   char *dst
,   const size_t size)
{
    int result(-1);

#ifdef __GNUC__
    result = snprintf(dst, size, "%d", value);
#else // __GNUC__
    result = _itoa_s(value, dst, size, 10);
#endif // __GNUC__

    dst[size - 1] = '\0';
    return result;
}

} // namespace osgHimmel