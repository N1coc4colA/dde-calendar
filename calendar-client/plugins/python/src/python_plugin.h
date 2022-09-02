#ifndef PYTHON_PLUGIN
#define PYTHON_PLUGIN

#include <calendar_service_plugin_interface.h>
#include <QObject>

#include "bindings.h"

class PythonCalendar : public CalendarServicePluginInterface
{
	Q_OBJECT

public:
	explicit PythonCalendar(QObject *ptr = nullptr);
	~PythonCalendar();

	void connectUser() override;

	void startGettingJobs(QDate beg, QDate end) override;
	void getJobs(QMap<QDate, QVector<ScheduleDataInfo> > &info) override;

    QString serviceName() const override;
    QString serviceVersion() const override;
    QString serviceSettings() const override;

private:
	PythonAPI _api;
};

#endif

