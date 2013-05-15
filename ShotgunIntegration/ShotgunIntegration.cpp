// Copyright (c) 2013 Shotgun Software Inc.

/**********************************************************\

  Auto-generated ShotgunIntegration.cpp

  This file contains the auto-generated main plugin object
  implementation for the Shotgun Integration project

\**********************************************************/
#include <boost/algorithm/string.hpp>

#include "URI.h"
#include "DOM/Window.h"

#include "ShotgunIntegrationAPI.h"
#include "ShotgunIntegration.h"
#include "WildcardMatch.h"

///////////////////////////////////////////////////////////////////////////////
/// @fn ShotgunIntegration::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginInitialize()
///
/// @see FB::FactoryBase::globalPluginInitialize
///////////////////////////////////////////////////////////////////////////////
void ShotgunIntegration::StaticInitialize()
{
    // Place one-time initialization stuff here; As of FireBreath 1.4 this should only
    // be called once per process
}

///////////////////////////////////////////////////////////////////////////////
/// @fn ShotgunIntegration::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginDeinitialize()
///
/// @see FB::FactoryBase::globalPluginDeinitialize
///////////////////////////////////////////////////////////////////////////////
void ShotgunIntegration::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here. As of FireBreath 1.4 this should
    // always be called just before the plugin library is unloaded
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  ShotgunIntegration constructor.  Note that your API is not available
///         at this point, nor the window.  For best results wait to use
///         the JSAPI object until the onPluginReady method is called
///////////////////////////////////////////////////////////////////////////////
ShotgunIntegration::ShotgunIntegration()
{
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  ShotgunIntegration destructor.
///////////////////////////////////////////////////////////////////////////////
ShotgunIntegration::~ShotgunIntegration()
{
    // This is optional, but if you reset m_api (the shared_ptr to your JSAPI
    // root object) and tell the host to free the retained JSAPI objects then
    // unless you are holding another shared_ptr reference to your JSAPI object
    // they will be released here.
    releaseRootJSAPI();
    m_host->freeRetainedObjects();
}

void ShotgunIntegration::onPluginReady()
{
    // When this is called, the BrowserHost is attached, the JSAPI object is
    // created, and we are ready to interact with the page and such.  The
    // PluginWindow may or may not have already fire the AttachedEvent at
    // this point.
}

void ShotgunIntegration::shutdown()
{
    // This will be called when it is time for the plugin to shut down;
    // any threads or anything else that may hold a shared_ptr to this
    // object should be released here so that this object can be safely
    // destroyed. This is the last point that shared_from_this and weak_ptr
    // references to this object will be valid
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  Creates an instance of the JSAPI object that provides your main
///         Javascript interface.
///
/// Note that m_host is your BrowserHost and shared_ptr returns a
/// FB::PluginCorePtr, which can be used to provide a
/// boost::weak_ptr<ShotgunIntegration> for your JSAPI class.
///
/// Be very careful where you hold a shared_ptr to your plugin class from,
/// as it could prevent your plugin class from getting destroyed properly.
///////////////////////////////////////////////////////////////////////////////
FB::JSAPIPtr ShotgunIntegration::createJSAPI()
{
    // m_host is the BrowserHost
    return boost::make_shared<ShotgunIntegrationAPI>(FB::ptr_cast<ShotgunIntegration>(shared_from_this()), m_host, getSecurityZone());
}

int ShotgunIntegration::getSecurityZone()
{
    char *env;
    std::string domain;
    std::string protocol;

    FB::DOM::WindowPtr window = m_host->getDOMWindow();
    FB::URI location(window->getLocation());

    env = getenv("SHOTGUN_PLUGIN_PROTOCOL_RESTRICTION");
    protocol = (env == NULL) ? "https" : std::string(env);

    env = getenv("SHOTGUN_PLUGIN_DOMAIN_RESTRICTION");
    domain = (env == NULL) ? "*.shotgunstudio.com" : std::string(env);

    if (location.protocol == "file") {
        m_host->htmlLog("[ShotgunIntegration] Local Security Scope");
        return FB::SecurityScope_Local;
    }

    // simple wildcard match for protocol
    bool protocolMatch = WildcardMatch(protocol, location.protocol);

    // split domain on commas
    bool domainMatch = false;
    std::vector<std::string> domains;
    boost::algorithm::split(domains, domain, boost::algorithm::is_any_of(","));
    for (std::vector<std::string>::iterator it = domains.begin(); it != domains.end(); ++it) {
        std::string entry = *it;
        boost::algorithm::trim(entry);
        domainMatch |= WildcardMatch(entry, location.domain);
        if (domainMatch)
            break;
    }

    if (protocolMatch && domainMatch) {
        m_host->htmlLog("[ShotgunIntegration] Protected Security Scope");
        return FB::SecurityScope_Protected;
    }

    m_host->htmlLog("[ShotgunIntegration] Public Security Scope");
    return FB::SecurityScope_Public;
}

bool ShotgunIntegration::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool ShotgunIntegration::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool ShotgunIntegration::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}
bool ShotgunIntegration::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *)
{
    // The window is attached; act appropriately
    return false;
}

bool ShotgunIntegration::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *)
{
    // The window is about to be detached; act appropriately
    return false;
}

