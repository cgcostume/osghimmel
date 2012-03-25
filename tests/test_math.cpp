
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


#include "test_math.h"

#include "test.h"

#include "osgHimmel/mathmacros.h"


using namespace osgHimmel;

void test_math()
{
    // Check if int cast truncates.
    ASSERT_EQ(int, 1, static_cast<int>( 1.66));
    ASSERT_EQ(int, 1, static_cast<int>( 1.33));
    ASSERT_EQ(int,-1, static_cast<int>(-1.33));
    ASSERT_EQ(int,-1, static_cast<int>(-1.66));

    // Check decimal hours.
    ASSERT_AB(long double, 0.81, _day(19, 26, 24), 0.000001);


    ASSERT_EQ(long double, 1.0, _mod( 1.0, 3.0));
    ASSERT_EQ(long double, 1.5, _mod( 1.5, 3.0));
    ASSERT_EQ(long double, 2.9999, _mod( 2.9999, 3.0));

    ASSERT_EQ(long double, 0.0, _mod( 3.0, 3.0));
    ASSERT_EQ(long double, 0.0, _mod( 0.0, 1.0));
    ASSERT_EQ(long double, 0.5, _mod( 1.5, 1.0));
    ASSERT_EQ(long double, 0.5, _mod(-1.5, 1.0));
    ASSERT_EQ(long double, 0.4, _mod(-0.6, 1.0));

    ASSERT_EQ(long double, 10.0, _mod(1234, 24.0));

    ASSERT_AB(long double, 13.225388, _hours(198.38082), 0.000001);
    ASSERT_AB(long double, _hour(13, 13, 31.4), _hours( 198.38082), 0.000001);
    ASSERT_AB(long double, _hour(13, 13, 31.4), _hours(-161.61918), 0.000001);

    ASSERT_AB(long double, -7.78507, -_decimal(7, 47, 06), 0.0001);

    TEST_REPORT();
}
