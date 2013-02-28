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
    [oPanel setResolvesAliases:NO];
    
    result = [oPanel runModal];
    
    if (result == NSOKButton) {
        BOOL isDir;
        NSArray *filesToOpen = [oPanel filenames];
        NSFileManager *fm = [NSFileManager defaultManager];
        
        for ( NSString *path in filesToOpen ) {
            std::string strPath([path cStringUsingEncoding:[NSString defaultCStringEncoding]]);
            if ([fm fileExistsAtPath:path isDirectory:&isDir] && isDir)
                strPath.push_back('/');
            out.push_back(strPath);
        }
    }
    
    [pool release];
    
    cb(out);
}