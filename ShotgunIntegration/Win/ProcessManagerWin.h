// Copyright (c) 2013 Shotgun Software Inc.

#ifndef ProcessManagerWin_h__
#define ProcessManagerWin_h__

#include <string>

#include "../ProcessManager.h"

class ProcessManagerWin : public ProcessManager
{
public:
    void Open(const FB::BrowserHostPtr& host, const std::string &path);
    void _open(const std::string &path);
	bp::child Launch(
		const std::string &exec,
		const std::vector<std::string> &arguments);

protected:
    ProcessManagerWin() {};
    ~ProcessManagerWin() {};
    friend class ProcessManager;
};
#endif // ProcessManagerWin_h__