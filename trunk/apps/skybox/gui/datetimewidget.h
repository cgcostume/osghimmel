
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
#ifndef __DATETIMEWIDGET_H__
#define __DATETIMEWIDGET_H__


#include <QWidget>

class Ui_DateTimeWidget;

class QTimer;
class QDateTime;

class TimeF;
class AbstractHimmelScene;


class DateTimeWidget 
:    public QWidget
{
    Q_OBJECT

public:
    DateTimeWidget(
        TimeF &timef
    ,   QWidget *parent = NULL);

    virtual ~DateTimeWidget();


    void setScene(AbstractHimmelScene *scene);

public slots:

    void cycle();
    void pause();
    void stop();

protected slots:

    void on_dateTimeEdit_dateTimeChanged(const QDateTime &datetime);

    void on_applyPushButton_clicked(bool checked = false);
    void on_cyclePushButton_toggled(bool checked);

    void on_secondsPerCycleDoubleSpinBox_valueChanged(double value);

    void on_timeSlider_valueChanged(int value);

    void on_latitudeDoubleSpinBox_valueChanged(double d);
    void on_latitudeLineEdit_editingFinished();
    
    void on_longitudeDoubleSpinBox_valueChanged(double d);
    void on_longitudeLineEdit_editingFinished();
   
    void on_presetComboBox_currentIndexChanged(int index);


    void me_timeout();

protected:

    void autoApply();

    const bool isCycling() const;

	const float getTimeSliderF() const;


    static const double latitudeFromText(const QString &text);
    static const double longitudeFromText(const QString &text);

protected:
    std::auto_ptr<Ui_DateTimeWidget> m_ui;

    TimeF &m_timef;
    AbstractHimmelScene *m_scene;

    QTimer *m_timer;

    bool m_presetChanged;
};

#endif // __DATETIMEWIDGET_H__