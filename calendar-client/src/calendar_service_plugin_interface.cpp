#include "calendar_service_plugin_interface.h"

#include <QMap>
#include <QVariant>

#include <iostream>

#pragma message("Has been processed ------------------------------------------------------------------------------------")

class CSPI_Storage
{
public:
	inline static CSPI_Storage *instance() {
		static CSPI_Storage *inst = new CSPI_Storage;
		return inst;
	}

	QMap<QString, QVariant> commonData;
};

QVariant StorageSystem::sharedData(QString s)
{
	std::cout << "Accessing common value: " << s.toStdString() << std::endl;
	return CSPI_Storage::instance()->commonData[s];
}

void StorageSystem::setSharedData(QString k, QVariant v)
{
	std::cout << "Setting common value: " << k.toStdString() << std::endl;
	CSPI_Storage::instance()->commonData[k] = v;
}

