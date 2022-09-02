#include "bindings.h"

#include "calendar_service_plugin_interface.h"

#include "libplatform/libplatform.h"

#include <QFile>
#include <QDir>

#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>


void printWelcome()
{
	std::cout << "-----> Welcome to NodeJs integration :)" << std::endl;
}

JS_EXPOSE_VOID_FUNC(printWelcome)


void RegisterModule(v8::Local<v8::Object> exports)
{
	v8::Isolate *iso = v8::Isolate::GetCurrent();
    v8pp::module addon(iso);

	JsObjectRegistry::instance()->build(&addon, iso);
    v8::Local<v8::Object> instanciated = addon.new_instance();

    exports->SetPrototype(iso->GetCurrentContext(), instanciated);
}


	//NODE_MODULE(addon, RegisterModule)
	std::unique_ptr<v8::Platform> platform;


// Reads a file into a v8 string.
v8::MaybeLocal<v8::String> ReadFile(v8::Isolate* isolate, const char* name) {
	FILE* file = fopen(name, "rb");
	if (file == NULL) {
		return v8::MaybeLocal<v8::String>();
	}

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	rewind(file);

	char* chars = new char[size + 1];
	chars[size] = '\0';
	for (size_t i = 0; i < size;) {
		i += fread(&chars[i], 1, size - i, file);
		if (ferror(file)) {
			fclose(file);
			return v8::MaybeLocal<v8::String>();
		}
	}
	fclose(file);
	v8::MaybeLocal<v8::String> result = v8::String::NewFromUtf8(isolate, 
			chars, 
			v8::NewStringType::kNormal, 
			static_cast<int>(size));
	delete[] chars;
	return result;
}


void JsEnv::installEnvironment()
{
	data.mo = new v8pp::module(data.iso);
	data.iso->GetCurrentContext()->Global()->Set(v8::String::NewFromUtf8(data.iso, "corejs"), data.mo->new_instance());
	JsEnvRegistry::instance()->exposeEnvironment(data.mo, data.iso);
}


int JsEnv::RunNodeInstance(const std::vector<std::string>& args, const std::vector<std::string>& exec_args)
{
	if (!data.successfullRan) {
		std::cout << "Starting execution." << std::endl;
		int exit_code = 0;
		// Set up a libuv event loop.
		data.loop = uv_loop_t();
		int ret = uv_loop_init(&data.loop);
		if (ret != 0) {
			fprintf(stderr, "%s: Failed to initialize loop: %s\n",
							args[0].c_str(),
							uv_err_name(ret));
			return 1;
		}

		data.allocator = node::ArrayBufferAllocator::Create();

		data.iso = node::NewIsolate(data.allocator.get(), &data.loop, data._platform.get());
		if (data.iso == nullptr) {
			fprintf(stderr, "%s: Failed to initialize V8 Isolate\n", args[0].c_str());
			return 1;
		}

		v8::Locker locker(data.iso);
		v8::Isolate::Scope isolate_scope(data.iso);


		// Create a node::IsolateData instance that will later be released using
		// node::FreeIsolateData().
		data.isolate_data = std::unique_ptr<node::IsolateData, decltype(&node::FreeIsolateData)>(
															node::CreateIsolateData(data.iso, &data.loop, data._platform.get(), data.allocator.get()),
															node::FreeIsolateData);

		// Set up a new v8::Context.
		v8::HandleScope handle_scope(data.iso);
		data.ctx = node::NewContext(data.iso);
		if (data.ctx.IsEmpty()) {
			fprintf(stderr, "%s: Failed to initialize V8 Context\n", args[0].c_str());
			return 1;
		}

		std::cout << "Creating the NodeJs env." << std::endl;

		// The v8::Context needs to be entered when node::CreateEnvironment() and
		// node::LoadEnvironment() are being called.
		v8::Context::Scope context_scope(data.ctx);

		// Create a node::Environment instance that will later be released using
		// node::FreeEnvironment().
		data.env = std::unique_ptr<node::Environment, decltype(&node::FreeEnvironment)>(
				node::CreateEnvironment(data.isolate_data.get(), data.ctx, args, exec_args),
				node::FreeEnvironment);

		std::cout << "Setting up local Js data." << std::endl;

		//Expose what is declared into v8pp.
		installEnvironment();
		std::cout << "Running Js code." << std::endl;

		// Set up the Node.js instance for execution, and run code inside of it.
		// There is also a variant that takes a callback and provides it with
		// the `require` and `process` objects, so that it can manually compile
		// and run scripts as needed.
		// The `require` function inside this script does *not* access the file
		// system, and can only load built-in Node.js modules.
		// `module.createRequire()` is being used to create one that is able to
		// load files from the disk, and uses the standard CommonJS file loader
		// instead of the internal-only `require` function.
		v8::MaybeLocal<v8::Value> loadenv_ret = node::LoadEnvironment(
				data.env.get(),
				"const publicRequire = require('module').createRequire(process.cwd() + '/');"
				"globalThis.require = publicRequire;"
				"const req = require('module').createRequire('/home/nicolas/');"
				"const rtm = req('/home/nicolas/scripted.js');"
				"console.log(req);"
				"console.log(rtm);"
				"console.log(rtm.generateMessage);"
			);

		//[TODO] Alterate the function of require to only load what it should.
		//[TODO] Catch if an error occured and if so, disable running js. Currently it is considered as fatal, making the process stop.

		if (loadenv_ret.IsEmpty()) {	// There has been a JS exception.
			std::cout << "Execution of Js code failed" << std::endl;
			return 1;
		}


		std::cout << "Execution of Js code succeed" << std::endl;
		// SealHandleScope protects against handle leaks from callbacks.
		v8::SealHandleScope seal(data.iso);
		bool more = true;
		while (more == true) {
			uv_run(&data.loop, UV_RUN_DEFAULT);
			// V8 tasks on background threads may end up scheduling new tasks in the
			// foreground, which in turn can keep the event loop going. For example,
			// WebAssembly.compile() may do so.
			data._platform.get()->DrainTasks(data.iso);
			// If there are new tasks, continue.
			more = uv_loop_alive(&data.loop);
			if (more) continue;
			// node::EmitBeforeExit() is used to emit the 'beforeExit' event on
			// the `process` object.
			node::EmitBeforeExit(data.env.get());
			// 'beforeExit' can also schedule new work that keeps the event loop
			// running.
			more = uv_loop_alive(&data.loop);
		}

		std::cout << "Exiting from NodeJs." << std::endl;

		// node::EmitExit() returns the current exit code.
		exit_code = node::EmitExit(data.env.get());
	} else {
		//We have to run a script.
	}
}


JsEnv::JsEnv()
{
	thread_pool_size = 4;

	//Setup dummy values for the startup.
	int argc = 1;
	const char *argv_t[1] = {""};
	char **argv = new char *[1];

	int _len = 1;

	int i = 0;
	while (i < _len) {
		char *data = new char[strlen(argv_t[i])];
		memcpy(data, argv_t[i], sizeof(char)*strlen(argv_t[i]));
		argv[i] = data;
		++i;
	}

	//Setup libuv
	char **argv_uv = uv_setup_args(argc, argv);
	
	//Cleanup allocated resources
	i = 0;
	while (i < _len) {
		delete[] argv[i];
		++i;
	}
	delete[] argv;

	//Continue libuv's setup.
	std::vector<std::string> args(argv_uv, argv_uv + argc);
	std::vector<std::string> exec_args;
	std::vector<std::string> errors;

	// Print any error that might have happened.
	int exit_code = node::InitializeNodeWithArgs(&args, &exec_args, &errors);
	for (const std::string& error : errors) {
		fprintf(stderr, "%s: %s\n", args[0].c_str(), error.c_str());
	}

	if (exit_code != 0) {
		return;// exit_code;
	}


	// Create a v8::Platform instance. `MultiIsolatePlatform::Create()` is a way
	// to create a v8::Platform instance that Node.js can use when creating
	// Worker threads. When no `MultiIsolatePlatform` instance is present,
	// Worker threads are disabled.
	data._platform = node::MultiIsolatePlatform::Create(thread_pool_size);
	if (!data.v8_loaded) {
		data.v8_loaded = true;
		v8::V8::InitializePlatform(data._platform.get());
		v8::V8::Initialize();
	}

	//Run the first script.
	int ret = RunNodeInstance(args, exec_args);

	return; //ret;
}

JsEnv::~JsEnv()
{
	// Dispose the isolate, libuv and tear down v8.

	std::cout << "Closing Js env." << std::endl;

	// node::Stop() can be used to explicitly stop the event loop and keep
	// further JavaScript from running. It can be called from any thread,
	// and will act like worker.terminate() if called from another thread.
	node::Stop(data.env.get());

	// Unregister the Isolate with the platform and add a listener that is called
	// when the Platform is done cleaning up any state it had associated with
	// the Isolate.
	bool platform_finished = false;
	data._platform.get()->AddIsolateFinishedCallback(
		data.iso,
		[](void* data) {
			*static_cast<bool*>(data) = true;
		},
		&platform_finished
	);
	
	data._platform->UnregisterIsolate(data.iso);
	data.iso->Dispose();

	// Wait until the platform has cleaned up all relevant resources.
	while (!platform_finished) {
		uv_run(&data.loop, UV_RUN_ONCE);
	}
	int err = uv_loop_close(&data.loop);

	//Js closing failed due to a libuv issue.
	assert(err == 0);

	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();
}

void JsEnv::run_js(const char *code)
{
}


JsAPI::JsAPI(bool)
{
}

JsAPI::~JsAPI()
{
	// Dispose the persistent context.
	//context.Dispose();
}


void JsAPI::requestConnection()
{
	std::cout << "Requesting connection !!" << std::endl;
	if (false) {
		return;
	}
}

void JsAPI::requestJobs(std::string a, std::string b)
{
	if (false) {
		return;
	}
}

void JsAPI::getJobsResult()
{
	if (false) {
		return;
	}
}

//CalendarEvent getNextJob() const;

void JsAPI::load()
{
	if (false) {
		return;
	}
}

std::string JsAPI::pluginName() const
{
	if (true) {
		return "Undefined";
	}
	
}

std::string JsAPI::pluginVersion() const
{
	if (true) {
		return "Undefined";
	}
}

std::string JsAPI::jsonSettings() const
{
	if (true) {
		return "{}";
	}
}

void JsAPI::settingChanged(std::string name, std::string value) const
{
	if (false) {
		return;
	}
}

