#include "plugin.h"

#include "bindings/bindings.h"

#include <QTimeZone>

#include <iostream>

//Function exportations required to ensure that we run the same library as the target app.
DECLARE_AS_PLUGIN

//Python libs has to be loaded before the plugin, don't ask me why.
//PLUGIN_PRELOAD_LIBS("/usr/lib/x86_64-linux-gnu/libpython3.9.so")//, "/usr/lib/python3.9/lib-dynload/_ssl.cpython-39-x86_64-linux-gnu.so")

//Export functions used to access our interface from the target app.
DECLARE_CSPI_INTERFACE(GoogleCalendar)

//RFC3339 specifies as is: "yyyy-MM-dd hh:mm:ss:msms((+ | -)hh:mm | Z)", "yyyy-MM-ddThh:mm:msms((+ | -)hh:mm | Z)", where T is a separator (or ' '), Z replaces +00:00 (UTC).

QDateTime fromRFC3339(QString val)
{
	QString other;
	QStringList beg = val.split("T");
	if (beg.count() == 1) {
		beg = val.split(" ");
	}
	if (beg.count() == 1) {
		//Error
		return QDateTime();
	}
	int tz;
	if (beg[1].endsWith("Z")) {
		//Means UTC = 0
		tz = 0;
	} else {
		QStringList utc = beg[1].split("+");
		if (utc.count() == 1) {
			utc = beg[1].split("-");
		}
		tz = utc[0].toInt() *3600 + utc[1].toInt() * 60;
		other = utc[0];
	}
	
	//Now that we have the UTC, transform the zoned-date-time to a local date-time.
	QStringList date_str = beg[0].split("-");
	QDate _date(date_str[0].toInt(), date_str[1].toInt(), date_str[2].toInt());

	QStringList time_str = other.split(":");
	QTime _time(time_str[0].toInt(), time_str[1].toInt(), time_str[2].toInt(), time_str[3].toInt());
	
	return QDateTime(_date, _time, Qt::TimeSpec::OffsetFromUTC, tz);
}

QString toRFC3339(QDate &date, bool begin = true)
{
	QString out = date.toString("yyyy-MM-dd");
	out += "T";

	//Then add time.
	out += (begin ? "23:59.00"
			   : "00:00.00"
		   );

	//Add the time zone. Get the local UTC.
	int tz = QTimeZone::systemTimeZone().offsetFromUtc(QDateTime::currentDateTime()) / 3600;
	if (tz > 0) {
		out += "+";
	}
	out += QString::number(tz);

	return out;
}

GoogleCalendar::GoogleCalendar(QObject *ptr)
	: CalendarServicePluginInterface(ptr)
{
}

GoogleCalendar::~GoogleCalendar()
{
}

void GoogleCalendar::connectUser()
{
	//We connect nothing.
	std::cout << "Connecting user." << std::endl;
	_api.requestConnection();
}

void GoogleCalendar::startGettingJobs(QDate beg, QDate end)
{
	//Load some data from a server.
	std::cout << "Starting to get the jobs." << std::endl;
	_api.requestJobs(toRFC3339(beg).toStdString(), toRFC3339(end).toStdString());
}

void GoogleCalendar::getJobs(QMap<QDate, QVector<ScheduleDataInfo> > &info)
{
	//Add your things to the QMap's content (info)
	std::cout << "Giving retrieved jobs result." << std::endl;
	_api.getJobsResult();
	GCEvent current = _api.getNextJob();
	while (!current.ending()) {
		ScheduleDataInfo data;
		data.setTitleName(QString::fromStdString(current.title()));
		data.setAllDay(current.isAllDay());
		//data.setRecurID(); [TODO] Construct it in a way that the local backend will not interfere with it.
		data.setType(0); //[TODO] Lookup local job types to match this one, so we can assign a value.
		data.setDescription(QString::fromStdString(current.description()));
		data.setIsLunar(current.isLunar());
		
		//Create the beginning to ending data.
		RepetitionRule repete;
		repete.setEndDate(fromRFC3339(QString::fromStdString(current.end())));
		//[TODO] Support all RRuleID and RRuleEndType
		repete.setRuleId(RepetitionRule::RRuleID::RRule_NONE);
		repete.setRuleType(RepetitionRule::RRuleEndType::RRuleType_NEVER);
		repete.setEndCount(0);

		data.setRepetitionRule(repete);

		QDateTime _dt = fromRFC3339(QString::fromStdString(current.start()));
		info[_dt.date()] << data;

		GCEvent current = _api.getNextJob();
	}
}

QString GoogleCalendar::serviceName() const
{
	return "Google Calendar (testing)";
}

QString GoogleCalendar::serviceVersion() const
{
	return "v0.1";
}

QString GoogleCalendar::serviceSettings() const
{
	return "{}";
}

