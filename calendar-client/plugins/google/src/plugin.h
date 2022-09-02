#ifndef GPLUGIN_H
#define GPLUGIN_H

#include <calendar_service_plugin_interface.h>
#include <QObject>

#include "bindings/bindings.h"

class GoogleCalendar : public CalendarServicePluginInterface
{
	Q_OBJECT

public:
	explicit GoogleCalendar(QObject *ptr = nullptr);
	~GoogleCalendar();

	void connectUser() override;

	void startGettingJobs(QDate beg, QDate end) override;
	void getJobs(QMap<QDate, QVector<ScheduleDataInfo> > &info) override;

    QString serviceName() const override;
    QString serviceVersion() const override;
    QString serviceSettings() const override;

private:
	CalendarAPI _api;
};

#endif
