// Copyright (c) 2013 Shotgun Software Inc.

#pragma warning (push)
#pragma warning (disable : 4005)

#include "win_common.h"
#include <commdlg.h>
#include "utf8_tools.h"

#include "Win/PluginWindowlessWin.h"
#include "Win/PluginWindowWin.h"

#include <string>
#include <sstream>
#include <shlobj.h>
#include <boost/thread.hpp>
#include <boost/tokenizer.hpp>

#include "precompiled_headers.h" 
#include "DialogManagerWin.h"

#pragma warning (pop)

// Forward decls
LRESULT CALLBACK AllowDirectoriesProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
UINT_PTR CALLBACK OFNHookProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

DialogManager* DialogManager::get()
{
    static DialogManagerWin inst;
    return &inst;
}

void DialogManagerWin::OpenFolderDialog(const FB::BrowserHostPtr &host, FB::PluginWindow* win, bool multi, const PathCallback& cb)
{
    FB::PluginWindowWin* wndWin = dynamic_cast<FB::PluginWindowWin*>(win);
    FB::PluginWindowlessWin* wndlessWin = dynamic_cast<FB::PluginWindowlessWin*>(win);

    HWND browserWindow = wndWin ? wndWin->getBrowserHWND() : wndlessWin->getHWND();
    boost::thread dlgThread(&DialogManagerWin::_showFileDialog, this, host, browserWindow, multi, cb);
}

void DialogManagerWin::_showFileDialog(const FB::BrowserHostPtr &host, HWND wnd, bool multi, const PathCallback& cb)
{
	DWORD flags;
	FB::VariantList out;
	TCHAR Filestring[4096] = {0};

	// Configure the file dialog
	flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NODEREFERENCELINKS | OFN_ENABLEHOOK | OFN_ENABLESIZING;
	if (multi)
		flags |= OFN_ALLOWMULTISELECT;

	// Setup shared data for the dialog's procs
	WndProcData data;
	data.host = host;
	data.handledOk = false;

	// Fill out the settings for the dialog
    OPENFILENAME opf;
	::ZeroMemory(&opf, sizeof(opf));

    opf.lStructSize = sizeof(OPENFILENAME);
    opf.hwndOwner = wnd;
    opf.lpstrFile = Filestring;
    opf.nMaxFile = 4096;
    opf.Flags = flags;
	opf.lCustData = (LPARAM)&data;
	opf.lpfnHook = OFNHookProc;
	opf.lpstrFilter = _T("All\0*.*\0Images\0*.JPG;*.JPEG;*.BMP;*.PNG;*.TIFF;*.EXR\0");
	opf.nFilterIndex = 1;

	// Call the dialog
	BOOL res = ::GetOpenFileName(&opf);

	if (data.handledOk) {
		// handledOk is true if we took over for the Ok button being clicked, use shared data
		std::vector<fs::path>::iterator j;
		for (j = data.selection.begin(); j!=data.selection.end(); ++j) {
			fs::path fname = data.directory / *j;
			out.push_back(FB::wstring_to_utf8(fname.wstring()));
		}
	} else if (res) {
		// res is true if somebody double clicked a file, need to process standard results
		if(opf.nFileOffset < lstrlen(opf.lpstrFile)) {
			// Single-Select
			out.push_back(FB::wstring_to_utf8(opf.lpstrFile));
	    } else {
			// Multi-Select
			fs::path directory(opf.lpstrFile);

			while (opf.lpstrFile[opf.nFileOffset] != 0) {
				// Add the path
				fs::path fname = directory / fs::path(opf.lpstrFile + opf.nFileOffset);
				out.push_back(FB::wstring_to_utf8(fname.wstring()));

				// Setup for the next round
				opf.nFileOffset += (wcslen(opf.lpstrFile+opf.nFileOffset) + 1);
			}
        }
    }

	// Call the callback
    cb(out);
}

LRESULT CALLBACK AllowDirectoriesProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WndProcData* pData = (WndProcData*)::GetWindowLong(hWnd, GWL_USERDATA);

	if(uMsg == WM_COMMAND && HIWORD(wParam) == BN_CLICKED) {
		if(LOWORD(wParam) == IDOK) {
			TCHAR buffer[4096];

			// Get the directory
			::SendMessage(hWnd, CDM_GETFOLDERPATH, 4096, (LPARAM)buffer);
			pData->directory = fs::path(buffer);

			// Get the filenames
			::SendMessage(hWnd, CDM_GETSPEC, 4096, (LPARAM)buffer);

			// Split the spec string to grab individual file names
			typedef boost::escaped_list_separator<wchar_t, std::wstring::traits_type> Seperator;
			typedef boost::tokenizer<Seperator, std::wstring::const_iterator, std::wstring> Tokenizer;
			typedef std::multimap<fs::path, bool> PathMap;
			typedef std::vector<fs::path> PathVector;

			Seperator seps(L"", L" ", L"\"");
			std::wstring spec(buffer);
			Tokenizer tok(spec, seps);
			PathVector specPaths;
			for (Tokenizer::iterator i=tok.begin(); i!=tok.end(); ++i) {
				std::wstring fname(*i);
				if (!fname.empty())
					specPaths.push_back(fname);
			}

			// Get handle to the file list
			HWND shellHWnd = ::FindWindowEx(hWnd, NULL, L"SHELLDLL_DefView", NULL);
			HWND lstHWnd = ::FindWindowEx(shellHWnd, NULL, L"SysListView32", NULL);

			// Grab out selections
			HRESULT res = ::SendMessage(lstHWnd, LVM_GETITEMCOUNT, 0, 0);
			LVITEM lvi = {0};
			lvi.mask = LVIF_TEXT;
			lvi.cchTextMax = 4096;
			PathMap selection;
			for (int i=0; i<res; i++) {
				lvi.iItem = i;
				lvi.pszText = buffer;
				::SendMessage(lstHWnd, LVM_GETITEM, 0, (LPARAM)&lvi);
				HRESULT selected = ::SendMessage(lstHWnd, LVM_GETITEMSTATE, i, LVIS_SELECTED);
				if (selected)
					selection.insert(std::make_pair(fs::path(lvi.pszText), false));
			}
			
			// Account for all specs
			for (PathVector::const_iterator i=specPaths.begin(); i!=specPaths.end(); ++i) {
				const fs::path &fname = *i;
				if (selection.count(fname) == 0) {
					// file not in selection, it might have had its extension stripped
					if (fname.has_extension()) {
						fs::path stem = fname.stem();
						size_t matches = selection.count(stem);
						if (matches > 0) {
							// stem in selection (only erase one, since there may be mulitple matches)
							pData->selection.push_back(fname);
							selection.erase(selection.find(stem));
						}
					}
				} else {
					// file directly in selection
					pData->selection.push_back(fname);
					selection.erase(selection.find(fname));
				}
			}

			// Everything left in the selection have not been accounted for. Must be directories.
			for (PathMap::iterator i=selection.begin(); i!=selection.end(); ++i)
				pData->selection.push_back(i->first);

			// And post a cancel so the dialog goes away even if only directories were selected
			pData->handledOk = true;
			::PostMessage(hWnd, WM_COMMAND, IDCANCEL, 0);
			return 1;
		}
	}

	// Fall through to the previous wndProc
	return ::CallWindowProc(pData->wndProc, hWnd, uMsg, wParam, lParam);
}

UINT_PTR CALLBACK OFNHookProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uiMsg)
	{
		case WM_NOTIFY:
		{
			LPNMHDR hdr = (LPNMHDR)lParam;
			switch (hdr->code)
			{
				case CDN_INITDONE:
				{
					// Grab our handle to the DialogManagerWin
					LPOFNOTIFY lpofn = (LPOFNOTIFY)lParam;
					WndProcData* pData = (WndProcData*)lpofn->lpOFN->lCustData;
	
					// Override the text on the Ok button
					::SendMessage(GetParent(hWnd), CDM_SETCONTROLTEXT, IDOK, (LPARAM)_T("&Select"));
	
					// "Subclass" the dialog to use our own wndproc
					::SetWindowLong(GetParent(hWnd), GWL_USERDATA, (long)pData);
					pData->wndProc = (WNDPROC)::SetWindowLongPtr(GetParent(hWnd), GWLP_WNDPROC, (LPARAM)AllowDirectoriesProc);

					break;
				} // CDN_INITDONE
			} // hdr->code
		} // WM_NOTIFY
	} // uiMsg

	return NULL;
}
