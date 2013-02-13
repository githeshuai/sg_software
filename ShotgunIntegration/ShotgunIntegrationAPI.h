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
    ShotgunIntegrationAPI(const ShotgunIntegrationPtr& plugin, const FB::BrowserHostPtr& host) :
        m_plugin(plugin), m_host(host)
    {
        registerMethod("open", make_method(this, &ShotgunIntegrationAPI::open));
        registerMethod("pickFileOrDirectory", make_method(this, &ShotgunIntegrationAPI::pickFileOrDirectory));
        registerMethod("pickFilesOrDirectories", make_method(this, &ShotgunIntegrationAPI::pickFilesOrDirectories));
        
        // Read-only property
        registerProperty("version", make_property(this, &ShotgunIntegrationAPI::get_version));
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
    std::string get_version();
    
    void open(const std::string& path);
    void pickFileOrDirectory(FB::JSObjectPtr callback);
    void pickFilesOrDirectories(FB::JSObjectPtr callback);
    
private:
    ShotgunIntegrationWeakPtr m_plugin;
    FB::BrowserHostPtr m_host;
    
    void fileSelectCallback(const FB::VariantList& paths, FB::JSObjectPtr callback);
};

#endif // H_ShotgunIntegrationAPI

