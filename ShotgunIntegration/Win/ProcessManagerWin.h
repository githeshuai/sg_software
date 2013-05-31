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

protected:
    ProcessManagerWin() {};
    ~ProcessManagerWin() {};

	FB::VariantMap _ExecuteTankCommand(
        const std::string &pipelineConfigPath,
        const std::string &command,
        const std::vector<std::string> &args);

    friend class ProcessManager;
};
#endif // ProcessManagerWin_h__