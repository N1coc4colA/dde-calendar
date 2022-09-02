#include "plugin_checking.h"

//[ADDED]

#include <iostream>
#include <map>
#include <string>


static std::map<std::string, std::vector<std::string>> preloaded_libraries;

typedef const char *(*_Qt_build_version)();
typedef int (*_Dtk_build_version)();
typedef std::vector<const char *> (*_Plugin_preload_libs)();

template<typename T>
bool contains(std::vector<T> arr, T k)
{
     return (std::find(arr.begin(), arr.end(), k) != arr.end());
}

bool isLibraryCompatible(QLibrary &lib, bool enable_preload)
{
    if (_Qt_build_version qtFunc = (_Qt_build_version)lib.resolve("_Qt_build_version")) {
     	if (_Dtk_build_version dtkFunc = (_Dtk_build_version)lib.resolve("_Dtk_build_version")) {
     		if (QString(QT_VERSION_STR) == qtFunc()) {
     			if (DTK_VERSION == dtkFunc()) {
     				if (enable_preload) {
     					if (_Plugin_preload_libs pplFunc = (_Plugin_preload_libs)lib.resolve("_Plugin_preload_libs")) {
     						std::vector<const char *> vect = pplFunc();

     						QList<QString> libsList;
     						for (auto val : vect) {
     							libsList << val;
     						}

     						//Then unload the library, load the requirements, then load again the library.
     						lib.unload();

     						for (auto val : libsList) {
     							std::cout << "Preloading the library: " << val.toStdString() << std::endl;
     							QLibrary cLib(val);
     							cLib.load();
     						}
     						lib.load();
     					} else {
     						return true;
     					}
     				} else {
     					return true;
     				}
     			} else {
    				std::cout << "Dtk versions are not the same." << std::endl;
     			}
     		} else {
    			std::cout << "Qt versions are not the same." << std::endl;
    		}
       	} else {
    		std::cout << "Failed to resolve symbol _Dtk_build_version; checking impossible." << std::endl;
       	}
    } else {
    	std::cout << "Failed to resolve symbol _Qt_build_version; checking impossible." << std::endl;
    }
    return false;
}

