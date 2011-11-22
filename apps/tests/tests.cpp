
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


#include "abstracttest.h"

#include "test_astronomy.h"


#include <iostream>
#include <vector>


typedef std::vector<AbstractTest*> t_tests;

int main(int argc, char* argv[])
{
    t_tests tests;
    tests.push_back(new test_Astronomy());

    t_tests initFailed;
    t_tests runFailed;

    t_tests::const_iterator i;
    t_tests::const_iterator iEnd;


    iEnd = tests.end();
    for(i = tests.begin(); i != iEnd; ++i)
    {
        if(!(*i)->initialize())
        {
            initFailed.push_back(*i);
            continue;
        }

        if(!(*i)->run())
        {
            runFailed.push_back(*i); 
            continue;
        }
    }


    if(initFailed.size() + runFailed.size() > 0)
    {
        std::cout << std::endl << std::endl << "test(s) failed:" << std::endl;

        iEnd = initFailed.end();
        for(i = initFailed.begin(); i != iEnd; ++i)
            std::cout << "Initialization of " << (*i)->identifier() << " has failed" << std::endl;

        iEnd = runFailed.end();
        for(i = runFailed.begin(); i != iEnd; ++i)
            std::cout << (*i)->identifier() << " failed" << std::endl;    

        std::cout << std::endl << std::endl << initFailed.size() + runFailed.size() << " test(s) failed" << std::endl << std::endl;
    }
    else
        std::cout << std::endl << std::endl << "All tests did successfully run." << std::endl << std::endl;


    return 0;
}