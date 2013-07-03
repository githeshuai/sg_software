// Copyright (c) 2013 Shotgun Software Inc.

#include <boost/assign.hpp>
#include <boost/filesystem.hpp>

#include "ProcessManager.h"

namespace fs = ::boost::filesystem;

#define TANK_SCRIPT_NAME "shotgun"
#define TANK_FALLBACK_SCRIPT_NAME "tank"

void ProcessManager::VerifyArguments(const std::string &pipelineConfigPath, const std::string &command)
{
    try {
        if (!boost::starts_with(command, "shotgun"))
            throw FB::script_error("ExecuteTankCommand error");
    
        fs::path pcPath = pipelineConfigPath;
        if (!fs::is_directory(pcPath)) {
            std::string err = "Could not find the Tank Configuration on disk: " + pcPath.string();
            throw FB::script_error(err);
        }
    
        fs::path exec = pcPath / TANK_SCRIPT_NAME;

        if (!fs::is_regular_file(exec))
            exec = pcPath / TANK_FALLBACK_SCRIPT_NAME;
        
        if (!fs::is_regular_file(exec))
            throw FB::script_error("Could not find the Tank command on disk: " + exec.string());
    } catch (fs::filesystem_error &e) {
        std::string msg = std::string("Error finding the Tank command on disk: ") + e.what();
        throw FB::script_error(msg);
    }
}

FB::VariantMap ProcessManager::ExecuteTankCommand(
    const FB::BrowserHostPtr& host,
    const std::string &pipelineConfigPath,
    const std::string &command,
    const std::vector<std::string> &args)
{
    return _ExecuteTankCommand(pipelineConfigPath, command, args);
}

FB::VariantMap ProcessManager::_ExecuteTankCommand(
    const std::string &pipelineConfigPath,
    const std::string &command,
    const std::vector<std::string> &args)
{    
    try {
        VerifyArguments(pipelineConfigPath, command);
    
        fs::path pcPath = pipelineConfigPath;
        fs::path exec = pcPath / TANK_SCRIPT_NAME;

        if (!fs::is_regular_file(exec))
            exec = pcPath / TANK_FALLBACK_SCRIPT_NAME;

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

void ProcessManager::ExecuteTankCommandAsync(
        const FB::BrowserHostPtr& host,
        const std::string &pipelineConfigPath,
        const std::string &command,
        const std::vector<std::string> &args,
        const ExecuteTankCallback &cb)
{
    host->htmlLog("[ShotgunIntegration] ExecuteTankCommandAsync");
    VerifyArguments(pipelineConfigPath, command);
    boost::thread cmdThread(&ProcessManager::_ExecuteTankCommandAsync, this, pipelineConfigPath, command, args, cb);
}

void ProcessManager::_ExecuteTankCommandAsync(
        const std::string &pipelineConfigPath,
        const std::string &command,
        const std::vector<std::string> &args,
        const ExecuteTankCallback &cb)
{
    FB::VariantMap results = _ExecuteTankCommand(pipelineConfigPath, command, args);
    cb(results["retcode"].convert_cast<int>(), results["out"].convert_cast<std::string>(), results["err"].convert_cast<std::string>());
}