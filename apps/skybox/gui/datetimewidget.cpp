
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

#include "datetimewidget.h"
#include "apps/skybox/ui_datetimewidget.h"
#include "abstracthimmelscene.h"

#include "include/timef.h"
#include "include/mathmacros.h"


#include <QTimer>
#include <QRegExp>

#include <assert.h>


DateTimeWidget::DateTimeWidget(
    TimeF &timef
,   QWidget* parent)
:   QWidget(parent)
,   m_ui(new Ui_DateTimeWidget)
,   m_timer(new QTimer())
,   m_presetChanged(false)
,   m_timef(timef)
,   m_scene(NULL)
{
    m_ui->setupUi(this);

    m_ui->secondsPerCycleDoubleSpinBox->setValue(m_timef.getSecondsPerCycle());

    QDateTime dt(QDateTime::fromTime_t(m_timef.gett()));
    dt.setUtcOffset(0);

    m_ui->dateTimeEdit->setDateTime(dt);
    m_ui->utcOffsetDoubleSpinBox->setValue(m_timef.getUtcOffset() / 3600);

    me_timeout();

    connect(m_timer, SIGNAL(timeout()), this, SLOT(me_timeout()));


    // fill presets
    const QStringList presets = QStringList()
        << ";00°00'N:000°00'E;0"
        << "Aberdeen, Scotland;57°09'N:002°09'W;0"
        << "Adelaide, Australia;34°55'S:138°36'E;+9.5"
        << "Algiers, Algeria;36°50'N:003°00'E;+1"
        << "Amsterdam, Netherlands;52°22'N:004°53'E;+1"
        << "Ankara, Turkey;39°55'N:032°55'E;+2"
        << "Asunción, Paraguay;25°15'S:057°40'W;-4"
        << "Athens, Greece;37°58'N:023°43'E;+2"
        << "Auckland, New Zealand;36°52'S:174°45'E;+13"
        << "Bangkok, Thailand;13°45'N:100°30'E;+7"
        << "Barcelona, Spain;41°23'N:002°09'E;+1"
        << "Beijing, China;39°55'N:116°25'E;+8"
        << "Belém, Brazil;01°28'S:048°29'W;-3"
        << "Belfast, Northern Ireland;54°37'N:005°56'W;0"
        << "Belgrade, Serbia;44°52'N:020°32'E;+1"
        << "Berlin, Germany;52°30'N:013°25'E;+1"
        << "Birmingham, England;52°25'N:001°55'W;0"
        << "Bogotá, Colombia;04°32'N:074°15'W;-5"
        << "Bombay, India;19°00'N:072°48'E;+5.5"
        << "Bordeaux, France;44°50'N:000°31'W;+1"
        << "Bremen, Germany;53°05'N:008°49'E;+1"
        << "Brisbane, Australia;27°29'S:153°08'E;+10"
        << "Bristol, England;51°28'N:002°35'W;0"
        << "Brussels, Belgium;50°52'N:004°22'E;+1"
        << "Bucharest, Romania;44°25'N:026°07'E;+2"
        << "Budapest, Hungary;47°30'N:019°05'E;+1"
        << "Buenos Aires, Argentina;34°35'S:058°22'W;+1"
        << "Cairo, Egypt;30°02'N:031°21'E;+2"
        << "Calcutta, India;22°34'N:088°24'E;+5.5"
        << "Canton, China;23°07'N:113°15'E;+8"
        << "Cape Town, South Africa;33°55'S:018°22'E;+2"
        << "Caracas, Venezuela;10°28'N:067°02'W;-4.5"
        << "Cayenne, French Guiana;04°49'N:052°18'W;-3"
        << "Chihuahua, Mexico;28°37'N:106°05'W;-7"
        << "Chongqing, China;29°46'N:106°34'E;+8"
        << "Copenhagen, Denmark;55°40'N:012°34'E;+1"
        << "Córdoba, Argentina;31°28'S:064°10'W;-3"
        << "Dakar, Senegal;14°40'N:017°28'W;0"
        << "Darwin, Australia;12°28'S:130°51'E;+9.5"
        << "Djibouti, Djibouti;11°30'N:043°03'E;+3"
        << "Dublin, Ireland;53°20'N:006°15'W;0"
        << "Durban, South Africa;29°53'S:030°53'E;+2"
        << "Edinburgh, Scotland;55°55'N:003°10'W;0"
        << "Frankfurt, Germany;50°07'N:008°41'E;+1"
        << "Georgetown, Guyana;06°45'N:058°15'W;-4"
        << "Glasgow, Scotland;55°50'N:004°15'W;0"
        << "Guatemala City, Guatemala;14°37'N:090°31'W;-6"
        << "Guayaquil, Ecuador;02°10'S:079°56'W;-5"
        << "Hamburg, Germany;53°33'N:010°02'E;+1"
        << "Hammerfest, Norway;70°38'N:023°38'E;+1"
        << "Havana, Cuba;23°08'N:082°23'W;-5"
        << "Helsinki, Finland;60°10'N:025°00'E;+2"
        << "Hobart, Tasmania;42°52'S:147°19'E;+10"
        << "Hong Kong, China;22°20'N:114°11'E;+8"
        << "Iquique, Chile;20°10'S:070°07'W;-4"
        << "Irkutsk, Russia;52°30'N:104°20'E;+9"
        << "Jakarta, Indonesia;06°16'S:106°48'E;+7"
        << "Johannesburg, South Africa;26°12'S:028°04'E;+2"
        << "Kingston, Jamaica;17°59'N:076°49'W;-5"
        << "Kinshasa, Congo;04°18'S:015°17'E;+1"
        << "Kuala Lumpur, Malaysia;03°08'N:101°42'E;+8"
        << "La Paz, Bolivia;16°27'S:068°22'W;-4"
        << "Leeds, England;53°45'N:001°30'W;0"
        << "Lima, Peru;12°00'S:077°02'W;-5"
        << "Lisbon, Portugal;38°44'N:009°09'W;0"
        << "Liverpool, England;53°25'N:003°00'W;0"
        << "London, England;51°32'N:000°05'W;0"
        << "Lyon, France;45°45'N:004°50'E;+1"
        << "Madrid, Spain;40°26'N:003°42'W;+1"
        << "Manchester, England;53°30'N:002°15'W;0"
        << "Manila, Philippines;14°35'N:120°57'E;+8"
        << "Marseille, France;43°20'N:005°20'E;+1"
        << "Mazatlán, Mexico;23°12'N:106°25'W;-7"
        << "Mecca, Saudi Arabia;21°29'N:039°45'E;+3"
        << "Melbourne, Australia;37°47'S:144°58'E;+10"
        << "Mexico City, Mexico;19°26'N:099°07'W;-6"
        << "Milan, Italy;45°27'N:009°10'E;+1"
        << "Montevideo, Uruguay;34°53'S:056°10'W;-3"
        << "Moscow, Russia;55°45'N:037°36'E;+3"
        << "Munich, Germany;48°08'N:011°35'E;+1"
        << "Nagasaki, Japan;32°48'N:129°57'E;+9"
        << "Nagoya, Japan;35°07'N:136°56'E;+9"
        << "Nairobi, Kenya;01°25'S:036°55'E;+3"
        << "Nanjing (Nanking), China;32°03'N:118°53'E;+8"
        << "Naples, Italy;40°50'N:014°15'E;+1"
        << "New Delhi, India;28°35'N:077°12'E;+5.5"
        << "Newcastle upon Tyne, England;54°58'N:001°37'W;0"
        << "Odessa, Ukraine;46°27'N:030°48'E;+2"
        << "Osaka, Japan;34°32'N:135°30'E;+9"
        << "Oslo, Norway;59°57'N:010°42'E;+1"
        << "Panama City, Panama;08°58'N:079°32'W;-5"
        << "Paramaribo, Suriname;05°45'N:055°15'W;-3"
        << "Paris, France;48°48'N:002°20'E;+1"
        << "Perth, Australia;31°57'S:115°52'E;+8"
        << "Plymouth, England;50°25'N:004°05'W;0"
        << "Port Moresby, Papua New Guinea;09°25'S:147°08'E;+10"
        << "Prague, Czech Republic;50°05'N:014°26'E;+1"
        << "Rangoon, Myanmar;16°50'N:096°00'E;+6.5"
        << "Reykjavík, Iceland;64°04'N:021°58'W;-3"
        << "Rio de Janeiro, Brazil;22°57'S:043°12'W;-3"
        << "Rome, Italy;41°54'N:012°27'E;+1"
        << "Salvador, Brazil;12°56'S:038°27'W;-3"
        << "Santiago, Chile;33°28'S:070°45'W;-4"
        << "St. Petersburg, Russia;59°56'N:030°18'E;+3"
        << "São Paulo, Brazil;23°31'S:046°31'W;-3"
        << "Shanghai, China;31°10'N:121°28'E;+8"
        << "Singapore, Singapore;01°14'N:103°55'E;+8"
        << "Sofia, Bulgaria;42°40'N:023°20'E;+2"
        << "Stockholm, Sweden;59°17'N:018°03'E;+1"
        << "Sydney, Australia;34°00'S:151°00'E;+10"
        << "Tananarive, Madagascar;18°50'S:047°33'E;+3"
        << "Tehran, Iran;35°45'N:051°45'E;+3.5"
        << "Tokyo, Japan;35°40'N:139°45'E;+9"
        << "Tripoli, Libya;32°57'N:013°12'E;+2"
        << "Venice, Italy;45°26'N:012°20'E;+1"
        << "Veracruz, Mexico;19°10'N:096°10'W;-6"
        << "Vienna, Austria;48°14'N:016°20'E;+1"
        << "Vladivostok, Russia;43°10'N:132°00'E;+11"
        << "Warsaw, Poland;52°14'N:021°00'E;+1"
        << "Wellington, New Zealand;41°17'S:174°47'E;+12"
        << "Zürich, Switzerland;47°21'N:008°31'E;+1";

    foreach(const QString &preset, presets)
    {
        QStringList temp = preset.split(";");
        m_ui->presetComboBox->addItem(temp[0], temp[1] + ":" + temp[2]);
    }
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

    m_timef.start();
}


void DateTimeWidget::pause()
{
    if(m_ui->cyclePushButton->isChecked())
        m_ui->cyclePushButton->setChecked(false);

    m_ui->timeSlider->setEnabled(true);

    m_timer->stop();

    m_timef.pause();
}


void DateTimeWidget::stop()
{
    pause();
    m_timef.stop();
}


void DateTimeWidget::autoApply()
{
    if(m_ui->autoApplyPushButton->isChecked())
        on_applyPushButton_clicked();
}


void DateTimeWidget::on_dateTimeEdit_dateTimeChanged(const QDateTime &datetime)
{
    autoApply();
}


void DateTimeWidget::on_nowPushButton_clicked(bool checked)
{
    QDateTime lt(QDateTime::currentDateTime());
    QDateTime ut(lt.toUTC());

    m_ui->dateTimeEdit->setDateTime(lt);

    lt.setUtcOffset(0);
    m_ui->utcOffsetDoubleSpinBox->setValue(-lt.secsTo(ut) / 3600.0);
}


void DateTimeWidget::on_applyPushButton_clicked(bool)
{
    const time_t t(dateTime().toTime_t());

    stop();

    m_timef.setUtcOffset(m_ui->utcOffsetDoubleSpinBox->value() * 3600);
    m_timef.sett(t);

    if(m_scene && m_scene->hasLocationSupport())
    {
        m_scene->setLatitude(m_ui->latitudeDoubleSpinBox->value());
        m_scene->setLongitude(m_ui->longitudeDoubleSpinBox->value());

        const double a = m_scene->setAltitude(m_ui->altitudeDoubleSpinBox->value());
        m_ui->altitudeDoubleSpinBox->blockSignals(true);
        m_ui->altitudeDoubleSpinBox->setValue(a);
        m_ui->altitudeDoubleSpinBox->blockSignals(false);
    }

    me_timeout();
}


void DateTimeWidget::on_autoApplyPushButton_clicked(bool checked)
{
    if(checked)
        autoApply();
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

    // Don't change seconds fraction. Slider works on minutes.
    const long double seconds = _frac(m_timef.getf() * 1440.0) / 1440.0;

    m_timef.setf(timeSliderF() + seconds, true);
}


const float DateTimeWidget::timeSliderF() const
{
    return static_cast<float>(m_ui->timeSlider->value() - m_ui->timeSlider->minimum()) 
        / (m_ui->timeSlider->maximum() - m_ui->timeSlider->minimum());
}


const int DateTimeWidget::utcOffset() const
{
    const double hours = m_ui->utcOffsetDoubleSpinBox->value();
    const int seconds = hours * 60 * 60;

    return seconds;
}

const QDateTime DateTimeWidget::dateTime() const
{
    return m_ui->dateTimeEdit->dateTime();
}

void DateTimeWidget::on_utcOffsetDoubleSpinBox_valueChanged(double d)
{
    QString prefix("UTC");
    if(d >= 0.0)
        prefix.append("+");

    m_ui->utcOffsetDoubleSpinBox->setPrefix(prefix);
       
    autoApply();
}


void DateTimeWidget::on_latitudeDoubleSpinBox_valueChanged(double d)
{
    d = _clamp(-90.0, +90.0, d);

    const int h = _abs(d);
    const int m = _abs(_frac(d) * 60.0);
    const int s = _abs(_frac(d * 60.0) * 60.0);

    QString latitude = QString("%1°%2'%3'' %4")
        .arg(h, 2, 10, QChar('0'))
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0'))
        .arg(d < 0 ? "S" : "N");

    if(!m_presetChanged)
    {
        m_ui->presetComboBox->blockSignals(true);
        m_ui->presetComboBox->setCurrentIndex(0);
        m_ui->presetComboBox->blockSignals(false);
    }

    m_ui->latitudeLineEdit->setText(latitude);

    autoApply();
}


void DateTimeWidget::on_longitudeDoubleSpinBox_valueChanged(double d)
{
    d = _clamp(-180.0, +180.0, d);

    const int h = _abs(d);
    const int m = _abs(_frac(d) * 60.0);
    const int s = _abs(_frac(d * 60.0) * 60.0);

    QString longitude = QString("%1°%2'%3'' %4")
        .arg(h, 3, 10, QChar('0'))
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0'))
        .arg(d < 0 ? "W" : "E");

    if(!m_presetChanged)
    {
        m_ui->presetComboBox->blockSignals(true);
        m_ui->presetComboBox->setCurrentIndex(0);
        m_ui->presetComboBox->blockSignals(false);
    }

    m_ui->longitudeLineEdit->setText(longitude);

    autoApply();
}


void DateTimeWidget::on_latitudeLineEdit_editingFinished()
{
    m_ui->latitudeDoubleSpinBox->setValue(latitudeFromText(m_ui->latitudeLineEdit->text()));
}


void DateTimeWidget::on_longitudeLineEdit_editingFinished()
{
    m_ui->longitudeDoubleSpinBox->setValue(longitudeFromText(m_ui->longitudeLineEdit->text()));
}


void DateTimeWidget::on_altitudeDoubleSpinBox_valueChanged(double d)
{
    autoApply();
}


const double DateTimeWidget::latitudeFromText(const QString &text)
{
    QRegExp re("\\d+");
    int i = 0;
    
    i = re.indexIn(text, i);
    int h = text.mid(i, re.matchedLength()).toInt();

    i = re.indexIn(text, i + re.matchedLength());
    int m = text.mid(i, re.matchedLength()).toInt();

    i = re.indexIn(text, i + re.matchedLength());
    int s = text.mid(i, re.matchedLength()).toInt();

    if(text.right(1).toUpper() == "S")
        h *= -1;

    return (h < 0 ? -1 : 1) * _decimal(_abs(h), m, s);
}

const double DateTimeWidget::longitudeFromText(const QString &text)
{
    QRegExp re("\\d+");
    int i = 0;
    
    i = re.indexIn(text, i);
    int h = text.mid(i, re.matchedLength()).toInt();

    i = re.indexIn(text, i + re.matchedLength());
    int m = text.mid(i, re.matchedLength()).toInt();

    i = re.indexIn(text, i + re.matchedLength());
    int s = text.mid(i, re.matchedLength()).toInt();

    if(text.right(1).toUpper() == "W")
        h *= -1;

    return (h < 0 ? -1 : 1) * _decimal(_abs(h), m, s);
}


void DateTimeWidget::on_presetComboBox_currentIndexChanged(int index)
{
    QStringList temp = m_ui->presetComboBox->itemData(index).toString().split(":");

    m_presetChanged = true;

    m_ui->latitudeLineEdit->setText(temp[0]);
    on_latitudeLineEdit_editingFinished();

    m_ui->longitudeLineEdit->setText(temp[1]);
    on_longitudeLineEdit_editingFinished();

    if(index != 0)
        m_ui->utcOffsetDoubleSpinBox->setValue(temp[2].toFloat());

    m_presetChanged = false;

    autoApply();
}


void DateTimeWidget::setScene(AbstractHimmelScene *scene)
{
    m_scene = scene;

    const bool hasLocationSupport(scene ? scene->hasLocationSupport() : false);

    m_ui->latitudeDoubleSpinBox->setEnabled(hasLocationSupport);
    m_ui->latitudeLabel->setEnabled(hasLocationSupport);
    m_ui->latitudeLineEdit->setEnabled(hasLocationSupport);

    m_ui->longitudeDoubleSpinBox->setEnabled(hasLocationSupport);
    m_ui->longitudeLabel->setEnabled(hasLocationSupport);
    m_ui->longitudeLineEdit->setEnabled(hasLocationSupport);

    m_ui->altitudeDoubleSpinBox->setEnabled(hasLocationSupport);
    m_ui->altitudeLabel->setEnabled(hasLocationSupport);

    m_ui->presetComboBox->setEnabled(hasLocationSupport);
    m_ui->presetLabel->setEnabled(hasLocationSupport);
}