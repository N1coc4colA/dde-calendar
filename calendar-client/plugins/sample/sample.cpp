#include "sample.h"

#include <iostream>


//Export functions used to access our interface from the target app.
DECLARE_CSPI_INTERFACE(CalendarSample)

//Function exportations required to ensure that we run the same library as the target app.
DECLARE_AS_PLUGIN

CalendarSample::CalendarSample(QObject *ptr)
	: CalendarServicePluginInterface(ptr)
{
	std::cout << "Contructed CalendarSample interface." << std::endl;
}

CalendarSample::~CalendarSample()
{
	std::cout << "Destroying a CalendarSample interface." << std::endl;
}

void CalendarSample::connectUser()
{
	//We connect nothing.
	std::cout << "Connecting user." << std::endl;
}

void CalendarSample::startGettingJobs(QDate beg, QDate end)
{
	//Load some data from a server.
	std::cout << "Starting to get the jobs." << std::endl;
	notice = ((beg < QDate::currentDate()) && (QDate::currentDate() < end));
}

void CalendarSample::getJobs(QMap<QDate, QVector<ScheduleDataInfo> > &info)
{
	//Add your things to the QMap's content (info)
	std::cout << "Giving retrieved jobs result." << std::endl;

	if (notice) {
		notice = false;
		QDate target = QDate::currentDate();
		QString current = target.toString("yyyy-MM-dd");

		ScheduleDataInfo _sch = ScheduleDataInfo::JsonStrToSchedule("{\"AllDay\":false,\"Description;\":\"\",\"End\":\"" + current + "T11:15:00+01:00\",\"ID\":-1,\"Ignore\":[],\"IsLunar\":false,\"RRule\":\"\",\"RecurID\":0,\"Remind\":\"\",\"Start\":\"" + current + "T10:15:00+01:00\",\"Title\":\"This is today! :)\",\"Type\":1}");
		info[target] << _sch;
	}
}

QString CalendarSample::serviceName() const
{
	std::cout << "Getting service name." << std::endl;
	return "Calendar Sample";
}

QString CalendarSample::serviceVersion() const
{
	std::cout << "Getting service version." << std::endl;
	return "v0.1";
}

QString CalendarSample::serviceSettings() const
{
	std::cout << "Getting service settings." << std::endl;
	return "";
}

