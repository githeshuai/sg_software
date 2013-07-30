// Copyright (c) 2013 Shotgun Software Inc.

#include "win_common.h"

#include <boost/assign.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>
#include <boost/algorithm/string/join.hpp>

#include "ProcessManagerWin.h"
#include "BrowserHost.h"

namespace fs = ::boost::filesystem;

/*
 * Factory method to return the correct instance of the
 * ProcessManager
 */
ProcessManager* ProcessManager::get()
{
    static ProcessManagerWin inst;
    return &inst;
}

/*
 * Construction
 */
ProcessManagerWin::ProcessManagerWin()
	: ProcessManager()
{}

/*
 * Destruction
 */
ProcessManagerWin::~ProcessManagerWin()
{}

/*
 * Return the name of the toolkit script
 */
const char * ProcessManagerWin::GetToolkitScriptName()
{
	return "shotgun.bat";
}

/*
 * Return the legacy name of the toolkit script as the
 * fall-back option
 */
const char * ProcessManagerWin::GetToolkitFallbackScriptName()
{
	return "tank.bat";
}

/*
 *
 */
void ProcessManagerWin::Open(const FB::BrowserHostPtr& host, const std::string &path)
{
	char *env;

	env = getenv("SHOTGUN_PLUGIN_LAUNCHER");
	if(env == NULL)
	    host->htmlLog("[ShotgunIntegration] Open \"" + path + "\"");
	else {
		std::string envStr(env);
	    host->htmlLog("[ShotgunIntegration] \"" + envStr + "\" \"" + path + "\"");
	}

	host->ScheduleOnMainThread(boost::shared_ptr<ProcessManagerWin>(), boost::bind(&ProcessManagerWin::_open, this, path));
}

/*
 *
 */
void ProcessManagerWin::_open(const std::string &path)
{
	HINSTANCE ret;
	char *env;

	env = getenv("SHOTGUN_PLUGIN_LAUNCHER");
	ret = ShellExecuteA(
		NULL,                                // HWND
		NULL,                                // verb
		(env == NULL) ? path.c_str() : env,  // file
		(env == NULL) ? NULL : path.c_str(), // params
		NULL,                                // cwd
		SW_SHOWDEFAULT                       // show command
	);
}

/*
 * Launch the specified toolkit command with the specified arguments
 */
bp::child ProcessManagerWin::Launch(const std::string &exec, const std::vector<std::string> &arguments)
{
	bp::win32_context ctx;

	ctx.environment = bp::self::get_environment();
    ctx.stdout_behavior = bp::capture_stream();
    ctx.stderr_behavior = bp::capture_stream();
    
	// have to capture stdin as well otherwise the xcopy
	// used in the batch file doesn't work correctly
	ctx.stdin_behavior = bp::capture_stream();

	// windows specific launch arguments
	STARTUPINFOA si;
    ::ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	ctx.startupinfo = &si;

	return bp::win32_launch(exec, arguments, ctx);
}
