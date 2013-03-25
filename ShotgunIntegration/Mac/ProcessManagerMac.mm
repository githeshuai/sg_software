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
    host->htmlLog("[ShotgunIntegration] Open \"" + path + "\"");
    host->ScheduleOnMainThread(boost::shared_ptr<ProcessManagerMac>(), boost::bind(&ProcessManagerMac::_open, this, path));
}

void ProcessManagerMac::_open(const std::string &path)
{
    NSString *macPath = [NSString stringWithCString:path.c_str() encoding:[NSString defaultCStringEncoding]];
    [[NSWorkspace sharedWorkspace] openFile:macPath];
}