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
	const char *x11Path = path.c_str();
	pid_t pid = fork();

	const char *wrapper = getenv("SHOTGUN_WRAPPER");

	switch(pid) {
		case 0: // child
		    if (wrapper!=NULL) {
		       execlp("env", "env", wrapper, x11Path, NULL);
		    } else {
		       execlp("xdg-open", "xdg-open", x11Path, NULL);
		    }
			// should never return from execlp
			exit(1);
		default: {
			// parent
		    if (wrapper!=NULL) {
		    	host->htmlLog(
					"[ShotgunIntegration] Launched wrapper \"" +
					boost::lexical_cast<std::string>(wrapper) + "\" on \"" +
					path +
					"\" (pid " +
					boost::lexical_cast<std::string>(pid) + ")"
				);
		    } else {
			    host->htmlLog(
                    "[ShotgunIntegration] Launched xdg-open \"" +
                    path +
                    "\" (pid " +
                    boost::lexical_cast<std::string>(pid) + ")"
			    );
		    }
		}
	}
}
