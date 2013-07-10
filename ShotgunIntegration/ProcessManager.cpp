// Copyright (c) 2013 Shotgun Software Inc.

#include <boost/assign.hpp>
#include <boost/filesystem.hpp>

#include "ProcessManager.h"

namespace fs = ::boost::filesystem;

#define TOOLKIT_SCRIPT_NAME "shotgun"
#define TOOLKIT_FALLBACK_SCRIPT_NAME "tank"

void ProcessManager::VerifyArguments(const std::string &pipelineConfigPath, const std::string &command)
{
    try {
        if (!boost::starts_with(command, "shotgun"))
            throw FB::script_error("ExecuteTankCommand error");
    
        fs::path pcPath = pipelineConfigPath;
        if (!fs::is_directory(pcPath)) {
            std::string err = "Could not find the Pipeline Configuration on disk: " + pcPath.string();
            throw FB::script_error(err);
        }
    
        fs::path exec = pcPath / TOOLKIT_SCRIPT_NAME;

        if (!fs::is_regular_file(exec))
            exec = pcPath / TOOLKIT_FALLBACK_SCRIPT_NAME;
        
        if (!fs::is_regular_file(exec))
            throw FB::script_error("Could not find the Toolkit command on disk: " + exec.string());
    } catch (fs::filesystem_error &e) {
        std::string msg = std::string("Error finding the Toolkit command on disk: ") + e.what();
        throw FB::script_error(msg);
    }
}

FB::VariantMap ProcessManager::ExecuteToolkitCommand(
    const FB::BrowserHostPtr& host,
    const std::string &pipelineConfigPath,
    const std::string &command,
    const std::vector<std::string> &args)
{
    return _ExecuteToolkitCommand(pipelineConfigPath, command, args);
}

FB::VariantMap ProcessManager::_ExecuteToolkitCommand(
    const std::string &pipelineConfigPath,
    const std::string &command,
    const std::vector<std::string> &args)
{    
    try {
        VerifyArguments(pipelineConfigPath, command);
    
        fs::path pcPath = pipelineConfigPath;
        fs::path exec = pcPath / TOOLKIT_SCRIPT_NAME;

        if (!fs::is_regular_file(exec))
            exec = pcPath / TOOLKIT_FALLBACK_SCRIPT_NAME;

        std::vector<std::string> arguments = boost::assign::list_of(exec.string())(command);
        arguments.insert(arguments.end(), args.begin(), args.end());

		bp::context ctx;
		ctx.environment = bp::self::get_environment();
		ctx.stdout_behavior = bp::capture_stream();
		ctx.stderr_behavior = bp::capture_stream();

		bp::child child = bp::launch(exec.string(), arguments, ctx);
        bp::status status = child.wait();

        int retcode;
        if (status.exited())
            retcode = status.exit_status();
        else
            retcode = -1;

        std::string line;
        std::ostringstream ossStdout;
        bp::pistream &isStdout = child.get_stdout();
        while (std::getline(isStdout, line)) {
            ossStdout << line << std::endl;
        }
        
        std::ostringstream ossStderr;
        bp::pistream &isStderr = child.get_stderr();
        while (std::getline(isStderr, line)) {
            ossStderr << line << std::endl;
        }
        
        return FB::variant_map_of<std::string>
            ("retcode", retcode)
            ("out", ossStdout.str())
            ("err", ossStderr.str());
    } catch (std::exception &e) {
        // May be running in a non-main thread.  Avoid propagating exception
        return FB::variant_map_of<std::string>
            ("retcode", -1)
            ("out", std::string(""))
            ("err", std::string(e.what()));
    }
}

void ProcessManager::ExecuteToolkitCommandAsync(
        const FB::BrowserHostPtr& host,
        const std::string &pipelineConfigPath,
        const std::string &command,
        const std::vector<std::string> &args,
        const ExecuteToolkitCallback &cb)
{
    host->htmlLog("[ShotgunIntegration] ExecuteToolkitCommandAsync");
    VerifyArguments(pipelineConfigPath, command);
    boost::thread cmdThread(&ProcessManager::_ExecuteToolkitCommandAsync, this, pipelineConfigPath, command, args, cb);
}

void ProcessManager::_ExecuteToolkitCommandAsync(
        const std::string &pipelineConfigPath,
        const std::string &command,
        const std::vector<std::string> &args,
        const ExecuteToolkitCallback &cb)
{
    FB::VariantMap results = _ExecuteToolkitCommand(pipelineConfigPath, command, args);
    cb(results["retcode"].convert_cast<int>(), results["out"].convert_cast<std::string>(), results["err"].convert_cast<std::string>());
}