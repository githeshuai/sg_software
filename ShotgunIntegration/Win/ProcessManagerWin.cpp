// Copyright (c) 2013 Shotgun Software Inc.

#include "win_common.h"

#include <boost/assign.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>
#include <boost/algorithm/string/join.hpp>

#include "ProcessManagerWin.h"
#include "BrowserHost.h"

namespace fs = ::boost::filesystem;

// local decls for utility functions
void RaiseWindowsError(const std::string &msg, const std::string &extra = std::string());
fs::wpath TempFilePath();

ProcessManager* ProcessManager::get()
{
    static ProcessManagerWin inst;
    return &inst;
}

void ProcessManagerWin::Open(const FB::BrowserHostPtr& host, const std::string &path)
{
	char *env;

	env = getenv("SHOTGUN_PLUGIN_LAUNCHER");
	if(env == NULL)
	    host->htmlLog("[ShotgunIntegration] Open \"" + path + "\"");
	else {
		std::string envStr(env);
	    host->htmlLog("[ShotgunIntegration] \"" + envStr + "\" \"" + path + "\"");
	}

	host->ScheduleOnMainThread(boost::shared_ptr<ProcessManagerWin>(), boost::bind(&ProcessManagerWin::_open, this, path));
}

void ProcessManagerWin::_open(const std::string &path)
{
	HINSTANCE ret;
	char *env;

	env = getenv("SHOTGUN_PLUGIN_LAUNCHER");
	ret = ShellExecuteA(
		NULL,                                // HWND
		NULL,                                // verb
		(env == NULL) ? path.c_str() : env,  // file
		(env == NULL) ? NULL : path.c_str(), // params
		NULL,                                // cwd
		SW_SHOWDEFAULT                       // show command
	);
}

FB::VariantMap ProcessManagerWin::_ExecuteToolkitCommand(
    const std::string &pipelineConfigPath,
    const std::string &command,
    const std::vector<std::string> &args)
{   
    try {
        VerifyArguments(pipelineConfigPath, command);
    
		// tmp files for stdout/stderr
		fs::wpath stdoutTmp = TempFilePath();
		fs::wpath stderrTmp = TempFilePath();
		
		// Build command line
		fs::wpath pcPath = pipelineConfigPath;
        fs::wpath exec = pcPath / "shotgun.bat";

        if (!fs::is_regular_file(exec))
            exec = pcPath / "tank.bat";

		std::vector<std::wstring> arguments = boost::assign::list_of
			(std::wstring(command.begin(), command.end()));
		for (std::vector<std::string>::const_iterator i=args.begin(); i!=args.end(); ++i)
			arguments.push_back(std::wstring(TEXT("\"")) + std::wstring(i->begin(), i->end()) + TEXT("\""));

		std::wstring cmdline = boost::algorithm::join(arguments, TEXT(" "));

		// boost::process uses CreateProcess, which is not setup to run batch files directly
		// in a proper envrionment.  In addition xcopy runs into issues when its stdout is
		// redirected.  The simplest working approach is to special case windows and use
		// ShellExecuteEx with the output of the command redirected via standard command
		// line style redirection into temp files.
		cmdline += TEXT(" > ") + stdoutTmp.wstring() + TEXT(" 2> ") + stderrTmp.wstring();

		// Run the command
		SHELLEXECUTEINFO ShExecInfo = {0};
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = exec.wstring().c_str();	
		ShExecInfo.lpParameters = cmdline.c_str();
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_HIDE;
		ShExecInfo.hInstApp = NULL;	
		
		BOOL bResult = ShellExecuteEx(&ShExecInfo);
		if (!bResult)
			RaiseWindowsError("Error in ShellExecuteEx", std::string(cmdline.begin(), cmdline.end()).c_str());
		WaitForSingleObject(ShExecInfo.hProcess,INFINITE);

		DWORD dwReturnCode = 0;
		bResult = GetExitCodeProcess(ShExecInfo.hProcess, &dwReturnCode);
		if (!bResult)
			RaiseWindowsError("Error in GetExitCodeProcess", std::string(cmdline.begin(), cmdline.end()).c_str());

		int retcode = dwReturnCode;

		// read temp file containing stdout
		std::ifstream ifStdout(stdoutTmp.string(), std::ios::in | std::ios::binary);
		if (!ifStdout)
			RaiseWindowsError("Error reading stdout");
		std::string strStdout;
		ifStdout.seekg(0, std::ios::end);
		if (ifStdout.tellg() > 0) {
			strStdout.resize(ifStdout.tellg());
			ifStdout.seekg(0, std::ios::beg);
			ifStdout.read(&strStdout[0], strStdout.size());
		}
		ifStdout.close();

		// read temp file containing stderr
		std::ifstream ifStderr(stderrTmp.string(), std::ios::in | std::ios::binary);
		if (!ifStderr)
			RaiseWindowsError("Error reading stderr");
		std::string strStderr;
		ifStderr.seekg(0, std::ios::end);
		if (ifStderr.tellg() > 0) {
			strStderr.resize(ifStderr.tellg());
			ifStderr.seekg(0, std::ios::beg);
			ifStderr.read(&strStderr[0], strStderr.size());
		}
		ifStderr.close();

		// clean up the temp files
		DeleteFile(stdoutTmp.wstring().c_str());
		DeleteFile(stderrTmp.wstring().c_str());

		// and return the results
		return FB::variant_map_of<std::string>
            ("retcode", retcode)
            ("out", strStdout)
            ("err", strStderr);
    } catch (std::exception &e) {
        // May be running in a non-main thread.  Avoid propagating exception
        return FB::variant_map_of<std::string>
            ("retcode", -1)
            ("out", std::string(""))
            ("err", std::string(e.what()));
    }
}

fs::wpath TempFilePath()
{ 
        std::vector<wchar_t> buffer(MAX_PATH); 
		DWORD len = ::GetTempPath(buffer.size(), &buffer[0]); 

        std::wstring directory(&buffer[0], buffer.size()); 
        if (!GetTempFileName(directory.c_str(), NULL, 0, &buffer[0]))
			RaiseWindowsError("Error getting tmpfile");
        std::wstring file(&buffer[0], buffer.size());                 
        return fs::wpath(file);
}

void RaiseWindowsError(const std::string &msg, const std::string &extra)
{
	DWORD error = GetLastError();
	if (error) {
		LPVOID lpMsgBuf;
		DWORD bufLen = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR) &lpMsgBuf,
			0, NULL);
		if (bufLen) {
			LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
			std::string result(lpMsgStr, lpMsgStr+bufLen);
			LocalFree(lpMsgBuf);

			std::ostringstream oss;
			oss << msg << " [" << error << ": " << result << "]";
			if (!extra.empty())
				oss << ": " << extra;
			throw std::exception(oss.str().c_str());
		}
	}
}