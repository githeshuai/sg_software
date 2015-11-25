// Copyright (c) 2013 Shotgun Software Inc.

#include "win_common.h"
#include "utf8_tools.h"

#include <boost/assign.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>
#include <boost/algorithm/string/join.hpp>

#include "ProcessManagerWin.h"
#include "BrowserHost.h"

namespace fs = ::boost::filesystem;

// environment variable to look for when a custom launch script is used
const wchar_t *gLauncherEnvVar = L"SHOTGUN_PLUGIN_LAUNCHER";

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
	// check to see if a non-default launcher has been specified in the environment:
	const wchar_t *env = _wgetenv(gLauncherEnvVar);
	if(env == NULL)
	{
		host->htmlLog("[ShotgunIntegration] Open \"" + path + "\"");
	}
	else 
	{
		std::ostringstream oss;
		oss << "[ShotgunIntegration] \"" << FB::wstring_to_utf8(env) << "\" \"" << path << "\"";
		host->htmlLog(oss.str());
	}

	// launch the file on the main thread:
	host->ScheduleOnMainThread(boost::shared_ptr<ProcessManagerWin>(), boost::bind(&ProcessManagerWin::_open, this, path));
}

/*
 *
 */
void ProcessManagerWin::_open(const std::string &rPath)
{
	// convert path from UTF8 multi-byte cstring to wstring:
	const std::wstring wPath = FB::utf8_to_wstring(rPath);

	// check to see if a non-default launcher has been specified in the environment:
	const wchar_t *env = _wgetenv(gLauncherEnvVar);

	// execute the launcher for the path:
	HINSTANCE ret = ShellExecuteW(
		NULL,									// HWND
		NULL,									// verb
		(env == NULL) ? wPath.c_str() : env, 	// file
		(env == NULL) ? NULL : wPath.c_str(),	// params
		NULL,									// cwd
		SW_SHOWDEFAULT							// show command
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
