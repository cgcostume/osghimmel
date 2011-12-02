
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
#ifndef __TEST_H__
#define __TEST_H__

#include <string>
#include <vector>
#include <map>

#define TEST_REPORT() \
    Test::report(__FILE__);

#define ASSERT_EQ(T, expected, actual) \
    Test::assert_eq(__FILE__, __LINE__, static_cast<T>(expected), #expected, static_cast<T>(actual), #actual)

#define ASSERT_EQ_NOT(T, expected, actual) \
    Test::assert_eq_not(__FILE__, __LINE__, static_cast<T>(expected), #expected, static_cast<T>(actual), #actual)

#define ASSERT_AB(T, expected, actual, max_allowed_difference) \
    Test::assert_eq(__FILE__, __LINE__, static_cast<T>(expected), #expected, static_cast<T>(actual), #actual, static_cast<T>(max_allowed_difference))

#define ASSERT_AB_NOT(T, expected, actual, max_allowed_difference) \
    Test::assert_eq_not(__FILE__, __LINE__, static_cast<T>(expected), #expected, static_cast<T>(actual), #actual, static_cast<T>(max_allowed_difference))

namespace
{
#define ASSERT_EQ_DECL(T) \
\
    static const bool assert_eq( \
        const std::string file \
    ,   const int line \
    ,   const T expected \
    ,   const std::string expected_string \
    ,   const T actual \
    ,   const std::string actual_string \
    ,   const T max_allowed_distance = 0)

#define ASSERT_EQ_NOT_DECL(T) \
\
    static const bool assert_eq_not( \
        const std::string file \
    ,   const int line \
    ,   const T expected \
    ,   const std::string expected_string \
    ,   const T actual \
    ,   const std::string actual_string \
    ,   const T max_allowed_distance = 0)
}


class Test
{
public:

    static void report(const std::string &file);

    ASSERT_EQ_DECL(unsigned short);
    ASSERT_EQ_NOT_DECL(unsigned short);

    ASSERT_EQ_DECL(short);
    ASSERT_EQ_NOT_DECL(short);

    ASSERT_EQ_DECL(unsigned int);
    ASSERT_EQ_NOT_DECL(unsigned int);

    ASSERT_EQ_DECL(int);
    ASSERT_EQ_NOT_DECL(int);

    ASSERT_EQ_DECL(float);
    ASSERT_EQ_NOT_DECL(float);

    ASSERT_EQ_DECL(double);
    ASSERT_EQ_NOT_DECL(double);

    ASSERT_EQ_DECL(long double);
    ASSERT_EQ_NOT_DECL(long double);

    ASSERT_EQ_DECL(__int64);
    ASSERT_EQ_NOT_DECL(__int64);

protected:

    template<typename T>
    static const bool assert_eq_t(
        const std::string file
    ,   const int line
    ,   const T expected
    ,   const std::string &expected_string
    ,   const T actual
    ,   const std::string &actual_string
    ,   const T max_allowed_distance);

    template<typename T>
    static const bool assert_eq_not_t(
        const std::string file
    ,   const int line
    ,   const T expected
    ,   const std::string &expected_string
    ,   const T actual
    ,   const std::string &actual_string
    ,   const T max_allowed_distance);


    static void report_true_eq();
    static void report_false_eq(
        const std::string file
    ,   const int line
    ,   const std::string &expectedExpr
    ,   const std::string &expected
    ,   const std::string &actualExpr
    ,   const std::string &actual);

    static void test(const std::string &file);

    static void test_succeeded(const std::string &file);
    static void test_failed(const std::string &file);

protected:

    typedef std::vector<std::string> t_report;
    typedef std::map<std::string, t_report> t_reportsByFile;

    typedef std::map<std::string, unsigned int> t_intByFile;

    static t_reportsByFile s_reportsByFile;

    static t_intByFile s_succeeded;
    static t_intByFile s_failed;
};

#endif // __TEST_H__