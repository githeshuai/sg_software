// Copyright (c) 2013 Shotgun Software Inc.

#include <boost/assign.hpp>
#include <boost/process.hpp>
#include <boost/filesystem.hpp>

#include "ProcessManager.h"

namespace ba = ::boost::asio;
namespace bp = ::boost::process;
namespace fs = ::boost::filesystem;

#if defined(BOOST_POSIX_API)
#  define TANK_SCRIPT_NAME "tank"
#elif defined(BOOST_WINDOWS_API)
#  define TANK_SCRIPT_NAME "tank.bat"
#else
#  error "Unsupported platform."
#endif

void ProcessManager::ExecuteTankCommand(
    const FB::BrowserHostPtr& host,
    const std::string &pipelineConfigPath,
    const std::string &command,
    const std::vector<std::string> &args,
    const ExecuteTankCallback &cb)
{
    host->htmlLog("[ShotgunIntegration] ExecuteTankCommand");
    
    if (!boost::starts_with(command, "shotgun"))
        throw FB::script_error("ExecuteTankCommand error");

    fs::path exec = pipelineConfigPath;
    if (!fs::is_directory(exec))
        throw FB::script_error("pipelineConfigPath not found: " + exec.string());
    
    exec /= TANK_SCRIPT_NAME;
    if (!fs::is_regular_file(exec))
        throw FB::script_error("pipelineConfigPath not found");

    std::vector<std::string> arguments = boost::assign::list_of(exec.string())(command);
    arguments.insert(arguments.end(), args.begin(), args.end());

    bp::context ctx;
    ctx.environment = bp::self::get_environment();
    ctx.stdout_behavior = bp::capture_stream();
    ctx.stderr_behavior = bp::capture_stream();

    bp::child child = bp::launch(exec, arguments, ctx);
    bp::posix_status status = child.wait();
    
    int retcode;
    if (status.exited())
        retcode = status.exit_status();
    else if (status.signaled())
        retcode = -status.term_signal();
    else if (status.stopped())
        retcode = -status.stop_signal();
    
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

    cb(retcode, ossStdout.str(), ossStderr.str());
}