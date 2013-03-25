// Copyright (c) 2013 Shotgun Software Inc.

#include "win_common.h"
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>

#include "ProcessManagerWin.h"
#include "BrowserHost.h"

ProcessManager* ProcessManager::get()
{
    static ProcessManagerWin inst;
    return &inst;
}

void ProcessManagerWin::Open(const FB::BrowserHostPtr& host, const std::string &path)
{
    host->htmlLog("[ShotgunIntegration] Open \"" + path + "\"");
    host->ScheduleOnMainThread(boost::shared_ptr<ProcessManagerWin>(), boost::bind(&ProcessManagerWin::_open, this, path));
}

void ProcessManagerWin::_open(const std::string &path)
{
	HINSTANCE ret;

	ret = ShellExecuteA(
		NULL,          // HWND
		NULL,          // verb
		path.c_str(),  // file
		NULL,          // params
		NULL,          // cwd
		SW_SHOWDEFAULT // show command
	);
}