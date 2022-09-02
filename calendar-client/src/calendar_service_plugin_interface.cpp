#include "calendar_service_plugin_interface.h"

#include <QMap>
#include <QVariant>

class CSPI_Storage
{
public:
	inline static CSPI_Storage *instance() {
		static CSPI_Storage *inst = new CSPI_Storage;
		return inst;
	}

	QMap<QString, QVariant> commonData;
};

QVariant CalendarServicePluginInterface::sharedData(QString s)
{
	return CSPI_Storage::instance()->commonData[s];
}

void CalendarServicePluginInterface::setSharedData(QString k, QVariant v)
{
	CSPI_Storage::instance()->commonData[k] = v;
}

