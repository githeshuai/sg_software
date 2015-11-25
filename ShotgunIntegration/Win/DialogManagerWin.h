// Copyright (c) 2013 Shotgun Software Inc.

#ifndef DialogManagerWin_h__
#define DialogManagerWin_h__

#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>

#include "../DialogManager.h"

namespace fs = boost::filesystem;

class DialogManagerWin : public DialogManager
{
public:
    void OpenFolderDialog(const FB::BrowserHostPtr &host, FB::PluginWindow* win, bool multi, const PathCallback& cb);

protected:
    DialogManagerWin() {};
    ~DialogManagerWin() {};
    void _showFileDialog(const FB::BrowserHostPtr &host, HWND wnd, bool multi, const PathCallback& cb);
    friend class DialogManager;
};

class WndProcData
{
public:
	WNDPROC wndProc;
	FB::BrowserHostPtr host;
	BOOL handledOk;
	fs::path directory;
	std::vector<fs::path> selection;
};

#endif // DialogManagerWin_h__