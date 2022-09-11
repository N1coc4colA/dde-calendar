#ifndef CALENDAR_SERVICE_PLUGIN_INTERFACE
#define CALENDAR_SERVICE_PLUGIN_INTERFACE


//[ADDED]

#include "plugin_checking.h"
#include "src/scheduledatainfo.h"

#include <QObject>

class StorageSystem
{
public:
	static QVariant sharedData(QString key);
	static void setSharedData(QString key, QVariant val);
};

class CalendarServicePluginInterface : public QObject
{
public:
	inline explicit CalendarServicePluginInterface(QObject *p = nullptr) : QObject(p) {}
	//virtual ~CalendarServicePluginInterface() = default;

    virtual void connectUser() = 0;
    
    //The first is called, after the second one is called.
    //This enables the use of asynchronous APIS, and wait for a result.
    //So we can start all retrieving tasks, and then get the result.
    virtual void startGettingJobs(QDate beg, QDate end) = 0;
    virtual void getJobs(QMap<QDate, QVector<ScheduleDataInfo> > &info) = 0;

    virtual QString serviceName() const = 0;
    virtual QString serviceVersion() const = 0;
    virtual QString serviceSettings() const = 0;

Q_SIGNALS:
	void eventAdded();
	void eventRemoved();
};

#define DECLARE_CSPI_INTERFACE(Class) \
		extern "C" { \
			CalendarServicePluginInterface *createInstance(QObject *p) \
			{ return new Class (p); } \
		}

#define CalendarServicePluginInterface_iid "com.deepin.Calendar.ServiceInterface/1.0"

Q_DECLARE_INTERFACE(CalendarServicePluginInterface, CalendarServicePluginInterface_iid)

#endif
