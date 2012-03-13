
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


#include "test_time.h"

#include "test.h"

#include "include/atime.h"
#include "include/timef.h"
#include "include/mathmacros.h"

#include <time.h>

void test_time()
{
    {
        t_aTime aTime(2011, 12, 2, 0, 42, 24, static_cast<short>(+1.11 * 3600));
        time_t t = aTime.toTime_t();

        TimeF f(t, 0);

        ASSERT_EQ(float, 0.029444444, f.getf());
        ASSERT_EQ(__int64, t, f.gett());

        f.setf(0.0, true);
        ASSERT_EQ(__int64, t - 42 * 60 - 24, f.gett());

        f.setf(_day( 0, 42, 24), true);
        ASSERT_EQ(__int64, t, f.gett());

        f.setf(_day(24, 42, 24), true);
        ASSERT_EQ(__int64, t, f.gett());

        f.setf(_day(0, 42, 23), true);
        ASSERT_EQ_NOT(__int64, t, f.gett());
        f.setf(_day(0, 42, 25), true);
        ASSERT_EQ_NOT(__int64, t, f.gett());
    }

    {
        time_t temp = 0;
        struct tm ttm;

        ttm.tm_isdst = 0;

        ttm.tm_mday = 3;
        ttm.tm_mon = 0;
        ttm.tm_year = 2012 - 1900;

        ttm.tm_min  = 0;
        ttm.tm_sec  = 0;
        ttm.tm_hour = 12;


        time_t t = mktime(&ttm);

        TimeF f(t, static_cast<time_t>(-3.33 * 3600), 0.0L);


        ASSERT_EQ(float, 0.5, f.getf());

        ASSERT_EQ(__int64, t, f.gett());


        t_aTime a;

        a = t_aTime::fromTimeF(f);

        ASSERT_EQ(short, a.year,   ttm.tm_year + 1900);
        ASSERT_EQ(short, a.month,  ttm.tm_mon + 1);
        ASSERT_EQ(short, a.day,    ttm.tm_mday);
        ASSERT_EQ(short, a.hour,   ttm.tm_hour);
        ASSERT_EQ(short, a.minute, ttm.tm_min);
        ASSERT_EQ(short, a.second, ttm.tm_sec);

    
        f.setf(0.5);
        a = t_aTime::fromTimeF(f);

        ASSERT_EQ(short, a.hour,   ttm.tm_hour);
        ASSERT_EQ(short, a.minute, ttm.tm_min);
        ASSERT_EQ(short, a.second, ttm.tm_sec);

        f.update();
        a = t_aTime::fromTimeF(f);

        ASSERT_EQ(short, a.hour,   ttm.tm_hour);
        ASSERT_EQ(short, a.minute, ttm.tm_min);
        ASSERT_EQ(short, a.second, ttm.tm_sec);

        f.setf(0.22, true);
        a = t_aTime::fromTimeF(f);

        ASSERT_EQ(__int64, t - (0.5 - 0.22) * 3600 * 24, f.gett());

        ASSERT_EQ(short, a.hour,   5);
        ASSERT_EQ(short, a.minute, 16);
        ASSERT_EQ(short, a.second, 48);
    }
    
    TEST_REPORT();
}
