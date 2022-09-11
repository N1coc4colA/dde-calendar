#include "pluginsmanager.h"

#include "plugin_checking.h"
#include "calendar_service_plugin_interface.h"

#include <QTextStream>
#include <QFile>
#include <QLibrary>
#include <QDir>
#include <QStandardPaths>

//[ADDED]
#include <iostream>

typedef CalendarServicePluginInterface * (*instFunction)(QObject *);

PluginsManager::PluginsManager(QObject *p) : QObject(p)
{
	readDisabledPlugins();

	StorageSystem::setSharedData("user", qgetenv("USERNAME"));

	QDir dir("/usr/lib/dde-calendar");
	QStringList filter = {"*.so"};
	dir.setNameFilters(filter);
	QStringList fileNames = dir.entryList(QDir::Files);

	for (const QString &fileName : fileNames) {
		QString absPath = dir.absoluteFilePath(fileName);
		if (!disabledPlugins.contains(absPath.split("/").last())) {
			QLibrary lib(absPath);
			if (isLibraryCompatible(lib)) {
				std::cout << "> Loading plugin: " << absPath.toStdString() << std::endl;
				if (instFunction func = (instFunction)lib.resolve("createInstance")) {
					if (CalendarServicePluginInterface *plugin = func(this)) {
		   				QString name = plugin->serviceName();
	 					std::cout << "*** Loaded Calendar Service Plugin: " << name.toStdString() << std::endl;
						pluginsList << plugin;
						mappedPlugins[name] = plugin;
						pluginsPaths[name] = absPath;
						connect(plugin, SIGNAL(eventAdded()), this, SLOT(requiresUpdate()));
						connect(plugin, SIGNAL(eventRemoved()), this, SLOT(requiresUpdate()));
					}
				} else {
					std::cout << "> Plugin does not provide interface exportation." << std::endl;
				}
		 	} else {
				std::cout << "> Error while loading: " << lib.errorString().toStdString() << std::endl;
			}
		}
	}
}

PluginsManager::~PluginsManager()
{
	qDeleteAll(pluginsList);
	for (auto path : pluginsPaths.values()) {
		QLibrary lib(path);
		lib.unload();
	}

	//Remove all duplicates and write the data.
	QString fp = QStandardPaths::writableLocation(QStandardPaths::HomeLocation).append("/.config/dde-calendar/disabled_plugins.list");
	QFile f(fp);
	if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&f);
		out << disabledPlugins.join("\n");
		f.close();
	}
}

void PluginsManager::readDisabledPlugins()
{
	QString fp = QStandardPaths::writableLocation(QStandardPaths::HomeLocation).append("/.config/dde-calendar/disabled_plugins.list");
	QFile f(fp);

	if (!f.exists() && f.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&f);
		out << "\n";
		f.close();
	}

	if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QString data(f.readAll());
		std::cout << "Disabled plugins are:\n" << data.toStdString() << "\n" << std::endl;
		disabledPlugins = data.split("\n");
		disabledPlugins.removeAll("");
		f.close();
	}
}

void PluginsManager::disablePlugin(QString name)
{
	if (mappedPlugins.contains(name)) {
		pluginsList.removeOne(mappedPlugins[name]);
		delete mappedPlugins[name];
		mappedPlugins.remove(name);
		disabledPlugins << pluginsPaths[name];
	}
}

void PluginsManager::enablePlugin(QString name)
{
	if (pluginsPaths.contains(name)) {
		QLibrary lib(pluginsPaths[name]);
		if (isLibraryCompatible(lib)) {
			if (instFunction func = (instFunction)lib.resolve("getInstance")) {
				if (CalendarServicePluginInterface *plugin = func(this)) {
					pluginsList << plugin;
					mappedPlugins[plugin->serviceName()] = plugin;
					connect(plugin, SIGNAL(eventAdded()), this, SLOT(requiresUpdate()));
	   				connect(plugin, SIGNAL(eventRemoved()), this, SLOT(requiresUpdate()));
				}
			} else {
				std::cout << "> Plugin does not provide interface exportation." << std::endl;
			}
		} else {
			std::cout << "> Error while loading: " << lib.errorString().toStdString() << std::endl;
		}
	}
}

QMap<QString, bool> PluginsManager::getPlugins()
{
	QMap<QString, bool> result;
	
	for (auto name : pluginsPaths.keys()) {
		result[name] = mappedPlugins.contains(name);
	}
	return result;
}

void PluginsManager::getJobs(QDate start, QDate end, QMap<QDate, QVector<ScheduleDataInfo> > &map)
{
	for (auto plug : pluginsList) {
		plug->startGettingJobs(start, end);
	}

	/*
		We have to generate a separate map the merge both. Otherwise, we can have some information becoming available from a plugin to another.
		Example: You have your Google Calendar account plugin that loads its data into the map.
		Then another plugin, which shouldn't have been loaded is, in fact, loaded. Then we pass the map to it so we load the data.
		However, under the hood, the plugin interacts with a webserver, it sends back all the data already in the map: it is a spyware.
		To avoid such thing, we pass an empty map, and we merge it, into 2 separated steps, not directly.
	*/


	for (auto plug : pluginsList) {
		QMap<QDate, QVector<ScheduleDataInfo>> tmp;
		plug->getJobs(tmp);

		for(auto e : tmp.keys()) {
			for (auto s : tmp[e]) {
				s.isFromPlugin = true;
				s.setID(-1);
				s.calendarServiceName = mappedPlugins.key(plug);
				map[e] << s;
			}
		}
	}
}

void PluginsManager::getJob(qint64 jid, ScheduleDataInfo &out)
{
	//[CHECK] Do we have to handle this here this way too?
}


