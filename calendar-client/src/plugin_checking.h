#ifndef PLUGIN_CHECKING_H
#define PLUGIN_CHECKING_H


//[ADDED]

#include <QLibrary>
#include <dtkcore_global.h>
#include <vector>

bool isLibraryCompatible(QLibrary &lib, bool enable_preload = true);

//We must force these to be visible, in some cases they can don't be shown without any real reasons.

#define DECLARE_AS_PLUGIN \
	extern "C" { \
		__attribute__((visibility("default"))) const char *_Qt_build_version() \
		{ return QT_VERSION_STR; } \
		__attribute__((visibility("default"))) int _Dtk_build_version() \
		{ return DTK_VERSION; } \
	}

#define PLUGIN_PRELOAD_LIBS(...) \
	extern "C" { \
		__attribute__((visibility("default"))) std::vector<const char *> _Plugin_preload_libs() \
		{ return std::vector<const char *>{__VA_ARGS__}; } \
	}

#endif

