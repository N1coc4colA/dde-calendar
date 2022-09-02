# Google Calendar Service Plugin
This plugin makes possible for Deepin Calendar (dde-calendar) to __show__ the content of the calendars associated
to your connected Google Account.

## Libraries
You need the __Qt__ & __Dtk__ libs used by dde-calendar, but also some __Python libraries__ to let it all run.
To install the Python libraries, use the following command:
```
# pip install --upgrade google-api-python-client google-auth-httplib2 google-auth-oauthlib
```

## Available features
For now, the plugin supports only a few things:
- Only the default calendar can be used.
- Might not support showing events in the proper way, but shows them.

What it cannot:
- Use multiple accounts.
- Add events (because not implemented in dde-calendar).
- Share, or show "metadata" of an event.
- Show any attachments of the event.

## The Python bindings
The Python to C++ binding is built manualy. However, there are some automations. If you want to use it for any
other code your are free to do so. All is in bindings.* files. CPythonObject wraps PyObject *, and has facilities
such as setAttribute, hasAttribute, attribute. Currently, hasMember and member are not implemented yet.
The support for function calls with arguments is not really supported. To achieve it here are the points:

### Never define it with arguments!
What will crash:
```
	def myfunc(self, a, b)
		print(a, b)
```

What will not crash:
```
	def myfunc(self):
		pass
```
If you want, anyway, to have some attributes, see the next point.

### Make them as some attributes :)
```
	def __init__(self):
		myfunc_0 = None
		myfunc_1 = None
```

### Full example
```
class Sample
	def __init__(self):
		myfunc_0 = None
		myfunc_1 = None

	def myfunc(self):
		print(myfunc_0, myfunc_1)
```

### How to expose it on the C++ side?
There are a few points that your class must implement.
If you want it to run well, you have to follow these guidelines.

#### 1: The operator.
It has to implement the CPyOperator:
```
	operator CPyObject();
```

#### 2: The basic members.
Your class has to provide a member named **obj** that, at least, inherits publicly of **__CPyObject__**, the header's minimum
is as follows:
```
class Sample
{
public:
	Sample(CPyObject); //If you want to support PY_OBJ_FUNC, see next point.
	operator Sample();

private:
	CPyObject obj = CPyObject();
};
```

#### 3: The MACROS you can use.
Here is a description of the macro's behaviour. Notice that it all has to be headerfull, as macros can define
functions as inline.
```
1. PY_ATTRIB(Type, Name, Default)
2. PY_STR_ATTRIB(Name)
3. PY_FUNC(Type, Name, Default)
4. PY_FUNC1(Name)
5. PY_STR_FUNC(Name, Default)
6. PY_OBJ_FUNC(Type, Name, Default)
7. PY_SPEC_FUNC_UNDEF(Name)
8. PY_SPEC_FUNC*(*)
9. PY_CALL(Name)
10. PY_RET_CALL(Name)
```
```
(1) Defines a function named Name to access the python function Name, returning the given type Type, if anything fails, return Default.
(2) = PY_ATTRIB(std::string, Name, "").
(3) Defines a function named Name to call the Python function Name, returning the type Type, if anything fails, return Default.
(4) Same as (4) but for void.
(5) Same as (4) but for std::string.
(6) Same as (4) but Type is a type handling a CPyObject.
(7) Defines a void function named Name, that accepts only one argument of type CPyObject.
(8) Custom functions that are undefined, and meant to only one-time use.
(9) Wrapper around PyCall function, giving out a CPyObject as result.
(10) Wrapper around PyCall function, equivalent to ```return <CPyObject>```.
```

#### 4: What about debugging?
If you want to debug, you can use a set of macros not mentionned before:
```
CPYO() -> Creates a CPyObject from a nullptr
CPYO_(data) -> Creates a CPyObject from the data, you can put any arbitrary data in the macro.
CPYO__(name, data) -> Does the same as CPYO_ except that it declares the variable Name. Like CPyObject Name(...).
```

#### 5: Enabling Python debug
If you're not using the Debug build, it is not enabled by default.
What you have to do is to define __DEBUG_PY_CALLS__.
It will not only print when attribute access fails, function call fails, it will also print any calls to the
CPyObject, such as conversions.
Using these are important, prefer them to a CPyObject Name = ...; declaration.
At runtime, the file, line at where the source pointer was created can be used to print additional data.

#### 6: Problems? Wanna go further?
The PyCall function wraps calling the Python function. You can use it or reimplement it as it supports
function arguments as parameters.
If you're worried bout the warning `nothing` at compile time from the ctor of CPyObject, 

##### -1: Impossible to get the function from C++
If your function is not visible, you can try to implement hasMember/member of CPyObject to really watch out
for everything (and more importantly, the object's functions). If you won't, you can declare an alias for
your function as is:
```
class Sample:
	def __init__(self):
		self.truthness = True

		self.getTruthness = self._getTruthness

	def _getTruthness(self):
		return self.truthness
```

In C++, you can make it as:
```
class Sample
{
public:
	inline explicit Sample() {}
	inline Sample(CPyObject source) : obj(source {}
	
	PY_ATTR(bool, thruthness, false);
	PY_FUNC(bool, getTruthness, false);

private:
	CPyObject obj = CPYO();
};
```

Then you can do this:
```
void someFunction(Sample s)
{
	bool bval = s.thruthness();
	bool bval2 = s.getThruthness();
	// Here, bval == bval2.
}
```

You can also access dumper functions such as \_\_dict\_\_. However, as GNU reserves all
\_\_<uppercase>*, you should avoid declaring such attributes, or any function to access it.
Prefer to build the whole function yourself, to avoid using __* in the C/C++ function name.

##### -2: I want to use it, but only for C, no C++
Then deal by yourself on how to use a macro to replace PyCall function, as it is the only
containing a template (variadic). Don't forget to replace the keyword `class` with `struct` ;)

##### -3: Where does the Python initialisation and import of the script happen then??
It all happens by the magic of computing. No, just joking.
For that, you can take a look at the content of bindings.cpp, in the CalendarAPI's ctor,
it does all the stuff. Notice that it is also implementing CPyObject, so you might also have
to separate the import of the object instanciation. What, you want to use arguments with the
\_\_init\_\_ function too? No, I have no clue about how to do so.

#### Other notes
- If you want to check that an object is stored, you should use CPyObject::is(), and not the default
CPyObject::operator bool() as it can retrieve wrong values if a type is present and convertible int boolean.
- The function printAttributes(CPyObject) can be used if you want to print available attributes of an object instance.
- How to export a C/C++ function to the Python code? That's not implemented yet, but that should be in a near future.
- If you want to only print called function while debugging, nothing else, undefine DEBUG (from CMakeFile.txt) and define DEBUG\_PY\_SYMBOLS, it will print functions called, not attributes accessed by functions.

## Building
Compile the plugin using CMake as:
```
mkdir build && cd build
cmake ../
make
```

### How to install
Just move the output library file to the right directory:
```
/usr/lib/dde-calendar
```

