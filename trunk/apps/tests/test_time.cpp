
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


#include "test_time.h"

#include "include/atime.h"
#include "include/timef.h"
#include "include/mathmacros.h"

#include <time.h>

void test_time()
{
    t_aTime aTime(2011, 12, 2, 0, 42, 24);
    time_t t = aTime.timet();

    TimeF timef(t);

    ASSERT_EQ(__int64, t, timef.gett());

    timef.setf(0.0, true);
    ASSERT_EQ(__int64, t - 42 * 60 - 24, timef.gett());
    
    timef.setf(_day( 0, 42, 24), true);
    ASSERT_EQ(__int64, t, timef.gett());

    timef.setf(_day(24, 42, 24), true);
    ASSERT_EQ(__int64, t, timef.gett());

    timef.setf(_day(0, 42, 23), true);
    ASSERT_EQ_NOT(__int64, t, timef.gett());
    timef.setf(_day(0, 42, 25), true);
    ASSERT_EQ_NOT(__int64, t, timef.gett());



    TEST_REPORT();
}
