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
    friend class ProcessManager;
};
#endif // ProcessManagerWin_h__