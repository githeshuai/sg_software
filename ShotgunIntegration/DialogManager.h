// Copyright (c) 2013 Shotgun Software Inc.

#ifndef DialogManager_h__
#define DialogManager_h__

#include <vector>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include "BrowserHost.h"
 
typedef boost::function<void (const FB::VariantList&)> PathCallback;
 
namespace FB { class PluginWindow; }
 
class DialogManager
{
public:
    static DialogManager* get();
    virtual void OpenFolderDialog(const FB::BrowserHostPtr& host, FB::PluginWindow* win, bool multi, const PathCallback& cb) = 0;
 
protected:
    DialogManager() {}
    virtual ~DialogManager() {}
};
 
#endif // DialogManager_h__