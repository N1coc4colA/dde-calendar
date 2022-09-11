#ifndef BINDINGS_H
#define BINDINGS_H

#include <python3.10/Python.h>
#include <string>


//class PyObject;

/* Helper functions to */


#ifdef GCPDEBUG

#include <iostream>
#define LOG_FN std::cout << __func__ << ": " << __LINE__ << std::endl;
#define DEBUG_PY_CALLS
#define DEBUG_PY_SYMBOLS

#else
#define LOG_FN

#endif


#ifndef DEBUG_PY_SYMBOLS
#define CLOG_FN(Name)

#else
#include <iostream>
#define CLOG_FN(Name) std::cout << " >>> Calling Python function: " << #Name << std::endl;

#endif

#define PUT_MSG /*_Pragma("message (\"Python function found\")")*/

#define STRINGIFY(x) STRINGIFY_IMPL(x)
#define STRINGIFY_IMPL(x) #x


#define CPYO__(name, data) CPyObject name(data, std::string(__FILE__ ": " STRINGIFY(__LINE__)))
#define CPYO_(data) CPyObject(data, std::string(__FILE__ ": " STRINGIFY(__LINE__)))
#define CPYO() CPyObject(nullptr, std::string(__FILE__ ": " STRINGIFY(__LINE__)))


#ifndef DEBUG_PY_CALLS

#define PY_ATTRIB(Type, Name, Default) \
	inline Type Name() const \
	{ \
		PUT_MSG\
		if (obj.hasAttribute(#Name)) { \
			return CPYO_(obj.attribute(#Name)); \
		} \
		return Default; \
	};

#define PY_STR_ATTRIB(Name) PY_ATTRIB(std::string, Name, "")
#define PY_STR_ATTRIB1(Name, Default) PY_ATTRIB(std::string, Name, Default)

#define PY_RET_CALL(Name) \
	return PyCall(obj, obj.attribute(#Name), std::string(#Name))

#define PY_CALL(Name) \
	PyCall(obj, obj.attribute(#Name), std::string(#Name))

#define PY_FUNC1(Name) \
	inline void Name () const \
	{ \
		PUT_MSG\
		CLOG_FN(Name); \
		if (obj.is() && obj.hasMember(#Name) && PyCallable_Check(obj.member(#Name))) { \
			PY_CALL(Name); \
		} \
	};

#define PY_FUNC(Type, Name, Default) \
	inline Type Name () const \
	{ \
		PUT_MSG\
		CLOG_FN(Name); \
		if (obj.is() && obj.hasMember(#Name) && PyCallable_Check(obj.member(#Name))) { \
			return (Type)(PY_CALL(Name)); \
		} \
		return Default; \
	};

#define PY_STR_FUNC(Name, Default) PY_FUNC(std::string, Name, Default)

#define PY_OBJ_FUNC(Type, Name, Default) \
	inline Type Name() const \
	{ \
		PUT_MSG\
		CLOG_FN(Name); \
		if (obj.is() && obj.hasMember(#Name) && PyCallable_Check(obj.member(#Name))) { \
			return Type(PY_CALL(Name)); \
		} \
		return Default; \
	};

#define PY_SPEC_FUNC1(Name) \
	inline void Name(std::string a, std::string b) const \
	{ \
		PUT_MSG\
		CLOG_FN(Name); \
		if (obj.is() && obj.hasMember(#Name) && PyCallable_Check(obj.member(#Name))) { \
			PyCall(obj, obj.attribute(#Name), std::string(#Name), a, b); \
		} \
	};

#define PY_SPEC_FUNC2(Name) \
	inline void Name(bool b) const \
	{ \
		PUT_MSG\
		CLOG_FN(Name); \
		if (obj.is() && obj.hasMember(#Name) && PyCallable_Check(obj.member(#Name))) { \
			PyCall(obj, obj.member(#Name), std::string(#Name), b); \
		} \
	};

#define PY_SPEC_FUNC_UNDEF(Name) \
	inline void Name(CPyObject param) const \
	{ \
		PUT_MSG\
		CLOG_FN(Name); \
		if (obj.is() && obj.hasMember(#Name) && PyCallable_Check(obj.member(#Name)) == 1) { \
			PyCall(obj, obj.member(#Name), std::string(#Name), param); \
		} \
	};

#else

#pragma message("Python binding data printing enabled.")

#include <iostream>

#define PRINT_MSG(Name) std::cout << " >>> Failed, object is not valid: " << obj.getObject() << ".\n     Pointer from: " << #Name << " : " << obj.additionals << std::endl;
#define PRINT_MSG_ATTRIB(Name) std::cout << " >>> Failed, no such attribute, or is not a function: " #Name ".\n     Pointer from: " << #Name << " : " << obj.additionals << "\nAvailable members:" << std::endl; printAttributes(obj);

#define PY_ATTRIB(Type, Name, Default) \
	inline Type Name() const { \
		if (obj.hasAttribute(#Name)) { \
			return CPYO_(obj.attribute(#Name)); \
		} else { \
			PRINT_MSG_ATTRIB(Name); \
		} \
		return Default; \
	}

#define PY_STR_ATTRIB(Name) PY_ATTRIB(std::string, Name, "")
#define PY_STR_ATTRIB1(Name, Default) PY_ATTRIB(std::string, Name, Default)

#define PY_RET_CALL(Name) \
	return PyCall(obj, obj.attribute(#Name), std::string(#Name))

#define PY_CALL(Name) \
	PyCall(obj, obj.attribute(#Name), std::string(#Name))

#define PY_FUNC1(Name) \
	inline void Name () const \
	{ \
		PUT_MSG \
		CLOG_FN(Name); \
		if (obj.is()) { \
			if (obj.hasMember(#Name) && PyCallable_Check(obj.member(#Name)) == 1) { \
				PY_CALL(Name); \
				return; \
			} else { \
				PRINT_MSG_ATTRIB(Name) \
				return; \
			} \
		} \
		PRINT_MSG(Name) \
	}

#define PY_FUNC(Type, Name, Default) \
	inline Type Name () const \
	{ \
		PUT_MSG \
		CLOG_FN(Name); \
		if (obj.is()) { \
			if (obj.hasMember(#Name) && PyCallable_Check(obj.member(#Name)) == 1) { \
				return (Type)(PY_CALL(Name)); \
			} else { \
				PRINT_MSG_ATTRIB(Name) \
				return Default; \
			} \
		} \
		PRINT_MSG(Name) \
		return Default; \
	}

#define PY_STR_FUNC(Name, Default) PY_FUNC(std::string, Name, Default)

#define PY_OBJ_FUNC(Type, Name, Default) \
	inline Type Name() const \
	{ \
		PUT_MSG \
		CLOG_FN(Name); \
		if (obj.is()) { \
			if (obj.hasMember(#Name) && PyCallable_Check(obj.member(#Name)) == 1) { \
				return Type(PY_CALL(Name)); \
			} else { \
				PRINT_MSG_ATTRIB(Name) \
				return Default; \
			} \
		} \
		PRINT_MSG(Name) \
		return Default; \
	}

#define PY_SPEC_FUNC1(Name) \
	inline void Name(std::string a, std::string b) const \
	{ \
		PUT_MSG \
		CLOG_FN(Name); \
		if (obj.is()) { \
			if (obj.hasMember(#Name) && PyCallable_Check(obj.member(#Name)) == 1) { \
				PyCall(obj, obj.member(#Name), std::string(#Name), a, b); \
				return; \
			} else { \
				PRINT_MSG_ATTRIB(Name) \
				return; \
			} \
		} \
		PRINT_MSG(Name) \
	}

#define PY_SPEC_FUNC2(Name) \
	inline void Name(bool b) const \
	{ \
		PUT_MSG \
		CLOG_FN(Name); \
		if (obj.is()) { \
			if (obj.hasMember(#Name) && PyCallable_Check(obj.member(#Name)) == 1) { \
				PyCall(obj, obj.member(#Name), std::string(#Name), b); \
				return; \
			} else { \
				PRINT_MSG_ATTRIB(Name) \
				return; \
			} \
		} \
		PRINT_MSG(Name) \
	}

#define PY_SPEC_FUNC_UNDEF(Name) \
	inline void Name(CPyObject param) const \
	{ \
		PUT_MSG \
		CLOG_FN(Name); \
		if (obj.is()) { \
			if (obj.hasMember(#Name) && PyCallable_Check(obj.member(#Name)) == 1) { \
				PyCall(obj.member(#Name), std::string(#Name), param); \
				return; \
			} else { \
				PRINT_MSG_ATTRIB(Name) \
				return; \
			} \
		} \
		PRINT_MSG(Name) \
	}

#endif


class CPyObject
{
public:
	explicit CPyObject(PyObject *_p = nullptr, std::string additional = "", bool nothing = true);
	CPyObject(const CPyObject &other, std::string additional);
	CPyObject(const CPyObject &other);
	~CPyObject();

	//Facilities for basic types.
	CPyObject(bool, std::string additional = "");
	CPyObject(long, std::string additional = "");
	CPyObject(long long, std::string additional = "");
	CPyObject(unsigned long, std::string additional = "");
	CPyObject(unsigned long long, std::string additional = "");
	CPyObject(double, std::string additional = "");
	CPyObject(int, std::string additional = "");
	CPyObject(unsigned int, std::string additional = "");
	CPyObject(std::string, std::string additional = "");

	PyObject *getObject();
	PyObject *setObject(PyObject *_p);

	PyObject *AddRef();
	void Release();
	PyObject *operator ->();
	bool is() const;

	operator PyObject *();
	PyObject *operator =(PyObject *other);
	void operator =(const CPyObject &other);

	void setAttribute(std::string d, CPyObject val);
	CPyObject attribute(std::string d) const;
	bool hasAttribute(std::string attrib) const;
	CPyObject member(std::string d) const;
	bool hasMember(std::string member) const;

	long toLong() const;
	operator bool() const;
	operator long long() const;
	operator unsigned long() const;
	operator unsigned long long() const;
	operator double() const;
	operator int() const;
	operator unsigned int() const;
	operator std::string() const;

	std::string additionals;

private:
	PyObject *p = nullptr;
	bool nothing = false;
};


void printAttributes(CPyObject obj);
std::string concatenate(std::string begin, int val);
void assignArgument(CPyObject *obj, std::string, CPyObject val, int i);
bool lookup(CPyObject object, std::string name);


class Bundler
{
public:
	int i = 0;
	CPyObject *target;
	std::string fname;

	template<typename A>
	void pack(A arg) {
		assignArgument(target, concatenate(fname, i), arg, i);
		i++;
	}
};


template<typename ... Args>
inline CPyObject PyCall(CPyObject obj, CPyObject func, std::string fname = "", Args ... args)
{
	LOG_FN

	//Dispatch the values.
	int argcount = sizeof ...(args);
	CPYO__(tuple, PyTuple_New(0));
	if (argcount > 0) {
		Bundler bundle;
		bundle.target = &obj;
		bundle.fname = fname;
		(bundle.pack(args), ...);
	}
	return CPYO_(PyObject_CallObject(func, tuple));
}


class CalendarEvent
{
public:
	explicit CalendarEvent();
	CalendarEvent(CPyObject);

	PY_STR_ATTRIB(date);
	PY_STR_ATTRIB(jsonData);
	PY_ATTRIB(bool, ending, true);

private:
	CPyObject obj;
};


class PythonAPI
{
public:
	explicit PythonAPI();
	~PythonAPI();

	PY_FUNC1(requestConnection);
	PY_SPEC_FUNC1(requestJobs);
	PY_FUNC1(getJobsResult);
	PY_OBJ_FUNC(CalendarEvent, getNextJob, CalendarEvent());
	PY_FUNC1(load);

	PY_STR_ATTRIB1(pluginName, "Undefined");
	PY_STR_ATTRIB1(pluginVersion, "Undefined");
	PY_STR_FUNC(jsonSettings, "{}");
	PY_SPEC_FUNC_UNDEF(settingChanged);

	operator CPyObject();

public:
	CPyObject obj = CPYO();
};


#endif

