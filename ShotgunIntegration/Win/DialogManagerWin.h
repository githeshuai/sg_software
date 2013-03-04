// Copyright (c) 2013 Shotgun Software Inc.

#ifndef DialogManagerWin_h__
#define DialogManagerWin_h__

#include <boost/noncopyable.hpp>
#include <vector>
#include <string>

#include "../DialogManager.h"

class DialogManagerWin : public DialogManager
{
public:
    void OpenFolderDialog(const FB::BrowserHostPtr& host, FB::PluginWindow* win, bool multi, const PathCallback& cb);

protected:
    DialogManagerWin() {};
    ~DialogManagerWin() {};
    void _showFileDialog(HWND wnd, bool multi, const PathCallback& cb);
    friend class DialogManager;
};
#endif // DialogManagerWin_h__