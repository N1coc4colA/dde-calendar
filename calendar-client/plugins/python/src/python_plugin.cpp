#include "python_plugin.h"

#include <iostream>


//Function exportations required to ensure that we run the same library as the target app.
DECLARE_AS_PLUGIN

//Export functions used to access our interface from the target app.
DECLARE_CSPI_INTERFACE(PythonCalendar)


PythonCalendar::PythonCalendar(QObject *ptr)
	: CalendarServicePluginInterface(ptr)
{
}

PythonCalendar::~PythonCalendar()
{
}

void PythonCalendar::connectUser()
{
	_api.requestConnection();
}

void PythonCalendar::startGettingJobs(QDate beg, QDate end)
{
	_api.requestJobs(beg.toString().toStdString(), end.toString().toStdString());
}

void PythonCalendar::getJobs(QMap<QDate, QVector<ScheduleDataInfo> > &info)
{
	_api.getJobsResult();
	CalendarEvent ev = _api.getNextJob();
	while (!ev.ending()) {
		ScheduleDataInfo inf = ScheduleDataInfo::JsonStrToSchedule(QString::fromStdString(ev.jsonData()));
		QDate date = QDate::fromString(QString::fromStdString(ev.date()));
		if (!date.isNull() && date.isValid()) {
			info[date] << inf;
		}
		ev = _api.getNextJob();
	}
}

QString PythonCalendar::serviceName() const
{
	return QString::fromStdString(_api.pluginName()) + "(Python)";
}

QString PythonCalendar::serviceVersion() const
{
	return QString::fromStdString(_api.pluginVersion());
}

QString PythonCalendar::serviceSettings() const
{
	return QString::fromStdString(_api.jsonSettings());
}
