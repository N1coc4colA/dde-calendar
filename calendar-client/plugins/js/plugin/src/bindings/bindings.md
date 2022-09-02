# Js bindings of C++ classes
Project to bind C++ classes to Js, usable in NodeJs context, meant to be plugins-like.
This project is built atop NodeJs, AND v8, trying to use it with SpiderMonkey, Chakra or JavaScriptCore is not supported. And is not planned.

## The goals
The goal of this project is to enable C++ to interact with Js code directly, and also the opposite.
It provides a class, **JsEnv** to run Js code.

## Requirements
To build this library, you need NodeJs libs, v8 libs, and v8pp, a smally customized version of the later is available.

## Implementation
### A plugin model
To make everything available, we use static objects and methods. This means that once you've interfaced properly your code, your classes, functions, are all available when using JsEnv.

### Some macros
A set of macros are defined to expose a JsObject-based class. I'll put JsObject-based objects/classes, but truth to be told, JsObject is not a class, it is just a conceptual thing, that an object is compliant to.

Here are the available macros, all:
```
(1) JS_EXPOSE_CLASS(Class)
(2) JS_OBJ_FUNC(Ret, Class, Name, ...)
(3) JS_VOID_OBJ_FUNC(Class, Name, ...)
(4) JS_EXPOSE_INHERITS(Class, Parent)
(5) JS_EXPOSE_CTOR(Class, arg0, ...)
(6) JS_EXPOSE_VAR(Class, Name)
(7) JS_EXPOSE_PROP(Class, Name)
(8) JS_EXPOSE_VOID_FUNC(Name, ...)
(9) JS_EXPOSE_FUNC(Name, Ret, ...)
```
You'll notice redundancy in the parameters, that's normal.
__Class__ has to be your class's name, it might be with the namespace too.
__Parent__ is a parent class of the class, it's when inheritance is involved.
__Ret__ has to be the **return type** of the functions, exclusing, yes, void.
__Name__ has to be the name of what you're exposing. Depending on the macro, it can be the one of a variable, function...
__arg0__ is used when v8, Js, v8pp or whatever you want requires at least one argument, like for the classes' constructors.
__...__ are the arguments, as type. Like int, bool, ClassWhatsoever, it can be empty.

So, to what these macros corresponds to?
```
(1) (*1) Put in the body of a class, declares members as public, be careful of what you put after. I recommend putting it before your ctors.
(2) (*2) Generates the declaration of the given function, and registers it to expose it to Js.
(3) (*2) The same as (2) but for void-returning functions.
(4) (*3) Tells Js that class inherits another. It might be relevant or not if the class is exposed. If it's not exposed, this will have no effect.
(5) (*4) Expose a constructor for the class to Js, it MUST at least have one argument to be Js-valid.
(6) (*5) Expose a variable to Js, being read-only for it.
(7) (*6) Expose a property, like (6) but you provide a getter and setter to let Js interact with it.
(8) (*7) Expose a void function that is not a member of class, or struct or anything else.
(9) (*7) The same as (8) but you choose the return type, that is non-void.
```
All these macros are not just decoration, there's not extra compilation like with Qt's MOC.
The macros generates extracode, functions, objects, generally static. Here is what they build:
```
(1) ___js_wrap_*, ___js_is_object
(2) ___wrap_*, ___static_js_wrap_*, function declaration
(3) ___static_js_inherits_*
(4) ___static_js_*
(5) ___static_js_var_*
(6) ___static_js_prop_*
(7) ___wrap_js_free_*
```
Unless you decided to use prefixes starting with "____", you should be alright. It is not because they does or doesn't contain "static" that they truly are!
The name of the members should change to ___js_* in the future.

### Template classes
List of template classes you might want to directly interact with:
```
JsFunction
JsVoidFunction
JsObjectFunction
JsVoidObjectFunction
JsObjectInheritance
JsObjectInheritance
JsObjectProperty
JsObjectVariable
```
They indirectly handle the exposure to Js.


### The registries
Registries are classes that directly registers everything you want to expose to Js. They are all *Registry. There is one for JsObject, JsEnv, JsFreeFunction (used for non-member).
When starting a JsEnv and the running a script might require their use. They might change in the future, but all their members are public. Any pointer that they might store must not
be deleted or free-ed as they are static objects.

### I want to use a custom name for the exposure.
At this time, the only name available is "corejs", module exposed over v8pp::module.
But in the future, the aim is to use the same model as for the objects registration to provide modules and submodules instead of just "corejs".

## Use
### Exposing just a function
```
#include "bindings.h"

void printWelcome()
{
	std::cout << "-----> Welcome to NodeJs integration :)" << std::endl;
}

JS_EXPOSE_VOID_FUNC(printWelcome)
```
When runing js code, you'll then be able to call the C++ function from js with "corejs.printWelcome();".

### Exposing a class
```
#include "bindings.h"

class JsTest
{
	JS_EXPOSE_CLASS(JsTest)

public:
	/* At least one parameter to make it instanciable from Js. */
	inline explicit JsTest(bool none = false);
	inline virtual ~JsTest();

	/* Expose void member function to Js. */
	JS_VOID_OBJ_FUNC(JsTest, printType);
};

inline JsTest::printType()
{
	std::cout << "I'm a JsTest instance!!" << std::endl;
}
```
You can now create an instance using "corejs.JsTest" and call on the object "printType();".

### Custom values
It might happen that your calls fails due to an object not exposed to Js, or you want to make conversions easier.
Then you should check out about v8pp's data converions, or v8's. But always prefer to use v8pp directly.

### I want to expose functions that are from another class
You can get as base the macros defined to figure out how to do so.
An easy way is to make a derived class, for example, you have the class A, with member function C. If you want to expose the class, you cal make a class B, that declares function D that calls A::C. Otherwise, expand the macros and change the class names. But be carefull, you'll be wrapping a parent class, so no JS_EXPOSE_INHERITS(A)!

## Behaviours
Beware of the JsValue converions. Also if a Js call is made and does not meet your requirements, no overload is done. If the number of arguments is not matched, or are not compatible, the C++ call to the real function is not made, but succeed from the point of view of Js, if they should return a value, a v8::Exception::Error is returned.

## I want to call a Js class/function from C++, not the opposite!
We also have solutions to that. We don't use v8pp here, only v8, but it's still compatible! You can even pass it to v8pp ;)

### The macros
These macros allows you to generate C++ functions that will call the Js function on your behalf.
```
(1) FROM_JS_FUNC(Ret, Name, ...)
(2) FROM_JS_VOID_FUNC(Name, ...)
(3) FROM_JS_VAR(Ret, Name)
(4) FROM_JS_PROP(Ret, Name)
```

What are these macros doing?
```
(1) Generates a caller for the target function, non-void returning.
(2) Same as (1) but void-returning.
(3)	Generates a getter for the variable.
(4) (*)Generates a getter and setter for the property.
```

(\*) generates a function set\_* and get\_* for the property.

## Licensing
This directory, and all its files are GPLv3 licensed

## Contributing
Read bindings.h file, you'll understand quickly the coding style that you have to follow.
Every function that comes from a template class or is a template has to be inline.
Don't worry about putting some comments and tags with [] if you make some modifications or anything.

