
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

#include "mainlogdispatcher.h"

#include "gui/logoutputlabel.h"
#include "gui/logoutputwidget.h"

#include <QStringList>

MainLogDispatcher::MainLogDispatcher()
:    LogDispatcher()
,    m_widgetOutput(NULL)
,    m_labelOutput(NULL)
,    m_fileOutput(NULL)
#ifndef NDEBUG
,    m_stdOutput(NULL)
#endif
{

#ifndef NDEBUG
    m_stdOutput = new StdLogOutput;
    attachLogOutput(m_stdOutput);
#endif

    const QString filePath("");
    QStringList fileSegs = QString("%1.log").arg(QString(APPLICATION_NAME).toLower()).split(".");
    QString fileName;

    int i = 0;
    while(true)
    {
        try
        {
            fileName = filePath + fileSegs[0] + (i > 0 ? QString("%1.").arg(i + 1) : ".") + fileSegs[1];
            m_fileOutput = new FileLogOutput(fileName);
            break;
        }
        catch(const std::exception &e)
        {
            ++i;

            delete m_fileOutput;
            m_fileOutput = NULL;

            _LOG_WARNING(e.what());
            if(i < 8 /* 8 parallel instances should be enough...*/)
                continue;
        }
        _LOG_ERROR(TR("Could not create any log file in %1").arg(fileName));
        break;
    }

    if(m_fileOutput)
    {
        _LOG_INFO(TR("Logs are additionally written to %1.").arg(fileName));
        attachLogOutput(m_fileOutput);
    }
}


MainLogDispatcher::~MainLogDispatcher()
{
    detachLogOutput(m_fileOutput);
    detachLogOutput(m_widgetOutput);
#ifndef NDEBUG
    detachLogOutput(m_stdOutput);
#endif
    delete m_fileOutput;
}


void MainLogDispatcher::setOutputWidget(LogOutputWidget *widget)
{
    detachLogOutput(m_widgetOutput);
    m_widgetOutput = widget;

    if(m_widgetOutput)
        attachLogOutput(m_widgetOutput);

    stopCaching();
}


void MainLogDispatcher::setOutputLabel(LogOutputLabel *label)
{
    detachLogOutput(m_labelOutput);
    m_labelOutput = label;

    if(m_labelOutput)
        attachLogOutput(m_labelOutput);
}