
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

#include "logoutputlabel.h"

#include <QMouseEvent>
#include <QDockWidget>
#include <QTimer>
#include <QTextDocument>


LogOutputLabel::LogOutputLabel(QDockWidget *output, QWidget* parent)
:	LogOutput()
,	QLabel(parent)
,	_output(output)
,	_timer(new QTimer())
,	_readyShowHelpShown(false)
,	_readyHideHelpShown(false)
{
	_ready = readyString();

	if(_output)
	{
		_output->hide();
		setToolTip(tr("click to show/hide log"));
	}

	// Interpret all text as plain text so html tags with formatting and line breaks don't cause the status bar to grow in size
	setTextFormat(Qt::PlainText);

	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	connect(_timer, SIGNAL(timeout()), this, SLOT(timedout()));
	_timer->setInterval(4000);
	_timer->setSingleShot(true);
}

LogOutputLabel::~LogOutputLabel()
{
	delete _timer;
}

void LogOutputLabel::print(const LogEntry &entry)
{
	QString message = entry.message();
	
	message = message.left(message.indexOf("\n"));
	
	switch(entry.type())
	{
	case LogEntry::WARNING_LOG:			
		setText(QString("warning: %1").arg(message));
		if(_output && _output->isVisible())
			_timer->start();
		else
			_timer->stop();

		break;
	case LogEntry::ERROR_LOG:
		setText(QString("error: %1").arg(message));
		if(_output && _output->isVisible())
			_timer->start();
		else
			_timer->stop();

		break;
	case LogEntry::MESSAGE_LOG:
	case LogEntry::INFORMATION_LOG:
		setText(QString("%1").arg(message));
		_timer->start();
		break;
	case LogEntry::DEBUG_LOG:
		setText(QString("debug: %1").arg(message));
		_timer->start();
		break;
	case LogEntry::WORKFLOW_LOG:
	case LogEntry::UNDEFINED_LOG:
	default:
		break;
	};
}

void LogOutputLabel::mousePressEvent(QMouseEvent *event)
{
	if(_output)
	{
		_output->setVisible(!_output->isVisible());
		timedout();
	}

	return QLabel::mousePressEvent(event);
}

void LogOutputLabel::timedout()
{
	setText(readyString());
}

inline const QString LogOutputLabel::readyString()
{
	if(_output)
	{
		if(_output->isVisible() && !_readyHideHelpShown)
		{
			_readyHideHelpShown = true;
			return QString("ready (click to hide log...)");
		}
		else if(_output->isHidden() && !_readyShowHelpShown)
		{
			_readyShowHelpShown = true;
			return QString("ready (click to show log...)");
		}
	}
	return QString("ready");
}