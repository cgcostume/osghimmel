
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

    const QDateTime dt(QDateTime::fromTime_t(m_timef.gett()));
    m_ui->dateTimeEdit->setDateTime(dt);

    me_timeout();

    connect(m_timer, SIGNAL(timeout()), this, SLOT(me_timeout()));


    // fill presets
    const QStringList presets = QStringList()
        << ";00°00'N:000°00'E"
        << "Aberdeen, Scotland;57°09'N:002°09'W"
        << "Adelaide, Australia;34°55'S:138°36'E"
        << "Algiers, Algeria;36°50'N:003°00'E"
        << "Amsterdam, Netherlands;52°22'N:004°53'E"
        << "Ankara, Turkey;39°55'N:032°55'E"
        << "Asunción, Paraguay;25°15'S:057°40'W"
        << "Athens, Greece;37°58'N:023°43'E"
        << "Auckland, New Zealand;36°52'S:174°45'E"
        << "Bangkok, Thailand;13°45'N:100°30'E"
        << "Barcelona, Spain;41°23'N:002°09'E"
        << "Beijing, China;39°55'N:116°25'E"
        << "Belém, Brazil;01°28'S:048°29'W"
        << "Belfast, Northern Ireland;54°37'N:005°56'W"
        << "Belgrade, Serbia;44°52'N:020°32'E"
        << "Berlin, Germany;52°30'N:013°25'E"
        << "Birmingham, England;52°25'N:001°55'W"
        << "Bogotá, Colombia;04°32'N:074°15'W"
        << "Bombay, India;19°00'N:072°48'E"
        << "Bordeaux, France;44°50'N:000°31'W"
        << "Bremen, Germany;53°05'N:008°49'E"
        << "Brisbane, Australia;27°29'S:153°08'E"
        << "Bristol, England;51°28'N:002°35'W"
        << "Brussels, Belgium;50°52'N:004°22'E"
        << "Bucharest, Romania;44°25'N:026°07'E"
        << "Budapest, Hungary;47°30'N:019°05'E"
        << "Buenos Aires, Argentina;34°35'S:058°22'W"
        << "Cairo, Egypt;30°02'N:031°21'E"
        << "Calcutta, India;22°34'N:088°24'E"
        << "Canton, China;23°07'N:113°15'E"
        << "Cape Town, South Africa;33°55'S:018°22'E"
        << "Caracas, Venezuela;10°28'N:067°02'W"
        << "Cayenne, French Guiana;04°49'N:052°18'W"
        << "Chihuahua, Mexico;28°37'N:106°05'W"
        << "Chongqing, China;29°46'N:106°34'E"
        << "Copenhagen, Denmark;55°40'N:012°34'E"
        << "Córdoba, Argentina;31°28'S:064°10'W"
        << "Dakar, Senegal;14°40'N:017°28'W"
        << "Darwin, Australia;12°28'S:130°51'E"
        << "Djibouti, Djibouti;11°30'N:043°03'E"
        << "Dublin, Ireland;53°20'N:006°15'W"
        << "Durban, South Africa;29°53'S:030°53'E"
        << "Edinburgh, Scotland;55°55'N:003°10'W"
        << "Frankfurt, Germany;50°07'N:008°41'E"
        << "Georgetown, Guyana;06°45'N:058°15'W"
        << "Glasgow, Scotland;55°50'N:004°15'W"
        << "Guatemala City, Guatemala;14°37'N:090°31'W"
        << "Guayaquil, Ecuador;02°10'S:079°56'W"
        << "Hamburg, Germany;53°33'N:010°02'E"
        << "Hammerfest, Norway;70°38'N:023°38'E"
        << "Havana, Cuba;23°08'N:082°23'W"
        << "Helsinki, Finland;60°10'N:025°00'E"
        << "Hobart, Tasmania;42°52'S:147°19'E"
        << "Hong Kong, China;22°20'N:114°11'E"
        << "Iquique, Chile;20°10'S:070°07'W"
        << "Irkutsk, Russia;52°30'N:104°20'E"
        << "Jakarta, Indonesia;06°16'S:106°48'E"
        << "Johannesburg, South Africa;26°12'S:028°04'E"
        << "Kingston, Jamaica;17°59'N:076°49'W"
        << "Kinshasa, Congo;04°18'S:015°17'E"
        << "Kuala Lumpur, Malaysia;03°08'N:101°42'E"
        << "La Paz, Bolivia;16°27'S:068°22'W"
        << "Leeds, England;53°45'N:001°30'W"
        << "Lima, Peru;12°00'S:077°02'W"
        << "Lisbon, Portugal;38°44'N:009°09'W"
        << "Liverpool, England;53°25'N:003°00'W"
        << "London, England;51°32'N:000°05'W"
        << "Lyons, France;45°45'N:004°50'E"
        << "Madrid, Spain;40°26'N:003°42'W"
        << "Manchester, England;53°30'N:002°15'W"
        << "Manila, Philippines;14°35'N:120°57'E"
        << "Marseilles, France;43°20'N:005°20'E"
        << "Mazatlán, Mexico;23°12'N:106°25'W"
        << "Mecca, Saudi Arabia;21°29'N:039°45'E"
        << "Melbourne, Australia;37°47'S:144°58'E"
        << "Mexico City, Mexico;19°26'N:099°07'W"
        << "Milan, Italy;45°27'N:009°10'E"
        << "Montevideo, Uruguay;34°53'S:056°10'W"
        << "Moscow, Russia;55°45'N:037°36'E"
        << "Munich, Germany;48°08'N:011°35'E"
        << "Nagasaki, Japan;32°48'N:129°57'E"
        << "Nagoya, Japan;35°07'N:136°56'E"
        << "Nairobi, Kenya;01°25'S:036°55'E"
        << "Nanjing (Nanking), China;32°03'N:118°53'E"
        << "Naples, Italy;40°50'N:014°15'E"
        << "New Delhi, India;28°35'N:077°12'E"
        << "Newcastle-on-Tyne, England;54°58'N:001°37'W"
        << "Odessa, Ukraine;46°27'N:030°48'E"
        << "Osaka, Japan;34°32'N:135°30'E"
        << "Oslo, Norway;59°57'N:010°42'E"
        << "Panama City, Panama;08°58'N:079°32'W"
        << "Paramaribo, Suriname;05°45'N:055°15'W"
        << "Paris, France;48°48'N:002°20'E"
        << "Perth, Australia;31°57'S:115°52'E"
        << "Plymouth, England;50°25'N:004°05'W"
        << "Port Moresby, Papua New Guinea;09°25'S:147°08'E"
        << "Prague, Czech Republic;50°05'N:014°26'E"
        << "Rangoon, Myanmar;16°50'N:096°00'E"
        << "Reykjavík, Iceland;64°04'N:021°58'W"
        << "Rio de Janeiro, Brazil;22°57'S:043°12'W"
        << "Rome, Italy;41°54'N:012°27'E"
        << "Salvador, Brazil;12°56'S:038°27'W"
        << "Santiago, Chile;33°28'S:070°45'W"
        << "St. Petersburg, Russia;59°56'N:030°18'E"
        << "São Paulo, Brazil;23°31'S:046°31'W"
        << "Shanghai, China;31°10'N:121°28'E"
        << "Singapore, Singapore;01°14'N:103°55'E"
        << "Sofia, Bulgaria;42°40'N:023°20'E"
        << "Stockholm, Sweden;59°17'N:018°03'E"
        << "Sydney, Australia;34°00'S:151°00'E"
        << "Tananarive, Madagascar;18°50'S:047°33'E"
        << "Teheran, Iran;35°45'N:051°45'E"
        << "Tokyo, Japan;35°40'N:139°45'E"
        << "Tripoli, Libya;32°57'N:013°12'E"
        << "Venice, Italy;45°26'N:012°20'E"
        << "Veracruz, Mexico;19°10'N:096°10'W"
        << "Vienna, Austria;48°14'N:016°20'E"
        << "Vladivostok, Russia;43°10'N:132°00'E"
        << "Warsaw, Poland;52°14'N:021°00'E"
        << "Wellington, New Zealand;41°17'S:174°47'E"
        << "Zürich, Switzerland;47°21'N:008°31'E";

    foreach(const QString &preset, presets)
    {
        QStringList temp = preset.split(";");
        m_ui->presetComboBox->addItem(temp[0], temp[1]);
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


void DateTimeWidget::on_dateTimeEdit_dateTimeChanged(const QDateTime &datetime)
{
    if(m_ui->autoApplyPushButton->isChecked())
        on_applyPushButton_clicked();
}


void DateTimeWidget::on_applyPushButton_clicked(bool)
{
    const time_t t(m_ui->dateTimeEdit->dateTime().toTime_t());

    stop();
    m_timef.sett(t, true);

    if(m_scene && m_scene->hasLocationSupport())
    {
        m_scene->setLatitude(m_ui->latitudeDoubleSpinBox->value());
        m_scene->setLongitude(m_ui->longitudeDoubleSpinBox->value());
    }

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

    m_timef.setf(getTimeSliderF(), true);
}


const float DateTimeWidget::getTimeSliderF() const
{
    return static_cast<float>(m_ui->timeSlider->value() - m_ui->timeSlider->minimum()) 
        / (m_ui->timeSlider->maximum() - m_ui->timeSlider->minimum());
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
}


void DateTimeWidget::on_latitudeLineEdit_editingFinished()
{
    m_ui->latitudeDoubleSpinBox->setValue(latitudeFromText(m_ui->latitudeLineEdit->text()));
}


void DateTimeWidget::on_longitudeLineEdit_editingFinished()
{

    m_ui->longitudeDoubleSpinBox->setValue(longitudeFromText(m_ui->longitudeLineEdit->text()));
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

    m_presetChanged = false;
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

    m_ui->presetComboBox->setEnabled(hasLocationSupport);
    m_ui->presetLabel->setEnabled(hasLocationSupport);
}