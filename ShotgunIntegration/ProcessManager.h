// Copyright (c) 2013 Shotgun Software Inc.

#ifndef ProcessManager_h__
#define ProcessManager_h__

#include <vector>
#include <string>
#include <boost/process.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include "BrowserHost.h"

namespace bp = ::boost::process;

typedef boost::function<void (const FB::VariantList&)> PathCallback;
typedef boost::function<void (int, const std::string&, const std::string&)> ExecuteToolkitCallback;

namespace FB { class PluginWindow; }

/*
 * Base class used to execute toolkit commands
 */
class ProcessManager
{
public:
    static ProcessManager * get();

    virtual void 			Open(const FB::BrowserHostPtr& host, const std::string &path) = 0;

    FB::VariantMap 			ExecuteToolkitCommand(const FB::BrowserHostPtr& host,
									const std::string &pipelineConfigPath,
									const std::string &command,
									const std::vector<std::string> &args);

    void 					ExecuteToolkitCommandAsync(const FB::BrowserHostPtr& host,
									const std::string &pipelineConfigPath,
									const std::string &command,
									const std::vector<std::string> &args,
									const ExecuteToolkitCallback &cb);

protected:
    						ProcessManager();
    virtual 				~ProcessManager();

    virtual const char *	GetToolkitScriptName();
    virtual const char *	GetToolkitFallbackScriptName();

    virtual bp::child 		Launch(const std::string &exec,
    								const std::vector<std::string> &arguments);

    
private:
    void 					VerifyArguments(const std::string &pipelineConfigPath,
    								const std::string &command);

    FB::VariantMap 			_ExecuteToolkitCommand(const std::string &pipelineConfigPath,
									const std::string &command,
									const std::vector<std::string> &args);

    void 					_ExecuteToolkitCommandAsync(const std::string &pipelineConfigPath,
									const std::string &command,
									const std::vector<std::string> &args,
									const ExecuteToolkitCallback &cb);
};

#endif // ProcessManager_h__
