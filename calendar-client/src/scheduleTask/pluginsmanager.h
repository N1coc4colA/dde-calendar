#ifndef PLUGINSMANAGER_H
#define PLUGINMANAGER_H

//[ADDED]

#include "../calendar_service_plugin_interface.h"

#include <QObject>
#include <QMap>

class PluginsManager : public QObject
{
	Q_OBJECT
public:
	PluginsManager(QObject *p = nullptr);
	~PluginsManager();

	inline static PluginsManager *instance() {
		static PluginsManager *inst = new PluginsManager;
		return inst;
	}

	void readDisabledPlugins();
	void disablePlugin(QString name);
	void enablePlugin(QString name);
	QMap<QString, bool> getPlugins();

	void getJobs(QDate start, QDate end, QMap<QDate, QVector<ScheduleDataInfo> > &map);
	void getJob(qint64 jid, ScheduleDataInfo &out);
	
Q_SIGNALS:
	void requiresUpdate();

private:
	QStringList disabledPlugins;
	QList<CalendarServicePluginInterface *> pluginsList;
	QMap<QString, QString> pluginsPaths;
	QMap<QString, CalendarServicePluginInterface *> mappedPlugins;
};

#endif

