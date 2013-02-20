#ifndef ProcessManagerX11_h__
#define ProcessManagerX11_h__

#include <string>
#include <boost/noncopyable.hpp>

#include "../ProcessManager.h"

class ProcessManagerX11 : public ProcessManager
{
public:
    void Open(const FB::BrowserHostPtr& host, const std::string &path);
    void _open(const FB::BrowserHostPtr& host, const std::string &path);

protected:
    ProcessManagerX11() {};
    ~ProcessManagerX11() {};
    friend class ProcessManager;
};
#endif // ProcessManagerX11_h__