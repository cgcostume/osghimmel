
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

#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <iostream>

#ifdef WIN32
#include <windows.h> // for message box
#endif

#include <QFile>
#include <QTextStream>

#ifndef WIN32
#include <stdio.h>
#else
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif


LogEntry::LogEntry(e_LogEntryType type, const QString &message)
:    _type(type)
,    _message(message)
,    _timestamp(QDateTime::currentDateTime())
{
}


LogEntry::LogEntry(const LogEntry &entry)
:    _type(entry._type)
,    _message(entry._message)
,    _timestamp(entry._timestamp)
,    _file(entry._file)
,    _line(entry._line)
{
}


LogEntry::~LogEntry()
{
}





void StdLogOutput::print(const LogEntry &entry)
{
    //const QString timestamp(entry.timestamp().toString("yy-MM-dd::hh:mm:ss"));
    const QString timestamp(entry.timestamp().toString("hh:mm:ss"));

    switch(entry.type())
    {
    case LogEntry::DEBUG_LOG:
        std::cout << QString("%1 %2 %3: %4").arg(timestamp).arg(entry.file()).arg(entry.line()).arg(entry.message()).toStdString() << std::endl;
        break;
    case LogEntry::ERROR_LOG:
        std::cout << QString("%1 error: %2").arg(timestamp).arg(entry.message()).toStdString() << std::endl;
        break;
    case LogEntry::INFORMATION_LOG:
    case LogEntry::MESSAGE_LOG:
    case LogEntry::WORKFLOW_LOG:
        std::cout << QString("%1: %2").arg(timestamp).arg(entry.message()).toStdString() << std::endl;
        break;
    case LogEntry::WARNING_LOG:
        std::cout << QString("%1 warning: %2").arg(timestamp).arg(entry.message()).toStdString() << std::endl;
        break;
    case LogEntry::UNDEFINED_LOG:
    default:
        break;
    }
}





FileLogOutput::FileLogOutput(const QString &filename)
:    _opened(false)
,   _stream(NULL)
{
    _file = new QFile(filename);

#ifndef WIN32
    FILE* file = fopen(filename.toLocal8Bit(), "wx+");
    if(file)
        _opened = _file->open(file, QFile::WriteOnly);
#else
    int fhandle = _sopen(filename.toLocal8Bit(), O_CREAT | O_WRONLY | O_TEXT, SH_DENYWR, S_IWRITE);
    if(fhandle != -1)
        _opened = _file->open(fhandle, QFile::WriteOnly);
#endif

    if(_opened)
        _stream = new QTextStream(_file);    
    else
        throw std::exception(TR("Cannot open log file %1 in write mode.").arg(filename).toLocal8Bit());
}


FileLogOutput::~FileLogOutput()
{
    _file->close();
    delete _file;
}


void FileLogOutput::print(const LogEntry &entry)
{
    const QString timestamp(entry.timestamp().toString("yy-MM-dd::hh:mm:ss"));
    //const QString timestamp(entry.timestamp().toString("hh:mm:ss"));

    switch(entry.type())
    {
    case LogEntry::INFORMATION_LOG:
        (*_stream) << QString("%1:         %2\n").arg(timestamp).arg(entry.message());
        break;
    case LogEntry::WORKFLOW_LOG:
        (*_stream) << QString("%1:       f %2\n").arg(timestamp).arg(entry.message());
        break;
    case LogEntry::WARNING_LOG:
        (*_stream) << QString("%1 warning: %2\n").arg(timestamp).arg(entry.message());
        break;
    case LogEntry::ERROR_LOG:            
        (*_stream) << QString("%1 error:   %2\n").arg(timestamp).arg(entry.message());
        break;
    case LogEntry::DEBUG_LOG:            
        (*_stream) << QString("%1 %2 %3: %4\n").arg(timestamp).arg(entry.file()).arg(entry.line()).arg(entry.message());
        break;
    case LogEntry::MESSAGE_LOG:    
    case LogEntry::UNDEFINED_LOG:
    default:
        break;
    };

    _stream->flush();
}




LogDispatcher::LogDispatcher()
:    _cache(true)
{
}


LogDispatcher::~LogDispatcher()
{
    foreach(LogEntry *entry, _cachedEntries)
        delete entry;

    _cachedEntries.clear();
}


void LogDispatcher::append(const LogEntry &entry)
{
    if(_cache)
        cache(entry);

    foreach(LogOutput *output, _outputs)
        output->print(entry);
}


void LogDispatcher::cache(const LogEntry &entry)
{
    if(_cache)
        _cachedEntries.append(new LogEntry(entry));
}


void LogDispatcher::flushCache(LogOutput *output)
{
    foreach(LogEntry *entry, _cachedEntries)
        output->print(*entry);
}


void LogDispatcher::attachLogOutput(LogOutput *output)
{
    if(output && !_outputs.contains(output))
    {
        _outputs.insert(output);
        flushCache(output);
    }
}


void LogDispatcher::detachLogOutput(LogOutput *output)
{
    if(output && _outputs.contains(output))
        _outputs.remove(output);
}


void LogDispatcher::stopCaching()
{
    _cache = false;
}






//! this macro parses all arguments of the append functions 
//  and stores the value into the const std::string msg
#define PARSE_ARGS(MSG_IN, MSG_OUT)                    \
                                                       \
    va_list    argp;                                   \
    va_start(argp, MSG_IN);                            \
                                                       \
    char* MSG_OUT = (char*)malloc(sizeof(char) * 512); \
    vsprintf(MSG_OUT, MSG_IN, argp);


Log::Log()
:    _dispatcher(NULL)
{
}


Log::~Log()
{
}


void Log::attachDispatcher(LogDispatcher *dispatcher)
{
    if(!dispatcher)
        return;

    _dispatcher = dispatcher;
    
    // forwards the whole log history to the new dispatcher
    while(!_earlyEntries.isEmpty())
    {
        LogEntry *entry(_earlyEntries.takeFirst());
        _dispatcher->append(*entry);
        delete entry;
    }
}


LogDispatcher *Log::dispatcher()
{
    return _dispatcher;
}


void Log::lock()
{
}


void Log::unlock()
{
}


void Log::debug(const char *file, const int line) 
{
    lock();

    _debug_file = QString::fromLatin1(file);
    _debug_line = line;

    unlock();
}


void Log::append(LogEntry::e_LogEntryType type, const QString &message, const bool debug)
{
    LogEntry *entry = new LogEntry(type, message);

    if(debug)
    {
        entry->setFile(_debug_file);
        entry->setLine(_debug_line);
    }

    if(_dispatcher)
    {
        _dispatcher->append(*entry);
        delete entry;
    }
    else
    {
        // cache items
        _earlyEntries << entry;
    }
}


// DEBUG LOGs

#ifdef _DEBUG

void Log::appendDebug(const char *message, ...) 
{

    lock();

    if(!message) 
        return;

    PARSE_ARGS(message, msg);
    append(LogEntry::DEBUG_LOG, QString(msg), true);
    free(msg);

    unlock();
}

void Log::appendDebug(const QString &message) 
{
    lock();

    if(message.isEmpty()) 
        return;

    append(LogEntry::DEBUG_LOG, message, true);

    unlock();
}


#else

void Log::appendDebug(const char*, ...) 
{
}


void Log::appendDebug(const QString&) 
{
}

#endif



// MESSAGE_LOGs

void Log::appendMessage(const char *message, ...) 
{
    if(!message) 
        return;

    PARSE_ARGS(message, msg);
    append(LogEntry::MESSAGE_LOG, QString(msg), false);
    free(msg);
}
void Log::appendMessage(const QString &message) 
{
    if(message.isEmpty()) 
        return;
    append(LogEntry::MESSAGE_LOG, message, false);
}


// INFORMATION LOGs

void Log::appendInformation(const char *message, ...) 
{
    if(!message) 
        return;

    PARSE_ARGS(message, msg);
    append(LogEntry::INFORMATION_LOG, QString(msg), false);
    free(msg);
}
void Log::appendInformation(const QString &message) 
{
    if(message.isEmpty()) 
        return;
    append(LogEntry::INFORMATION_LOG, message, false);
}


void Log::appendWorkflow(const char *message, ...) 
{
    if(!message) 
        return;

    PARSE_ARGS(message, msg);
    append(LogEntry::WORKFLOW_LOG, QString(msg), false);
    free(msg);
}
void Log::appendWorkflow(const QString &message) 
{
    if(message.isEmpty()) 
        return;
    append(LogEntry::WORKFLOW_LOG, message, false);
}


void Log::appendWarning(const char *message, ...) 
{
    if(!message) 
        return;

    PARSE_ARGS(message, msg);
    append(LogEntry::WARNING_LOG, QString(msg), false);
    free(msg);
}
void Log::appendWarning(const QString &message) 
{
    if(message.isEmpty()) 
        return;
    append(LogEntry::WARNING_LOG, message, false);
}


void Log::appendError(const char *message, ...) 
{
    if(!message) 
        return;

    PARSE_ARGS(message, msg);
    append(LogEntry::ERROR_LOG, QString(msg), false);
    free(msg);
}
void Log::appendError(const QString &message) 
{
    if(message.isEmpty()) 
        return;
    append(LogEntry::ERROR_LOG, message, false);
}




void ThreadSafeLog::lock()
{
    _mutex.lock();
}


void ThreadSafeLog::unlock()
{
    _mutex.unlock();
}




Log *GlobalLog::m_log = NULL;


GlobalLog::GlobalLog()
{
}


GlobalLog::~GlobalLog()
{
}


Log *GlobalLog::instance()
{
    if(!m_log)
        m_log = new GlobalLog();

    return m_log;
}
