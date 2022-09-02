#include "js_plugin.h"

#include <iostream>


//Function exportations required to ensure that we run the same library as the target app.
DECLARE_AS_PLUGIN

//Export functions used to access our interface from the target app.
DECLARE_CSPI_INTERFACE(JsCalendar)


JsCalendar::JsCalendar(QObject *ptr)
	: CalendarServicePluginInterface(ptr)
{
}

JsCalendar::~JsCalendar()
{
}

void JsCalendar::connectUser()
{
	_api.requestConnection();
}

void JsCalendar::startGettingJobs(QDate beg, QDate end)
{
	_api.requestJobs(beg.toString().toStdString(), end.toString().toStdString());
}

void JsCalendar::getJobs(QMap<QDate, QVector<ScheduleDataInfo> > &info)
{
	_api.getJobsResult();
	/*CalendarEvent ev = _api.getNextJob();
	while (!ev.ending()) {
		ScheduleDataInfo inf = ScheduleDataInfo::JsonStrToSchedule(QString::fromStdString(ev.jsonData()));
		QDate date = QDate::fromString(QString::fromStdString(ev.date()));
		if (!date.isNull() && date.isValid()) {
			info[date] << inf;
		}
		ev = _api.getNextJob();
	}*/
}

QString JsCalendar::serviceName() const
{
	return QString::fromStdString(_api.pluginName()) + "(Js)";
}

QString JsCalendar::serviceVersion() const
{
	return QString::fromStdString(_api.pluginVersion());
}

QString JsCalendar::serviceSettings() const
{
	return QString::fromStdString(_api.jsonSettings());
}
