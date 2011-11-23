
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


#include "test.h"

#include <math.h>
#include <iostream>
#include <sstream>

#include <assert.h>

namespace
{

#define REPORT_TRUE(file) \
    do { test_succeeded(file); } while(false)

#define REPORT_TRUE_EQ(file) \
    REPORT_TRUE(file)


#define REPORT_FALSE(file, line, expected, expectedExpr, actual, actualExpr, operatorExpr) \
    do { test_failed(file); \
    \
    std::stringstream stream; \
    \
    stream.setf(std::ios::fixed, std::ios::floatfield); \
    stream.precision(20); \
    \
    stream << "FAILED:  " << expectedExpr.c_str() << " == " << actualExpr \
           << " -> " << expected << " " << operatorExpr << " " << actual \
           << std::endl << "in " << file << "(" << line << ")"; \
    \
    s_reportsByFile[file].push_back(stream.str()); } while(false)

#define REPORT_FALSE_EQ(file, line, expected, expectedExpr, actual, actualExpr) \
    REPORT_FALSE(file, line, expected, expectedExpr, actual, actualExpr, "==")

}


Test::t_reportsByFile Test::s_reportsByFile;

Test::t_intByFile Test::s_succeeded;
Test::t_intByFile Test::s_failed;


void Test::report(const std::string &file)
{
    if(s_failed.find(file) == s_failed.end())
    {
        assert(s_succeeded.find(file) == s_succeeded.end());
        return;
    }


    const int failed = s_failed[file];
    const int succeeded = s_succeeded[file];

    if(succeeded + failed == 0)
        return;

    if(failed)
    {
        std::cout << std::endl << "--------" << std::endl 
            << "REPORT:  " << failed << " of " << succeeded + failed 
            << " Tests Failed." << std::endl << std::endl;

        // reports available
        assert(s_reportsByFile.find(file) != s_reportsByFile.end());

        t_report &report = s_reportsByFile[file];

        if(report.empty())
            return;

        for(int i = 0; i < report.size(); ++i)
            std::cout << report[i] << std::endl << std::endl;
    }
    else
        std::cout << std::endl << "--------" << std::endl 
            << "REPORT:  All " << succeeded << " Tests Passed." << std::endl << std::endl;
}


void Test::test(const std::string &file)
{
  if(s_succeeded.find(file) != s_succeeded.end()
    || s_failed.find(file) != s_failed.end())
    return;
    
    std::cout << std::endl << "TEST:    " << file << "" << std::endl 
        << "--------" << std::endl;

    s_succeeded[file] = 0;
    s_failed[file] = 0;
}


void Test::test_succeeded(const std::string &file)
{
    test(file);
    ++s_succeeded[file];

    std::cout << ".";
}


void Test::test_failed(const std::string &file)
{
    test(file);
    ++s_failed[file];

    std::cout << "#";
}


template<typename T>
const bool Test::assert_eq_t(
    const std::string file
,   const int line
,   const T expected
,   const std::string &expected_string
,   const T actual
,   const std::string &actual_string
,   const T max_allowed_distance)
{
    const bool success(max_allowed_distance ? abs(expected - actual) < max_allowed_distance : expected == actual);

    if(success)
        REPORT_TRUE_EQ(file);
    else
        REPORT_FALSE_EQ(file, line, expected, expected_string, actual, actual_string);

    return success;
}


const bool Test::assert_eq(
    const std::string file
,   const int line
,   const long double expected
,   const std::string expected_string
,   const long double actual
,   const std::string actual_string
,   const long double max_allowed_distance)
{
    return assert_eq_t<double>(file, line, expected, expected_string, actual, actual_string, max_allowed_distance);
}

