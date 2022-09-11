#include "bindings.h"

#include "calendar_service_plugin_interface.h"
#include "python_plugin.h"

#include <QFile>
#include <QDir>

#include <iostream>


void assignArgument(CPyObject *obj, std::string s, CPyObject val, int i)
{
	obj->setAttribute(s, val);
}

std::string concatenate(std::string begin, int val)
{
	return begin.append("_").append(std::to_string(val));
}

bool lookup(CPyObject obj, std::string argument)
{
	if (obj.is()) {
		CPYO__(pDict, obj.attribute("__dict__"));
		if (pDict.is()) {
			PyObject *pMKey = nullptr, *pMValue = nullptr;
			for (Py_ssize_t i = 0; PyDict_Next(pDict, &i, &pMKey, &pMValue);) {
				const char *_key = PyUnicode_AsUTF8(pMKey);
				if (lookup(obj.attribute(_key), argument)) {
					return true;
				}
			}
		}
	}
	return false;
}

void printAttributes(CPyObject obj)
{
	if (obj.is()) {
		CPYO__(pDict, obj.attribute("__dict__"));
		if (pDict.is()) {
			PyObject *pMKey = nullptr, *pMValue = nullptr;
			for (Py_ssize_t i = 0; PyDict_Next(pDict, &i, &pMKey, &pMValue);) {
				const char *_key = PyUnicode_AsUTF8(pMKey);
				std::cout << " ---> " << _key << std::endl;
			}
		}
	}
}

CPyObject::CPyObject(PyObject *_p, std::string add, bool nope)
	: additionals(add), p(_p), nothing(!nope)
{
	LOG_FN
#ifdef DEBUG_PY_CALLS
	if (!nothing) {
		if (p) {
			std::cout << "Created object from PyObject pointer: " << _p << ": " << additionals << std::endl;
		} else {
			std::cout << "Created object from NULL pointer: " << additionals << std::endl;
		}
	}
#endif
}

CPyObject::CPyObject(const CPyObject &other, std::string add)
	: additionals(add)
{
	LOG_FN
	p = other.p;
	if (p) {
		Py_INCREF(p);
	}
}

CPyObject::CPyObject(const CPyObject &other)
	: additionals(other.additionals)
{
	LOG_FN
	p = other.p;
	if (p) {
		Py_INCREF(p);
	}
}

CPyObject::CPyObject(bool b, std::string add)
	: additionals(add)
{
	LOG_FN
	Py_INCREF((b) ? Py_True : Py_False);
	p = (b) ? Py_True : Py_False;
}

CPyObject::CPyObject(long l, std::string add) : CPyObject(PyLong_FromLong(l), add, nothing)
{
	LOG_FN
}

CPyObject::CPyObject(long long l, std::string add) : CPyObject(PyLong_FromLongLong(l), add, nothing)
{
	LOG_FN
}

CPyObject::CPyObject(unsigned long l, std::string add) : CPyObject(PyLong_FromUnsignedLong(l), add, nothing)
{
	LOG_FN
}

CPyObject::CPyObject(unsigned long long l, std::string add) : CPyObject(PyLong_FromUnsignedLongLong(l), add, nothing)
{
	LOG_FN
}

CPyObject::CPyObject(double d, std::string add) : CPyObject(PyLong_FromDouble(d), add, nothing)
{
	LOG_FN
}

CPyObject::CPyObject(int i, std::string add) : CPyObject(static_cast<long>(i), add)
{
	LOG_FN
}

CPyObject::CPyObject(unsigned int i, std::string add) : CPyObject(static_cast<unsigned long>(i), add)
{
	LOG_FN
}

CPyObject::CPyObject(std::string s, std::string add) : CPyObject(PyUnicode_FromString(s.c_str()), add, nothing)
{
	LOG_FN
}

bool CPyObject::hasAttribute(std::string attrib) const
{
	if (p) {
		return (PyObject_HasAttrString(p, attrib.c_str()) == 1);
	}
	return false;
}

CPyObject::~CPyObject()
{
	LOG_FN
	Release();
}

PyObject *CPyObject::getObject()
{
	//Do we add a ref here?
	LOG_FN
	return p;
}

PyObject *CPyObject::setObject(PyObject *_p)
{
	LOG_FN
	if (p && p != _p) {
		Py_DECREF(p); //Really needed?
	}
	return (p = _p);
}

PyObject *CPyObject::AddRef()
{
	LOG_FN
	if (p) {
		Py_INCREF(p);
	}
	return p;
}

void CPyObject::Release()
{
	LOG_FN
	if (p) {
		Py_DECREF(p);
	}
	p = nullptr;
}


PyObject *CPyObject::operator ->()
{
	LOG_FN
	return p;
}

bool CPyObject::is() const
{
	return (p != nullptr);
}

CPyObject::operator PyObject *()
{
	LOG_FN
	//[CHECK] Do we really have to increment it like... All the time?
	if (p) {
		Py_INCREF(p);
	}
	return p;
}

PyObject *CPyObject::operator =(PyObject *other)
{
	LOG_FN
	if (p && p != other) {
		Py_DECREF(p); //Is it really needed?
	}
	p = other;
	return p;
}

void CPyObject::operator =(const CPyObject &other)
{
	p = other.p;
	AddRef();
}

void CPyObject::setAttribute(std::string d, CPyObject v)
{
	LOG_FN
	if (p) {
		if (hasAttribute(d)) {
			PyObject_SetAttrString(p, d.c_str(), v);
		}
#ifdef DEBUG_PY_CALLS
		 else {
		 	std::cout << "Undefined attribute " << d << std::endl;
		}
#endif
	}
#ifdef DEBUG_PY_CALLS
	 else {
		std::cout << "Trying to set attribute of non-existant object." << std::endl;
	}
#endif
}

CPyObject CPyObject::attribute(std::string d) const
{
	LOG_FN
	if (p) {
		if (PyObject_HasAttrString(p, d.c_str())) {
			return CPYO_(PyObject_GetAttrString(p, d.c_str()));
		}
#ifdef DEBUG_PY_CALLS
		  else {
			std::cout << "Failed to get attribute " << d << std::endl;
		}
#endif
	}
#ifdef DEBUG_PY_CALLS
	  else {
		std::cout << "Trying to get attribute of non-existant object." << std::endl;
	}
#endif
	return CPYO();
}

bool CPyObject::hasMember(std::string d) const
{
	return hasAttribute(d);
}

CPyObject CPyObject::member(std::string d) const
{
	return attribute(d);
}

CPyObject::operator bool() const
{
	LOG_FN
	if (p) {
		return PyObject_IsTrue(p);
	}
	return false;
}

long CPyObject::toLong() const
{
	LOG_FN
	if (p && PyLong_Check(p)) {
		return PyLong_AsLong(p);
	}
	return 0;
}

CPyObject::operator long long() const
{
	LOG_FN
	if (p && PyLong_Check(p)) {
		return PyLong_AsLongLong(p);
	}
	return 0;
}

CPyObject::operator unsigned long() const
{
	LOG_FN
	if (p && PyLong_Check(p)) {
		return PyLong_AsUnsignedLong(p);
	}
	return 0;
}

CPyObject::operator unsigned long long() const
{
	LOG_FN
	if (p && PyLong_Check(p)) {
		return PyLong_AsUnsignedLongLong(p);
	}
	return 0;
}

CPyObject::operator double() const
{
	LOG_FN
	if (p && PyLong_Check(p)) {
		return PyLong_AsDouble(p);
	}
	return 0;
}

CPyObject::operator int() const
{
	LOG_FN
	if (p && PyLong_Check(p)) {
		return static_cast<int>(PyLong_AsLong(p));
	}
	return 0;
}

CPyObject::operator unsigned int() const
{
	LOG_FN
	if (p && PyLong_Check(p)) {
		return static_cast<unsigned int>(PyLong_AsUnsignedLong(p));
	}
	return 0;
}

CPyObject::operator std::string() const
{
	LOG_FN
	if (p && PyUnicode_Check(p)) {
		return std::string(PyUnicode_AsUTF8(p));
	}
	return "";
}

PythonAPI::PythonAPI()
{
	LOG_FN

	//Load the script from our QRC file.
	QFile f("/usr/lib/dde-calendar/python-plugin/main.py");
	f.open(QIODevice::ReadOnly);

	if (!StorageSystem::sharedData("python_is_loaded").toBool()) {
		StorageSystem::setSharedData("python_is_loaded", true);
		Py_Initialize();
	}
	StorageSystem::setSharedData("python_running_extensions", StorageSystem::sharedData("python_running_extensions").toInt() +1);

	CPYO__(pGlobal, PyDict_New());

	//Generate the module.
	CPYO__(pNewMod, PyImport_AddModule("__main__"));
	PyModule_AddStringConstant(pNewMod, "__file__", "");

	CPYO__(pLocal, PyModule_GetDict(pNewMod));

	//Execute the script.
	CPYO__(pValue, PyRun_String(QString(f.readAll()).toStdString().c_str(), Py_file_input, pGlobal, pLocal));
	
	if (pValue.is()) {
		//Check that we have a class CalendarAPI
		if (PyObject_HasAttrString(pNewMod, "CalendarInterface") == 1) {
			//Create the instance of the class.
			CPYO__(capi, pNewMod.attribute("CalendarInterface"));
			if (capi.is() && PyCallable_Check(capi) == 1) {
				PyObject *_obj = PyObject_CallObject(capi, nullptr);
				obj = CPYO_(_obj);
			}
			if (!obj.is()) {
				std::cout << "Failed to instanciate CalendarInterface." << std::endl;
				PyErr_Print();
			} else {
				QString abs = "/home/" + qgetenv("USER") + "/.config/dde-calendar";
				QDir _dir(abs);
				_dir.filePath(abs);
				if (!_dir.exists()) {
					_dir.mkdir(abs);
				}
				obj.setAttribute("configdir", abs.toStdString());

				//Load the internal data.
				load();
			}
		} else {
			PyErr_Print();
			std::cout << "Unable to locate CalendarInterface, available vars are:" << std::endl;
			PyObject *const pDict = PyModule_GetDict(pNewMod);
			PyObject *pMKey = nullptr, *pMValue = nullptr;
			for (Py_ssize_t i = 0; PyDict_Next(pDict, &i, &pMKey, &pMValue);) {
				const char *_key = PyUnicode_AsUTF8(pMKey);
				std::cout << _key << std::endl;
			}
		}
	} else {
		std::cout << "A Python error occured:" << std::endl;
		PyErr_Print();
	}
}

PythonAPI::~PythonAPI()
{
	LOG_FN
	if ((StorageSystem::sharedData("python_running_extensions").toInt() -1) == 0 && StorageSystem::sharedData("python_is_loaded").toBool()) {
		StorageSystem::setSharedData("python_is_loaded", false);
		Py_Finalize();
	}
	StorageSystem::setSharedData("python_running_extensions", StorageSystem::sharedData("python_running_extensions").toInt() -1);
}

PythonAPI::operator CPyObject()
{
	return obj;
}

CalendarEvent::CalendarEvent()
{}

CalendarEvent::CalendarEvent(CPyObject o) : obj(o)
{}


