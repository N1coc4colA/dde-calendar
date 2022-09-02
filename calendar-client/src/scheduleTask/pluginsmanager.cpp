#include "pluginsmanager.h"

#include "plugin_checking.h"

#include <QLibrary>
#include <QDir>

//[ADDED]
#include <iostream>

typedef CalendarServicePluginInterface * (*instFunction)(QObject *);

PluginsManager::PluginsManager(QObject *p) : QObject(p)
{
	QDir dir("/home/nicolas/plugined/dde-calendar-5.9.1/build/calendar-client/plugins/js/plugin/");
    //QDir dir("/home/nicolas/plugined/dde-calendar-5.9.1/build/calendar-client/plugins/google/"); //"/usr/lib/dde-calendar");
    QStringList filter = {"*.so"};
    dir.setNameFilters(filter);
    QStringList fileNames = dir.entryList(QDir::Files);

    for (const QString &fileName : fileNames) {
        QString absPath = dir.absoluteFilePath(fileName);
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

PluginsManager::~PluginsManager()
{
	qDeleteAll(pluginsList);
	for (auto path : pluginsPaths.values()) {
		QLibrary lib(path);
		lib.unload();
	}
}

void PluginsManager::disablePlugin(QString name)
{
	if (mappedPlugins.contains(name)) {
		pluginsList.removeOne(mappedPlugins[name]);
		delete mappedPlugins[name];
		mappedPlugins.remove(name);
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
	for (auto plug : pluginsList) {
		plug->getJobs(map);
	}
}

void PluginsManager::getJob(qint64 jid, ScheduleDataInfo &out)
{
}


