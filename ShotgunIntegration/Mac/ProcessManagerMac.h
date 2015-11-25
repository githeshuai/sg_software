// Copyright (c) 2013 Shotgun Software Inc.

#ifndef ProcessManagerMac_h__
#define ProcessManagerMac_h__

#include <string>
#include <boost/noncopyable.hpp>

#include "../ProcessManager.h"

class ProcessManagerMac : public ProcessManager
{
public:
    void Open(const FB::BrowserHostPtr& host, const std::string &path);
    void _open(const std::string &path);

protected:
    ProcessManagerMac() {};
    ~ProcessManagerMac() {};
    friend class ProcessManager;
};
#endif // ProcessManagerMac_h__