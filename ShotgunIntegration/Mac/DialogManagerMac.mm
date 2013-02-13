#include <string>
#include <vector>
#include <boost/thread.hpp>
#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>
#include "logging.h"
 
#include "DialogManagerMac.h"
#include "BrowserHost.h"
 
DialogManager* DialogManager::get()
{
    static DialogManagerMac inst;
    return &inst;
}
 
 
void DialogManagerMac::OpenFolderDialog(const FB::BrowserHostPtr& host, FB::PluginWindow* win, bool multi, const PathCallback& cb)
{
    host->ScheduleOnMainThread(boost::shared_ptr<DialogManagerMac>(), boost::bind(&DialogManagerMac::_showFolderDialog, this, win, multi, cb));
}
 
void DialogManagerMac::_showFolderDialog(FB::PluginWindow* win, bool multi, const PathCallback& cb)
{
    FB::VariantList out;
    int result;
    NSAutoreleasePool* pool = [NSAutoreleasePool new];
    NSOpenPanel *oPanel = [NSOpenPanel openPanel];
    
    if ( multi )
        [oPanel setAllowsMultipleSelection:YES];
    else
        [oPanel setAllowsMultipleSelection:NO];
    
    [oPanel setCanChooseFiles:YES];
    [oPanel setCanChooseDirectories:YES];
    result = [oPanel runModalForDirectory:nil file:nil types:nil];
    
    if (result == NSOKButton) {
        NSArray *filesToOpen = [oPanel filenames];
        
        for ( NSString *path in filesToOpen ) {
            out.push_back([path cStringUsingEncoding:[NSString defaultCStringEncoding]]);
        }
    }
    
    [pool release];
    
    cb(out);
}