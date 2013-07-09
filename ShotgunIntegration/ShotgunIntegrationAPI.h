// Copyright (c) 2013 Shotgun Software Inc.

/**********************************************************\

  Auto-generated ShotgunIntegrationAPI.h

\**********************************************************/

#include <string>
#include <vector>
#include <sstream>
#include <boost/weak_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "ShotgunIntegration.h"

#ifndef H_ShotgunIntegrationAPI
#define H_ShotgunIntegrationAPI

#define ShotgunIntegrationAPI_MAJOR 0
#define ShotgunIntegrationAPI_MINOR 0
#define ShotgunIntegrationAPI_PATCH 2

class ShotgunIntegrationAPI : public FB::JSAPIAuto
{
public:
    ////////////////////////////////////////////////////////////////////////////
    /// @fn ShotgunIntegrationAPI::ShotgunIntegrationAPI(const ShotgunIntegrationPtr& plugin, const FB::BrowserHostPtr host)
    ///
    /// @brief  Constructor for your JSAPI object.
    ///         You should register your methods, properties, and events
    ///         that should be accessible to Javascript from here.
    ///
    /// @see FB::JSAPIAuto::registerMethod
    /// @see FB::JSAPIAuto::registerProperty
    /// @see FB::JSAPIAuto::registerEvent
    ////////////////////////////////////////////////////////////////////////////
    ShotgunIntegrationAPI(const ShotgunIntegrationPtr& plugin, const FB::BrowserHostPtr& host, const int securityZone) :
        JSAPIAuto(securityZone, "<JSAPI-Auto Secure Javascript Object>"), m_plugin(plugin), m_host(host)
    {
        // using this default all methods are denied unless the zone is SecurityScope_Local
        FB::scoped_zonelock _l(this, FB::SecurityScope_Local);

        // Register public members
        {
            FB::scoped_zonelock _l(this, FB::SecurityScope_Public);

            // Read-only property
            registerProperty("version", make_property(this, &ShotgunIntegrationAPI::get_version));
        }

        // Register protected members
        {
            FB::scoped_zonelock _l(this, FB::SecurityScope_Protected);

            registerMethod("open", make_method(this, &ShotgunIntegrationAPI::open));
            registerMethod("pickFileOrDirectory", make_method(this, &ShotgunIntegrationAPI::pickFileOrDirectory));
            registerMethod("pickFilesOrDirectories", make_method(this, &ShotgunIntegrationAPI::pickFilesOrDirectories));
            registerMethod("executeTankCommand", make_method(this, &ShotgunIntegrationAPI::executeToolkitCommand));
            registerMethod("executeToolkitCommand", make_method(this, &ShotgunIntegrationAPI::executeToolkitCommand));
            registerMethod("executeTankCommandAsync", make_method(this, &ShotgunIntegrationAPI::executeToolkitCommandAsync));
            registerMethod("executeToolkitCommandAsync", make_method(this, &ShotgunIntegrationAPI::executeToolkitCommandAsync));
        }

    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @fn ShotgunIntegrationAPI::~ShotgunIntegrationAPI()
    ///
    /// @brief  Destructor.  Remember that this object will not be released until
    ///         the browser is done with it; this will almost definitely be after
    ///         the plugin is released.
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~ShotgunIntegrationAPI() {};

    ShotgunIntegrationPtr getPlugin();

    // Read-only property ${PROPERTY.ident}
    FB::VariantMap get_version();

    // Local file linking
    void open(const std::string& path);
    
    void pickFileOrDirectory(FB::JSObjectPtr callback);
    
    void pickFilesOrDirectories(FB::JSObjectPtr callback);
    
    // Toolkit functionality
    FB::VariantMap executeToolkitCommand(
        const std::string &pipelineConfigPath,
        const std::string &command,
        const std::vector<std::string> &args);
    
    void executeToolkitCommandAsync(
            const std::string &pipelineConfigPath,
            const std::string &command,
            const std::vector<std::string> &args,
            FB::JSObjectPtr callback);

private:
    ShotgunIntegrationWeakPtr m_plugin;
    FB::BrowserHostPtr m_host;

    void fileSelectCallback(const FB::VariantList& paths, FB::JSObjectPtr callback);
    void executeToolkitCommandCallback(int retcode, const std::string& out, const std::string& err, FB::JSObjectPtr callback);
};

#endif // H_ShotgunIntegrationAPI

