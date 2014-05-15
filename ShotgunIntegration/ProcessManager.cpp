// Copyright (c) 2013 Shotgun Software Inc.

#include <boost/assign.hpp>
#include <boost/filesystem.hpp>

#include "ProcessManager.h"

namespace fs = ::boost::filesystem;

/*
 * Construction
 */
ProcessManager::ProcessManager()
{}

/*
 * Destruction
 */
ProcessManager::~ProcessManager()
{}

/*
 * Return the name of the toolkit script
 */
const char * ProcessManager::GetToolkitScriptName()
{
    return "shotgun";
}

/*
 * Return the legacy name of the toolkit script as the
 * fall-back option
 */
const char * ProcessManager::GetToolkitFallbackScriptName()
{
    return "tank";
}

/*
 * Verify that the arguments passed in to the plug-in are valid and can
 * be run
*/
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
    
        fs::path exec = pcPath / GetToolkitScriptName();

        if (!fs::is_regular_file(exec))
            exec = pcPath / GetToolkitFallbackScriptName();
        
        if (!fs::is_regular_file(exec))
            throw FB::script_error("Could not find the Toolkit command on disk: " + exec.string());

    } catch (fs::filesystem_error &e) {
        std::string msg = std::string("Error finding the Toolkit command on disk: ") + e.what();
        throw FB::script_error(msg);
    }
}

/*
 * Execute a toolkit command
 */
FB::VariantMap ProcessManager::ExecuteToolkitCommand(
    const FB::BrowserHostPtr& host,
    const std::string &pipelineConfigPath,
    const std::string &command,
    const std::vector<std::string> &args)
{
    return _ExecuteToolkitCommand(pipelineConfigPath, command, args);
}

/*
 * Launch the script/executable with the specified arguments and return the result
 */
bp::child ProcessManager::Launch(const std::string &exec, const std::vector<std::string> &arguments)
{
    bp::context ctx;

    ctx.environment = bp::self::get_environment();
    ctx.stdout_behavior = bp::capture_stream();
    ctx.stderr_behavior = bp::capture_stream();

    return bp::launch(exec, arguments, ctx);
}

/*
 * Helper class to retrieve std output/error from a child process.
 */
class OutputHandler
{
public:
	/*
	 * Construction
	 */
    OutputHandler(bp::pistream &rIStream)
        : m_rIStream(rIStream)
    {}
    /*
     * Return the output result
     */
    inline const std::string &GetOutput() const
    {
        return m_output;
    }
    /*
     * This will get run as the thread procedure
     * when a boost::thread is constructed with
     * an instance of this class
     */
    void operator()()
    {
        // read from the stream until it's closed:
        std::string line;
        std::ostringstream oss;
        while (std::getline(m_rIStream, line))
        {
            oss << line << std::endl;
        }
        m_output = oss.str();
    }

private:
    bp::pistream &  m_rIStream;
    std::string     m_output;
};

/*
 * Actually execute the command
 */
FB::VariantMap ProcessManager::_ExecuteToolkitCommand(
    const std::string &pipelineConfigPath,
    const std::string &command,
    const std::vector<std::string> &args)
{    
    try {
        VerifyArguments(pipelineConfigPath, command);
    
        fs::path pcPath = pipelineConfigPath;
        fs::path exec = pcPath / GetToolkitScriptName();

        if (!fs::is_regular_file(exec))
            exec = pcPath / GetToolkitFallbackScriptName();

        std::vector<std::string> arguments = boost::assign::list_of(exec.string())(command);
        arguments.insert(arguments.end(), args.begin(), args.end());

        bp::context ctx;
        ctx.environment = bp::self::get_environment();
        ctx.stdout_behavior = bp::capture_stream();
        ctx.stderr_behavior = bp::capture_stream();

        bp::child child = Launch(exec.string(), arguments);

        // Read std output and error streams from child process.  Each stream is
        // read in a separate thread to avoid them potentially blocking each other.
        OutputHandler stdoutHandler(child.get_stdout());
        OutputHandler stderrHandler(child.get_stderr());

        // Note: the boost::thread_group destructor will delete all thread
        // instances in the group which is why they are constructed on the heap
        boost::thread_group threadGroup;
        threadGroup.add_thread(new boost::thread(boost::ref(stdoutHandler)));
        threadGroup.add_thread(new boost::thread(boost::ref(stderrHandler)));

        // Wait for the child process to complete - keep trying through interrupts
        int retcode;
        while (true) {
            try {
                bp::status status = child.wait();
                if (status.exited())
                    retcode = status.exit_status();
                else
                    retcode = -1;
                break;
            } catch (boost::system::system_error &e) {
                if (e.code().value() != EINTR)
                    throw;
            }
        }

        // wait for OutputHandler threads to complete (all output has been read)
        threadGroup.join_all();
        
        return FB::variant_map_of<std::string>
            ("retcode", retcode)
            ("out", stdoutHandler.GetOutput())
            ("err", stderrHandler.GetOutput());
    } catch (std::exception &e) {
        // May be running in a non-main thread.  Avoid propagating exception
        return FB::variant_map_of<std::string>
            ("retcode", -1)
            ("out", std::string(""))
            ("err", std::string(e.what()));
    } catch (...) {
        return FB::variant_map_of<std::string>
            ("retcode", -1)
            ("out", std::string(""))
            ("err", std::string("unknown error executing toolkit command"));
    }
}

/*
 * Execute the toolkit command asynchronously
 */
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

/*
 * Actually execute the toolkit command asynchronously
 */
void ProcessManager::_ExecuteToolkitCommandAsync(
        const std::string &pipelineConfigPath,
        const std::string &command,
        const std::vector<std::string> &args,
        const ExecuteToolkitCallback &cb)
{
    FB::VariantMap results = _ExecuteToolkitCommand(pipelineConfigPath, command, args);
    cb(results["retcode"].convert_cast<int>(), results["out"].convert_cast<std::string>(), results["err"].convert_cast<std::string>());
}
