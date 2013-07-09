// Copyright (c) 2013 Shotgun Software Inc.

/**********************************************************\

  Auto-generated ShotgunIntegrationAPI.cpp

\**********************************************************/
#include "boost/assign.hpp"

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"
#include "global/config.h"

#include "ShotgunIntegrationAPI.h"
#include "DialogManager.h"
#include "ProcessManager.h"

void ShotgunIntegrationAPI::open(const std::string& path)
{
    ProcessManager* prcMgr = ProcessManager::get();
    prcMgr->Open(m_host, path);
}

void ShotgunIntegrationAPI::pickFileOrDirectory(FB::JSObjectPtr callback)
{
    DialogManager* dlgMgr = DialogManager::get();
    ShotgunIntegrationPtr plugin = m_plugin.lock();
    dlgMgr->OpenFolderDialog(m_host, plugin->GetWindow(), false, boost::bind(&ShotgunIntegrationAPI::fileSelectCallback, this, _1, callback));
}

void ShotgunIntegrationAPI::pickFilesOrDirectories(FB::JSObjectPtr callback)
{
    DialogManager* dlgMgr = DialogManager::get();
    ShotgunIntegrationPtr plugin = m_plugin.lock();
    dlgMgr->OpenFolderDialog(m_host, plugin->GetWindow(), true, boost::bind(&ShotgunIntegrationAPI::fileSelectCallback, this, _1, callback));
}

FB::VariantMap ShotgunIntegrationAPI::executeToolkitCommand(
    const std::string &pipelineConfigPath, const std::string &command, const std::vector<std::string> &args)
{
    ProcessManager* prcMgr = ProcessManager::get();
    return prcMgr->ExecuteToolkitCommand(m_host, pipelineConfigPath, command, args);
}

void ShotgunIntegrationAPI::executeToolkitCommandAsync(
    const std::string &pipelineConfigPath, const std::string &command, const std::vector<std::string> &args, FB::JSObjectPtr callback)
{
    ProcessManager* prcMgr = ProcessManager::get();
    prcMgr->ExecuteToolkitCommandAsync(
                m_host, pipelineConfigPath, command, args,
                boost::bind(&ShotgunIntegrationAPI::executeToolkitCommandCallback, this, _1, _2, _3, callback));
}

void ShotgunIntegrationAPI::fileSelectCallback(const FB::VariantList &paths, FB::JSObjectPtr callback)
{
    callback->Invoke("", FB::variant_list_of(paths));
}

void ShotgunIntegrationAPI::executeToolkitCommandCallback(int retcode, const std::string& out, const std::string& err, FB::JSObjectPtr callback)
{
    callback->Invoke("", FB::variant_list_of(retcode)(out)(err));
}

///////////////////////////////////////////////////////////////////////////////
/// @fn ShotgunIntegrationPtr ShotgunIntegrationAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
ShotgunIntegrationPtr ShotgunIntegrationAPI::getPlugin()
{
    ShotgunIntegrationPtr plugin(m_plugin.lock());
    if (!plugin) {
        throw FB::script_error("The plugin is invalid");
    }
    return plugin;
}

// Read-only property version
FB::VariantMap ShotgunIntegrationAPI::get_version()
{
    return FB::variant_map_of<std::string>
        ("major", ShotgunIntegrationAPI_MAJOR)
        ("minor", ShotgunIntegrationAPI_MINOR)
        ("patch", ShotgunIntegrationAPI_PATCH);
}
