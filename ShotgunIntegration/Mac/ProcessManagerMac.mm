// Copyright (c) 2013 Shotgun Software Inc.

#include <string>
#include <vector>
#include <boost/thread.hpp>
#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>
#include "logging.h"

#include "ProcessManagerMac.h"
#include "BrowserHost.h"

ProcessManager* ProcessManager::get()
{
    static ProcessManagerMac inst;
    return &inst;
}

void ProcessManagerMac::Open(const FB::BrowserHostPtr& host, const std::string &path)
{
    NSString *launcher = [[[NSProcessInfo processInfo] environment] objectForKey:@"SHOTGUN_PLUGIN_LAUNCHER"];
    if (launcher == nil) {
        host->htmlLog("[ShotgunIntegration] Open \"" + path + "\"");
    } else {
        std::string launcherString([launcher UTF8String]);
        host->htmlLog("[ShotgunIntegration] \"" + launcherString + "\" \"" + path + "\"");
    }

    host->ScheduleOnMainThread(boost::shared_ptr<ProcessManagerMac>(), boost::bind(&ProcessManagerMac::_open, this, path));
}

void ProcessManagerMac::_open(const std::string &path)
{
    NSString *launcher = [[[NSProcessInfo processInfo] environment] objectForKey:@"SHOTGUN_PLUGIN_LAUNCHER"];
    NSString *macPath = [NSString stringWithCString:path.c_str() encoding:[NSString defaultCStringEncoding]];

    if (launcher == nil) {
        [[NSWorkspace sharedWorkspace] openFile:macPath];
    } else {
        [[NSWorkspace sharedWorkspace] openFile:macPath withApplication:launcher];
    }
}