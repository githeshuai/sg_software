// Copyright (c) 2013 Shotgun Software Inc.

#include <string>
#include <vector>
#include <boost/thread.hpp>
#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>
#include "logging.h"

#include "ProcessManagerMac.h"
#include "BrowserHost.h"

// environment variable to look for when a custom launch script is used
const NSString * gLauncherEnvVar = @"SHOTGUN_PLUGIN_LAUNCHER";

ProcessManager* ProcessManager::get()
{
    static ProcessManagerMac inst;
    return &inst;
}

void ProcessManagerMac::Open(const FB::BrowserHostPtr& host, const std::string &path)
{
    // check to see if a non-defualt launcher has been specified in the environment:
    NSString *launcher = [[[NSProcessInfo processInfo] environment] objectForKey:gLauncherEnvVar];
    if (launcher == nil) {
        host->htmlLog("[ShotgunIntegration] Open \"" + path + "\"");
    } else {
        std::string launcherString([launcher UTF8String]);
        host->htmlLog("[ShotgunIntegration] \"" + launcherString + "\" \"" + path + "\"");
    }

    // always launch the file on the main thread:
    host->ScheduleOnMainThread(boost::shared_ptr<ProcessManagerMac>(), boost::bind(&ProcessManagerMac::_open, this, path));
}

void ProcessManagerMac::_open(const std::string &path)
{
    // check to see if a non-defualt launcher has been specified in the environment:
    NSString *launcher = [[[NSProcessInfo processInfo] environment] objectForKey:gLauncherEnvVar];

    // path is a UTF8 encoded cstring:
    NSString *macPath = [NSString stringWithUTF8String:path.c_str()];

    if (launcher == nil) {
        // use the system default launcher for this file
        [[NSWorkspace sharedWorkspace] openFile:macPath];
    } else {
        [[NSWorkspace sharedWorkspace] openFile:macPath withApplication:launcher];
    }
}