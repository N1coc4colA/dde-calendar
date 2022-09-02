#ifndef SAMPLE_H
#define SAMPLE_H

#include <calendar_service_plugin_interface.h>

#include <QObject>

class CalendarSample : public CalendarServicePluginInterface
{
	Q_OBJECT

public:
	explicit CalendarSample(QObject *ptr = nullptr);
	~CalendarSample();

	void connectUser() override;

	void startGettingJobs(QDate beg, QDate end) override;
	void getJobs(QMap<QDate, QVector<ScheduleDataInfo> > &info) override;
	
	
    QString serviceName() const override;
    QString serviceVersion() const override;
    QString serviceSettings() const override;
};

#endif
