// Copyright (c) 2013 Shotgun Software Inc.

#ifndef ProcessManagerWin_h__
#define ProcessManagerWin_h__

#include <string>
#include "../ProcessManager.h"

class ProcessManagerWin : public ProcessManager
{
public:
	virtual void 			Open(const FB::BrowserHostPtr& host, const std::string &path);

protected:
							ProcessManagerWin();
	virtual 				~ProcessManagerWin();

	virtual const char *	GetToolkitScriptName();
	virtual const char *	GetToolkitFallbackScriptName();

	virtual bp::child 		Launch(const std::string &exec,
								const std::vector<std::string> &arguments);

private:
	void 					_open(const std::string &rPath);

	friend class ProcessManager;
};
#endif // ProcessManagerWin_h__
