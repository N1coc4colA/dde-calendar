#ifndef JS_BINDINGS_H
#define JS_BINDINGS_H


	/*
		We use V8 Ptr compress.
	*/
	#ifndef V8_COMPRESS_POINTERS
	#define 	V8_COMPRESS_POINTERS
	#endif
	#ifndef V8_31BIT_SMIS_ON_64BIT_ARCH
	#define 	V8_31BIT_SMIS_ON_64BIT_ARCH
	#endif


#include "v8pp/module.hpp"
#include "v8pp/class.hpp"

#include <QString>
#include <QList>

#include <tuple>
#include <utility>
#include <type_traits>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <map>

#include <node.h>
#include <uv.h>


	/*
		[SECTION] Template helpers.
	*/


template<typename ... Args>
class template_size
{
public:
	inline explicit template_size(Args ...) {}

	inline operator int()
	{
		return size;
	}

	static const int size = sizeof...(Args);
};

	/*
		[SECTION] Js values converions.
	*/


enum JsVType {
	Int,
	Double,
	Float,
	Number,
	String,
	DString,
	Bool,
	External,
	Undef
};


class JsValue
{
public:
	inline JsValue(std::string i, v8::Isolate *isolate) : _type(JsVType::String), _string(i), iso(isolate) {}
	inline JsValue(int i, v8::Isolate *isolate) : _type(JsVType::Int), _double(i), iso(isolate) {}
	inline JsValue(double i, v8::Isolate *isolate) : _type(JsVType::Double), _double(i), iso(isolate) {}
	inline JsValue(float i, v8::Isolate *isolate) : _type(JsVType::Float), _double(i), iso(isolate) {}
	inline JsValue(bool i, v8::Isolate *isolate) : _type(JsVType::Bool), _bool(i), iso(isolate) {}

	inline JsValue(v8::Local<v8::Value> val, v8::Isolate *isolate) : iso(isolate)
	{
		if (val->IsString()) {
			_type = JsVType::String;
			_string = v8pp::from_v8<std::string>(iso, val);
		} else if (val->IsBoolean()) {
			_type = JsVType::Bool;
			_bool = v8pp::from_v8<bool>(iso, val);
		} else if (val->IsNumber()) {
			_type = JsVType::Double;
			_double = v8pp::from_v8<double>(iso, val);
		} else if (!val.IsEmpty()) {
			_type = JsVType::External;
			_external = val;
		} else {
			_type = JsVType::Undef;
		}
	}

	inline bool compatible(JsValue other) const
	{
		return ((other._type == _type)
				|| ((other._type == JsVType::Number)
					&& ((_type == JsVType::Int)
						|| (_type == JsVType::Double)
						|| (_type == JsVType::Float)
						)
					)
				|| ((_type == JsVType::Number)
					&& ((other._type == JsVType::Int)
						|| (other._type == JsVType::Double)
						|| (other._type == JsVType::Float)
						)
					)
				);
	}

	template<typename T>
	inline T to_any(T defaultValue) const
	{
		if (_type == JsVType::External) {
			return v8pp::from_v8<T>(iso, _external);
		} else {
			return defaultValue;
		}
	}

	inline operator v8::Local<v8::Value>() const
	{
		switch (_type) {
			case JsVType::Int: {
				return v8pp::to_v8(iso, _double);
			}
			case JsVType::Double: {
				return v8pp::to_v8(iso, _double);
			}
			case JsVType::Float: {
				return v8pp::to_v8(iso, _double);
			}
			case JsVType::Number: {
				return v8pp::to_v8(iso, _double);
			}
			case JsVType::String: {
				return v8pp::to_v8(iso, _string);
			}
			case JsVType::DString: {
				return v8pp::to_v8(iso, _string);
			}
			case JsVType::Bool: {
				return v8pp::to_v8(iso, _bool);
			}
			case JsVType::External: {
				return _external;
			}
			case JsVType::Undef: {
				break;
			}
		}
		return v8::Undefined(iso);
	}

	inline operator std::string() const
	{
		switch (_type) {
			case JsVType::Int: {
				return QString::number(static_cast<int>(_double)).toStdString();
			}
			case JsVType::Double: {
				return QString::number(_double).toStdString();
			}
			case JsVType::Float: {
				return QString::number(static_cast<float>(_double)).toStdString();
			}
			case JsVType::Number: {
				return QString::number(_double).toStdString();
			}
			case JsVType::String: {
				return _string;
			}
			case JsVType::DString: {
				return _string;
			}
			case JsVType::Bool: {
				return (_bool ? "true" : "false");
			}
			case JsVType::External: {
				return v8pp::from_v8<std::string>(iso, _external);
			}
			case JsVType::Undef: {
				break;
			}
		}
		return "";
	}

	inline operator double() const
	{
		switch (_type) {
			case JsVType::Int: {
				return _double;
			}
			case JsVType::Double: {
				return _double;
			}
			case JsVType::Float: {
				return _double;
			}
			case JsVType::Number: {
				return _double;
			}
			case JsVType::String: {
				return QString::fromStdString(_string).toDouble();
			}
			case JsVType::DString: {
				return QString::fromStdString(_string).toDouble();
			}
			case JsVType::Bool: {
				return (_bool ? 1 : 0);
			}
			case JsVType::External: {
				return v8pp::from_v8<double>(iso, _external);
			}
			case JsVType::Undef: {
				break;
			}
		}
		return 0;
	}

	inline operator float() const
	{
		switch (_type) {
			case JsVType::Int: {
				return static_cast<float>(_double);
			}
			case JsVType::Double: {
				return static_cast<float>(_double);
			}
			case JsVType::Float: {
				return static_cast<float>(_double);
			}
			case JsVType::Number: {
				return static_cast<float>(_double);
			}
			case JsVType::String: {
				return QString::fromStdString(_string).toFloat();
			}
			case JsVType::DString: {
				return QString::fromStdString(_string).toFloat();
			}
			case JsVType::Bool: {
				return (_bool ? 1 : 0);
			}
			case JsVType::External: {
				return v8pp::from_v8<float>(iso, _external);
			}
			case JsVType::Undef: {
				break;
			}
		}
		return 0;
	}

	inline operator bool()
	{
		switch (_type) {
			case JsVType::Int: {
				return (_double > 0);
			}
			case JsVType::Double: {
				return (_double > 0);
			}
			case JsVType::Float: {
				return (_double > 0);
			}
			case JsVType::Number: {
				return (_double > 0);
			}
			case JsVType::String: {
				return (_string == "true");
			}
			case JsVType::DString: {
				return (_string == "true");
			}
			case JsVType::Bool: {
				return _bool;
			}
			case JsVType::External: {
				return v8pp::from_v8<bool>(iso, _external);
			}
			case JsVType::Undef: {
				break;
			}
		}
		return false;
	}

private:
	JsVType _type;
	std::string _string;
	double _double;
	bool _bool;
	v8::Local<v8::Value> _external;
	v8::Isolate *iso;
};


	/*
		[SECTION] Js function call helpers.
	*/


class JsPackChecker
{
public:
	template<typename T>
	inline void check(v8::Local<v8::Value> val, v8::Isolate *iso)
	{
		state = state && JsValue(val, iso).compatible(JsValue(T()));
	}

	bool state = true;
};


template<typename ... Args>
class JsArgsCheck
{
public:
	template<typename ... Types>
	inline bool compare(v8::FunctionCallbackInfo<v8::Value> const &args)
	{
		static const int len = sizeof...(Types);
		if (len != size) {
			return false;
		} else if (len == 0) {
			return true;
		}

		JsPackChecker checkup;
		int i = 0;
		(checkup.check<Types>(args[i++]), ...);

		return checkup.state;
	}

	static const int size = sizeof ... (Args);
};


template<typename Obj, typename Function, typename Tuple, size_t ... I>
inline auto call(Obj obj, Function f, Tuple t, std::index_sequence<I ...>)
{
	 return (obj->*f)(std::get<I>(t) ...);
}


template<typename Obj, typename Function, typename Tuple>
inline auto call(Obj o, Function f, Tuple t)
{
	static constexpr auto size = std::tuple_size<Tuple>::value;
	return call(o, f, t, std::make_index_sequence<size>{});
}


template <class Func, class Tuple, size_t N = 0>
inline void runtime_exec(Func func, Tuple& tup, size_t idx)
{
	if (N == idx) {
		std::invoke(func, std::get<N>(tup));
		return;
	}

	if constexpr (N + 1 < std::tuple_size_v<Tuple>) {
		return runtime_get<Func, Tuple, N + 1>(func, tup, idx);
	}
}


template <class Func, class Tuple, size_t N = 0>
inline auto runtime_get(Tuple& tup, size_t idx)
{
	if (N == idx) {
		return std::get<N>(tup);
	}

	if constexpr (N + 1 < std::tuple_size_v<Tuple>) {
		return runtime_get<Tuple, N + 1>(tup, idx);
	}
}


template<typename Type, typename Method, typename Ret, typename ... Args>
class JsObjectFuncCall
{
public:
	inline explicit JsObjectFuncCall(Type *_ptr, Method meth)
	{
		func = meth;
		ptr = _ptr;
	}

	inline Ret run(v8::FunctionCallbackInfo<v8::Value> const &args, v8::Isolate *iso)
	{
		std::tuple<Args ...> tuple;
		//Build the pack.
		int i = 0;
		while (i < template_size<Args ...>()) {
			JsValue val(args[i], iso);
			runtime_get(tuple, i) = val;
			++i;
		}

		return call(ptr, func, tuple);
	}

	Type *ptr;
	Method func;
};


	/*
		[SECTION] Js function callers.
	*/


template<typename Type, typename Method, typename Ret>
class JsObjectFuncCall<Type, Method, Ret>
{
public:
	inline explicit JsObjectFuncCall(Type *_ptr, Method meth)
	{
		func = meth;
		ptr = _ptr;
	}

	inline virtual Ret run(v8::FunctionCallbackInfo<v8::Value> const &, v8::Isolate *)
	{
		std::tuple<> tuple;
		return call(ptr, func, tuple);
	}

	Type *ptr;
	Method func;
};


template<typename Type, typename Method, typename ...Args>
class JsVoidObjectFuncCall
{
public:
	inline explicit JsVoidObjectFuncCall(Type *_ptr, Method meth)
	{
		func = meth;
		ptr = _ptr;
	}

	inline void run(v8::FunctionCallbackInfo<v8::Value> const &args, v8::Isolate *iso)
	{
		std::tuple<Args ...> tuple;
		//Build the pack.
		int i = 0;
		while (i < template_size<Args ...>()) {
			JsValue val(args[i], iso);
			runtime_get(tuple, i) = val;
			++i;
		}

		call(ptr, func, tuple);
	}

	Type *ptr;
	Method func;
};


template<typename Type, typename Method>
class JsVoidObjectFuncCall<Type, Method>
{
public:
	inline explicit JsVoidObjectFuncCall(Type *_ptr, Method meth)
	{
		func = meth;
		ptr = _ptr;
	}

	inline void run(v8::FunctionCallbackInfo<v8::Value> const &, v8::Isolate *)
	{
		std::tuple<> tuple;
		call(ptr, func, tuple);
	}

	Type *ptr;
	Method func;
};


	/*
		[SECTION] Js environment registries.
	*/


class JsEnvAccessAbstract;


class JsEnvRegistry
{
public:
	inline static JsEnvRegistry *instance()
	{
		static JsEnvRegistry *inst = new JsEnvRegistry;
		return inst;
	}

	static void exposeEnvironment(v8pp::module *mo, v8::Isolate *iso);

	QList<JsEnvAccessAbstract *> registries;
};


class JsEnvAccessAbstract
{
public:
	inline explicit JsEnvAccessAbstract()
	{
		JsEnvRegistry::instance()->registries << this;
	}

	inline virtual void build(v8pp::module *, v8::Isolate *) {}
};


inline void JsEnvRegistry::exposeEnvironment(v8pp::module *mo, v8::Isolate *iso)
{
	std::cout << "Exposing C++ Js environment to v8." << std::endl;
	for (auto r : JsEnvRegistry::instance()->registries) {
		r->build(mo, iso);
	}
}


class JsFreeFunctionAccessAbstract;


class JsFreeFunctionRegistry : public JsEnvAccessAbstract
{
public:
	inline static JsFreeFunctionRegistry *instance()
	{
		static JsFreeFunctionRegistry *inst = new JsFreeFunctionRegistry;
		return inst;
	}

	void build(v8pp::module *mo, v8::Isolate *iso) override;

	QList<JsFreeFunctionAccessAbstract *> functions;
};


class JsFreeFunctionAccessAbstract
{
public:
	inline explicit JsFreeFunctionAccessAbstract(bool reg = true)
	{
		if (reg) {
			registerFunction();
		}
	}

	void registerFunction()
	{
		JsFreeFunctionRegistry::instance()->functions << this;
	}

	inline virtual void add(v8pp::module *, v8::Isolate *iso) {}
};


inline void JsFreeFunctionRegistry::build(v8pp::module *mo, v8::Isolate *iso)
{
	std::cout << "Exposing 'free' functions" << std::endl;
	for (auto f : functions) {
		f->add(mo, iso);
	}
}


class JsObjectRegistry : public JsEnvAccessAbstract
{
public:
	inline static JsObjectRegistry *instance()
	{
		static JsObjectRegistry *inst = new JsObjectRegistry;
		return inst;
	}

	inline void build(v8pp::module *mo, v8::Isolate *isolate) override
	{
		std::cout << "Exposing object classes" << std::endl;
		for (auto b : acquires) {
			(*b)(mo, isolate);
		}
	}

	QList<void(*)(v8pp::module *, v8::Isolate *)> acquires;
};

template<typename T>
class JsObjectAccessAbstract
{
public:
	inline virtual ~JsObjectAccessAbstract() {}
	inline virtual void add(v8pp::module *, v8pp::class_<T> &) {}
};


	/*
		[SECTION] JsObject related classes.
	*/


//Class exposing the types to Js.
template<typename T>
class JsObjectType
{
public:
	inline explicit JsObjectType()
	{
		JsObjectRegistry::instance()->acquires << &JsObjectType<T>::build;
	}

	inline static JsObjectType *instance()
	{
		static JsObjectType *inst = new JsObjectType;
		return inst;
	}

	inline static void exec(v8pp::class_<T> &c, QList<JsObjectAccessAbstract<T> *> &l)
	{
		for (auto v : l) {
			v->add(nullptr, c);
		}
	}

	inline static void build(v8pp::module *mo, v8::Isolate *iso)
	{
		std::cout << "Enabling class " << JsObjectType<T>::instance()->className << std::endl;
		v8pp::class_<T> ctmp(iso);
		exec(ctmp, instance()->constructors);
		exec(ctmp, instance()->properties);
		exec(ctmp, instance()->functions);
		exec(ctmp, instance()->methods);
		exec(ctmp, instance()->variables);
		if (mo) {
#ifdef V8PP_MAJOR
			mo->class_(JsObjectType::instance()->className, ctmp);
#else
			mo->set(JsObjectType::instance()->className, ctmp);
#endif
		}
		std::cout << "Class enabled." << std::endl;
	}

	std::string className;
	QList<JsObjectAccessAbstract<T> *> methods;
	QList<JsObjectAccessAbstract<T> *> constructors;
	QList<JsObjectAccessAbstract<T> *> variables;
	QList<JsObjectAccessAbstract<T> *> properties;
	QList<JsObjectAccessAbstract<T> *> functions;
	QList<JsObjectAccessAbstract<T> *> inheritances;
};


class JsObjectClass {};


template<typename T>
class JsClassDataSet : public JsObjectClass
{
public:
	inline explicit JsClassDataSet(std::string name) : JsObjectClass()
	{
		JsObjectType<T>::instance()->className = name;
	}
};


template<typename T, typename Variable>
class JsObjectVariable : public JsObjectAccessAbstract<T>
{
public:
	inline explicit JsObjectVariable(const char *vName, Variable _var)
		: JsObjectAccessAbstract<T>::JsObjectAccessAbstract()
		, vn(vName)
		, var(_var)
	{
		JsObjectType<T>::instance()->variables << dynamic_cast<JsObjectAccessAbstract<T> *>(this);
	}

	inline void add(v8pp::class_<T> &tmp) override
	{
		std::cout << "Enabling JS variable " << vn << std::endl;
#ifdef V8PP_MAJOR
		tmp.var(std::string(vn), var);
#else
		tmp.set(std::string(vn), var);
#endif
	}

	const char *vn;
	Variable var;
};


//Property will be used in a future version, currently, only the getter and setter are used.
template<typename T, typename Property, typename Getter, typename Setter>
class JsObjectProperty : public JsObjectAccessAbstract<T>
{
public:
	inline explicit JsObjectProperty(const char *vName, Property _prop, Getter g, Setter s)
		: JsObjectAccessAbstract<T>::JsObjectAccessAbstract()
		, vn(vName)
		, prop(_prop)
		, getter(g)
		, setter(s)
	{
		JsObjectType<T>::instance()->properties << dynamic_cast<JsObjectAccessAbstract<T> *>(this);
	}

	inline void add(v8pp::class_<T> &tmp) override
	{
		std::cout << "Enabling JS property " << vn << std::endl;
#ifdef V8PP_MAJOR
		tmp.property(std::string(vn), prop);
#else
		tmp.set(std::string(vn), property(getter, setter));
#endif
		std::cout << "Enabled." << std::endl;
	}

	const char *vn;
	Property prop;
	Getter getter;
	Setter setter;
};


template<typename T, typename Method>
class JsObjectConstructor : public JsObjectAccessAbstract<T>
{
public:
	inline explicit JsObjectConstructor(Method m)
		: JsObjectAccessAbstract<T>::JsObjectAccessAbstract()
	{
		meth = m;
		JsObjectType<T>::instance()->constructors << dynamic_cast<JsObjectAccessAbstract<T> *>(this);
	}

	inline void add(v8pp::class_<T> &tmp) override
	{
		std::cout << "Enabling JS constructor" << std::endl;
		//tmp.ctor<Args ...>(); Just because it fails.
		(*tmp.meth)();
	}

	Method meth;
};


//	The MEMBER ___js_is_object is always present for our JsObject-based classes.
//	If it is the case but it doesn't implement any JsObject-based parts, no worries,
//	v8pp will not care about it.

template <typename T>
concept JsObjectConcept = 
requires(T t) {
	T::___js_is_object;
}; 


template<typename T, typename P, typename Method>
class JsObjectInheritance : public JsObjectAccessAbstract<T>
{
public:
	inline explicit JsObjectInheritance(Method meth) : JsObjectAccessAbstract<T>::JsObjectAccessAbstract()
	{}
};


template<typename T, JsObjectConcept P, typename Method>
class JsObjectInheritance<T, P, Method>
{
public:
	inline explicit JsObjectInheritance(Method m)
		: JsObjectAccessAbstract<T>::JsObjectAccessAbstract()
	{
		meth = m;
		JsObjectType<T>::instance()->inheritances << dynamic_cast<JsObjectAccessAbstract<T> *>(this);
	}

	inline void add(v8pp::module *, v8pp::class_<T> &tmp) override {
		std::cout << "Enabling JS inheritance" << std::endl;
		(*tmp.meth)();
	}

	Method meth;
};


	/*
		[SECTION] Function exposure.
	*/


template<typename T, typename Func, typename ... Args>
class JsVoidObjectFunction : public JsObjectAccessAbstract<T>
{
public:
#ifdef V8PP_MAJOR
	inline explicit JsVoidObjectFunction(const char *funcName, void (T::*f)(Args ...))
#else
	inline explicit JsVoidObjectFunction(const char *funcName, Func f)
#endif
		: JsObjectAccessAbstract<T>::JsObjectAccessAbstract()
		, fn(funcName)
		, func(f)
	{
		JsObjectType<T>::instance()->functions << dynamic_cast<JsObjectAccessAbstract<T> *>(this);
	}

	inline void add(v8pp::module *, v8pp::class_<T> &tmp) override {
		std::cout << "Enabling JS function " << fn << std::endl;
#ifdef V8PP_MAJOR
		tmp.function(std::string(fn), std::forward<Func>(func));
#else
		tmp.set(std::string(fn), func);
#endif
	}

	const char *fn;
#ifdef V8PP_MAJOR
	void (T::*func)(Args ...);
#else
	const Func func;
#endif
};


template<typename T, typename Func, typename Ret, typename ... Args>
class JsObjectFunction : public JsObjectAccessAbstract<T>
{
public:
#ifdef V8PP_MAJOR
	inline explicit JsObjectFunction(const char *funcName, Ret (T::*f)(Args ...))
#else
	inline explicit JsObjectFunction(const char *funcName, Func &&f)
#endif
		: JsObjectAccessAbstract<T>::JsObjectAccessAbstract()
		, fn(funcName)
		, func(f)
	{
		JsObjectType<T>::instance()->functions << dynamic_cast<JsObjectAccessAbstract<T> *>(this);
	}

	inline void add(v8pp::module *, v8pp::class_<T> &tmp) override
	{
		std::cout << "Enabling JS function " << fn << std::endl;
#ifdef V8PP_MAJOR
		tmp.function(std::string(fn), std::forward<Func>(func));
#else
		tmp.set(std::string(fn), func);
#endif
	}

	const char *fn;
#ifdef V8PP_MAJOR
	Ret (T::*func)(Args ...);
#else
	const Func func;
#endif
};


	/*
		[SECTION] Js function-related classes, for static ones or just free ones.

		For functions, we have to ensure that the name is not registered twice and that the type is not the same.
	*/


template<typename T>
class JsFunctionsMap
{
public:
	inline static JsFunctionsMap<T> *instance() {
		static JsFunctionsMap<T> *inst = new JsFunctionsMap<T>;
		return inst;
	}

	std::map<const char *, bool> names;
};


template<typename Func, typename Ret, typename ...Args>
class JsFunction : public JsFreeFunctionAccessAbstract
{
public:
#ifdef V8PP_MAJOR
	inline explicit JsFunction(const char *n, Ret (*f)(Args ...))
#else
	inline explicit JsFunction(const char *n, Func f)
#endif
		: JsFreeFunctionAccessAbstract(false)
		, fn(n)
		, func(f)
	{
		if (!JsFunctionsMap<Func>::instance()->names[fn]) {
			JsFunctionsMap<Func>::instance()->names[fn] = true;
			registerFunction();
		}
	}

	inline void add(v8pp::module *mo, v8::Isolate *)
	{
		std::cout << "Enabling function " << fn << std::endl;
		if (mo) {
#ifdef V8PP_MAJOR
			mo->function(std::string(fn), std::forward<Func>(func));
#else
			mo->function(std::string(fn), func);
#endif
		}
	}

	const char *fn;
#ifdef V8PP_MAJOR
	Ret (*func)(Args ...);
#else
	Func func;
#endif
};


template<typename Func, typename ...Args>
class JsVoidFunction : public JsFreeFunctionAccessAbstract
{
public:
#ifdef V8PP_MAJOR
	inline explicit JsVoidFunction(const char *n, void (*f)(Args ...))
#else
	inline explicit JsVoidFunction(const char *n, Func f)
#endif
		: JsFreeFunctionAccessAbstract(false)
		, fn(n)
		, func(f)
	{
		if (!JsFunctionsMap<Func>::instance()->names[fn]) {
			JsFunctionsMap<Func>::instance()->names[fn] = true;
			registerFunction();
		}
	}

	inline void add(v8pp::module *mo, v8::Isolate *)
	{
		std::cout << "Enabling function " << fn << std::endl;
		if (mo) {
#ifdef V8PP_MAJOR
			mo->function(std::string(fn), std::forward<Func>(func));
#else
			mo->function(std::string(fn), func);
#endif
		}
	}

	const char *fn;
#ifdef V8PP_MAJOR
	void (*func)(Args ...);
#else
	Func func;
#endif
};


	/*
		[SECTION] Js MACROS to expose classes.
	*/


//Helpers for tokenization of the preprocessor.
#define GLUED(x) x
#define GLUES(tok, v, ...) tok##GLUED(v)##GLUES(__VA_ARGS)
#define GLUE(...) GLUES(_, __VA_ARGS__)

//Macro to set the name for debug options.
#define JS_EXPOSE_CLASS(Class) \
public: \
	inline static const JsClassDataSet<Class> ___js_wrap_##Class { #Class }; /* Sets up additional data at RT. */ \
	inline virtual const JsObjectClass * ___js_is_object() const { & Class :: ___js_wrap_##Class ; }; /* Tells us that effectively, the class is a Js object one as the member is here. */

//Here are macros to define our functions.
#define JS_OBJ_FUNC(Ret, Class, Name, ...) \
	inline v8::Local<v8::Value> ___wrap_##Name (v8::Isolate *isolate, v8::FunctionCallbackInfo<v8::Value> const &args) { \
		/* Before doing anything, check that the arguments are compatible with the target function. */ \
		if (JsArgsCheck< __VA_ARGS__ >().compare(args)) { \
			/* Generate the function call */ \
			return v8pp::to_v8(isolate, JsObjectFuncCall< Class, decltype(& Class :: Name), Ret __VA_OPT__(,) __VA_ARGS__ >(this, & Class :: Name).run(args, isolate)); \
		} \
		/* Return undef in case of issue. */ \
		return v8pp::throw_ex(isolate, "Invalid arguments passed, C++ not called."); \
	} \
	/* Registers the function. */ \
	inline static const JsObjectFunction<Class, decltype(& Class :: ___wrap_##Name), v8::Isolate *, const v8::FunctionCallbackInfo<v8::Value> &> ___static_js_wrap_##Name {#Name, & Class :: ___wrap_##Name}; \
	/* The real function declaration. */ \
	Ret Name( __VA_ARGS__ );

#define JS_VOID_OBJ_FUNC(Class, Name, ...) \
	inline void ___wrap_##Name (v8::Isolate *isolate, v8::FunctionCallbackInfo<v8::Value> const &args) { \
		if (JsArgsCheck< __VA_ARGS__ >().compare(args)) { \
			return JsVoidObjectFuncCall< Class, decltype(& Class :: Name) __VA_OPT__(,) __VA_ARGS__ >(this, & Class :: Name).run(args, isolate); \
		} \
	} \
	inline static const JsVoidObjectFunction<Class, decltype(& Class :: ___wrap_##Name), v8::Isolate *, const v8::FunctionCallbackInfo<v8::Value> &> ___static_js_wrap_##Name {#Name, & Class :: ___wrap_##Name}; \
	void Name( __VA_ARGS__ );

#define JS_EXPOSE_INHERITS(Class, Parent) \
	/* We have to provide the function pointer, if we do call it from within the class, we get some compilation errors that are strange. */ \
	inline static JsObjectInheritance<Class, Parent, decltype(&v8pp::class_< Class >::inherit< Parent >)> ___static_js_inherits_##Parent {& v8pp::class_< Class >::inherit< Parent > };

#define JS_EXPOSE_CTOR(Class, arg0, ...) \
	inline static JsObjectConstructor<Class, arg0 __VA_OPT__(,) __VA_ARGS__ > ___static_js_##GLUE(arg0 __VA_OPT__(,) __VA_ARGS__) { &v8pp::class_< Class >::ctor< __VA_ARGS__ > };

#define JS_EXPOSE_VAR(Class, Name) \
	inline static JsObjectVariable<Class, decltype(& Class :: Name)> ___static_js_var_##Name {#Name, & Class :: Name};

#define JS_EXPOSE_PROP(Class, Name) \
	inline static JsObjectProperty<Class, decltype(& Class :: Name)> ___static_js_prop_##Name {#Name, & Class :: Name};

#define JS_EXPOSE_VOID_FUNC(Name, ...) \
	inline static JsVoidFunction<decltype(& Name) __VA_OPT__(,) __VA_ARGS__ > ___wrap_js_free_##Name {#Name, & Name};

#define JS_EXPOSE_FUNC(Name, Ret, ...) \
	inline static JsFunction<decltype(& Name), Ret __VA_OPT__(,) __VA_ARGS__ > ____wrap_js_free_##Name {#Name, & Name};


class JsEnvData
{
public:
	std::unique_ptr<node::MultiIsolatePlatform> _platform;
	v8::Isolate* iso;
	std::shared_ptr<node::ArrayBufferAllocator> allocator;
	v8::Local<v8::Context> ctx;
	std::unique_ptr<node::IsolateData, decltype(&node::FreeIsolateData)> isolate_data{nullptr, nullptr};
	std::unique_ptr<node::Environment, decltype(&node::FreeEnvironment)> env{ nullptr, nullptr };
	v8pp::module *mo;
	uv_loop_t loop;

	bool successfullRan = false;

	inline static bool v8_loaded = false;
};


class JsEnv
{
public:
	explicit JsEnv();
	~JsEnv();

	void run_js(const char *code);
	void installEnvironment();
	int RunNodeInstance(const std::vector<std::string> &args, const std::vector<std::string> &exec_args);

	int thread_pool_size;

private:
	JsEnvData data;
};


class JsAPI
{
	JS_EXPOSE_CLASS(JsAPI)

public:
	explicit JsAPI(bool none = false);
	virtual ~JsAPI();

	JS_VOID_OBJ_FUNC(JsAPI, requestConnection);

	void requestJobs(std::string a, std::string b);
	void getJobsResult();
	//CalendarEvent getNextJob() const;
	void load();
	std::string pluginName() const;
	std::string pluginVersion() const;
	std::string jsonSettings() const;
	void settingChanged(std::string name, std::string value) const;

private:
	int ret;
	JsEnv env;
};


#endif

