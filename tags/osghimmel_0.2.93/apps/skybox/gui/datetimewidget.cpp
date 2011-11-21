
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

#include "datetimewidget.h"
#include "apps/skybox/ui_datetimewidget.h"

#include "include/timef.h"

#include <QTimer>

#include <assert.h>


DateTimeWidget::DateTimeWidget(
    TimeF &timef
,   QWidget* parent)
:   QWidget(parent)
,   m_ui(new Ui_DateTimeWidget)
,   m_timef(timef)
,   m_timer(new QTimer())
{
    m_ui->setupUi(this);

    m_ui->secondsPerCycleDoubleSpinBox->setValue(m_timef.getSecondsPerCycle());

    const QDateTime dt(QDateTime::fromTime_t(m_timef.gett()));
    m_ui->dateTimeEdit->setDateTime(dt);

    me_timeout();

    connect(m_timer, SIGNAL(timeout()), this, SLOT(me_timeout()));
}


DateTimeWidget::~DateTimeWidget()
{
    delete m_timer;
    m_timer = NULL;
}


void DateTimeWidget::cycle()
{
    m_ui->timeSlider->setEnabled(false);
    m_ui->cyclePushButton->setChecked(true);

    static const int msec = 16; // > 60fps
    m_timer->start(msec);

    m_timef.run();
}


void DateTimeWidget::pause()
{
    if(m_ui->cyclePushButton->isChecked())
        m_ui->cyclePushButton->setChecked(false);

    m_ui->timeSlider->setEnabled(true);

    m_timer->stop();

	m_timef.pause();
	m_timef.reset();

	m_timef.setf(getTimeSliderF(), true);
}


void DateTimeWidget::on_applyPushButton_clicked(bool checked)
{
    const time_t t(m_ui->dateTimeEdit->dateTime().toTime_t());

    m_timef.sett(t, true);
    me_timeout();
}


void DateTimeWidget::on_cyclePushButton_toggled(bool checked)
{
    checked ? cycle() : pause();
}


void DateTimeWidget::me_timeout()
{
    m_timef.update();

    const int t = m_timef.getf() * (m_ui->timeSlider->maximum() - m_ui->timeSlider->minimum()) + m_ui->timeSlider->minimum();
    m_ui->timeSlider->setValue(t);
}


const bool DateTimeWidget::isCycling() const
{
    return m_ui->cyclePushButton->isChecked();
}


void DateTimeWidget::on_secondsPerCycleDoubleSpinBox_valueChanged(double value)
{
    m_timef.setSecondsPerCycle(value);
}


void DateTimeWidget::on_timeSlider_valueChanged(int value)
{
    if(isCycling())
        return;

    pause();
}


const float DateTimeWidget::getTimeSliderF() const
{
    return static_cast<float>(m_ui->timeSlider->value() - m_ui->timeSlider->minimum()) 
        / (m_ui->timeSlider->maximum() - m_ui->timeSlider->minimum());
}
