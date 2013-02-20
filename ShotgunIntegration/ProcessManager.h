#ifndef ProcessManager_h__
#define ProcessManager_h__

#include <vector>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include "BrowserHost.h"

typedef boost::function<void (const FB::VariantList&)> PathCallback;

namespace FB { class PluginWindow; }

class ProcessManager
{
public:
    static ProcessManager* get();
    virtual void Open(const FB::BrowserHostPtr& host, const std::string &path) = 0;

protected:
    ProcessManager() {}
    virtual ~ProcessManager() {}
};

#endif // ProcessManager_h__