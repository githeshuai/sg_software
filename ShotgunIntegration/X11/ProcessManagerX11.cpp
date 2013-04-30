// Copyright (c) 2013 Shotgun Software Inc.

#include <string>
#include <vector>
#include <boost/thread.hpp>
#include "logging.h"

#include "ProcessManagerX11.h"
#include "BrowserHost.h"

ProcessManager* ProcessManager::get()
{
    static ProcessManagerX11 inst;
    return &inst;
}

void ProcessManagerX11::Open(const FB::BrowserHostPtr& host, const std::string &path)
{
    host->ScheduleOnMainThread(boost::shared_ptr<ProcessManagerX11>(), boost::bind(&ProcessManagerX11::_open, this, host, path));
}

void ProcessManagerX11::_open(const FB::BrowserHostPtr& host, const std::string &path)
{
	char *env;
	std::string launcher;
	const char *x11Path = path.c_str();
	pid_t pid = fork();

	env = getenv("SHOTGUN_PLUGIN_LAUNCHER");
	launcher = (env == NULL) ? "xdg-open" : std::string(env);

	switch(pid) {
		case 0: // child
			execlp(launcher.c_str(), launcher.c_str(), x11Path, NULL);
			// should never return from execlp
			exit(1);
		default: {
			// parent
			host->htmlLog(
				"[ShotgunIntegration] Launched \"" +
				launcher + " " +
				path +
				"\" (pid " +
				boost::lexical_cast<std::string>(pid) + ")"
			);
		}
	}
}
