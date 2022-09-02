#ifndef JS_PLUGIN
#define JS_PLUGIN

#include <calendar_service_plugin_interface.h>
#include <QObject>

#include "bindings/bindings.h"

class JsCalendar : public CalendarServicePluginInterface
{
	Q_OBJECT

public:
	explicit JsCalendar(QObject *ptr = nullptr);
	~JsCalendar();

	void connectUser() override;

	void startGettingJobs(QDate beg, QDate end) override;
	void getJobs(QMap<QDate, QVector<ScheduleDataInfo> > &info) override;

    QString serviceName() const override;
    QString serviceVersion() const override;
    QString serviceSettings() const override;

private:
	JsAPI _api;
};

#endif

