#include "win_common.h"
#include <commdlg.h>
#include <string>
#include <boost/thread.hpp>
#include "utf8_tools.h"
#include "Win/PluginWindowlessWin.h"
#include "Win/PluginWindowWin.h"

#include "DialogManagerWin.h"
#include <shlobj.h>
#include "precompiled_headers.h" 

DialogManager* DialogManager::get()
{
    static DialogManagerWin inst;
    return &inst;
}


void DialogManagerWin::OpenFolderDialog(const FB::BrowserHostPtr& host, FB::PluginWindow* win, bool multi, const PathCallback& cb)
{
    FB::PluginWindowWin* wndWin = dynamic_cast<FB::PluginWindowWin*>(win);
    FB::PluginWindowlessWin* wndlessWin = dynamic_cast<FB::PluginWindowlessWin*>(win);

    HWND browserWindow = wndWin ? wndWin->getBrowserHWND() : wndlessWin->getHWND();
    boost::thread dlgThread(&DialogManagerWin::_showFileDialog, this, browserWindow, multi, cb);
}

void DialogManagerWin::_showFileDialog(HWND wnd, bool multi, const PathCallback& cb)
{
	DWORD flags;
	FB::VariantList out;
	wchar_t Filestring[4096] = {0};

	// Configure the file dialog
	flags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_HIDEREADONLY;
	if (multi)
		flags |= OFN_ALLOWMULTISELECT;

	// Fill out the settings for the dialog
    OPENFILENAME opf;
	ZeroMemory(&opf, sizeof(opf));

    opf.lStructSize = sizeof(OPENFILENAME);
    opf.hwndOwner = wnd;
    opf.lpstrFile = Filestring;
    opf.nMaxFile = 4096;
    opf.Flags = flags;

	// Call the dialog
    if(GetOpenFileName(&opf)) {
		// Process the results
		if(opf.nFileOffset < lstrlen(opf.lpstrFile)) {
			// Single-Select
			out.push_back(FB::wstring_to_utf8(std::wstring(opf.lpstrFile)));
	    } else {
			// Multi-Select
			wchar_t FilePath[4096];
			wchar_t FileName[4096];

			wcscpy(FilePath, opf.lpstrFile);
			FilePath[opf.nFileOffset] = 0;
			while (opf.lpstrFile[opf.nFileOffset] != 0) {
				wcscpy(FileName, FilePath);
				wcscat(FileName, L"\\");
				wcscat(FileName, opf.lpstrFile+opf.nFileOffset);
				out.push_back(FB::wstring_to_utf8(std::wstring(FileName)));
				opf.nFileOffset += (wcslen(opf.lpstrFile+opf.nFileOffset) + 1);
			}
        }
    }

	// Call the callback
    cb(out);
}