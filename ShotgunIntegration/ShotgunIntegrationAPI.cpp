// Copyright (c) 2013 Shotgun Software Inc.

/**********************************************************\

  Auto-generated ShotgunIntegrationAPI.cpp

\**********************************************************/

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

void ShotgunIntegrationAPI::fileSelectCallback(const FB::VariantList &paths, FB::JSObjectPtr callback)
{
    callback->Invoke("", FB::variant_list_of(paths));
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
std::string ShotgunIntegrationAPI::get_version()
{
    return FBSTRING_PLUGIN_VERSION;
}
