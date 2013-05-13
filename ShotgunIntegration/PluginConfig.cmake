#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for Shotgun Integration
#
#\**********************************************************/

set(PLUGIN_NAME "ShotgunIntegration")
set(PLUGIN_PREFIX "SGI")
set(COMPANY_NAME "ShotgunSoftware")

# ActiveX constants:
set(FBTYPELIB_NAME ShotgunIntegrationLib)
set(FBTYPELIB_DESC "ShotgunIntegration 1.0 Type Library")
set(IFBControl_DESC "ShotgunIntegration Control Interface")
set(FBControl_DESC "ShotgunIntegration Control Class")
set(IFBComJavascriptObject_DESC "ShotgunIntegration IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "ShotgunIntegration ComJavascriptObject Class")
set(IFBComEventSource_DESC "ShotgunIntegration IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID 0ad3f363-697e-5073-9a57-0af344e6d48a)
set(IFBControl_GUID 1d6042e4-fe36-550e-9fd0-84ec8b632c0a)
set(FBControl_GUID 8fb3b918-9c3f-5d6d-9583-db2ced84e135)
set(IFBComJavascriptObject_GUID a07f636b-0729-53e0-a5d8-315ffba40127)
set(FBComJavascriptObject_GUID 5976f89d-c4cb-5880-85da-e9c6bbd314f5)
set(IFBComEventSource_GUID ab3185bc-1ef4-54f6-9b37-b459298d549b)
if ( FB_PLATFORM_ARCH_32 )
    set(FBControl_WixUpgradeCode_GUID dfbd6a52-0623-505e-901d-11af1e7dc428)
else ( FB_PLATFORM_ARCH_32 )
    set(FBControl_WixUpgradeCode_GUID fe6b3b74-55e9-5ba8-9a96-e33129ac53f4)
endif ( FB_PLATFORM_ARCH_32 )

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "ShotgunSoftware.ShotgunIntegration")
set(MOZILLA_PLUGINID "shotgunsoftware.com/ShotgunIntegration")

# strings
set(FBSTRING_CompanyName "Shotgun Software Inc.")
set(FBSTRING_PluginDescription "Shotgun Integration")
set(FBSTRING_PLUGIN_VERSION "0.0.2")
set(FBSTRING_LegalCopyright "Copyright 2013 Shotgun Software Inc.")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "Shotgun Integration")
set(FBSTRING_FileExtents "")
if ( FB_PLATFORM_ARCH_32 )
    set(FBSTRING_PluginName "Shotgun Integration")  # No 32bit postfix to maintain backward compatability.
else ( FB_PLATFORM_ARCH_32 )
    set(FBSTRING_PluginName "Shotgun Integration_${FB_PLATFORM_ARCH_NAME}")
endif ( FB_PLATFORM_ARCH_32 )
set(FBSTRING_MIMEType "application/x-shotgunintegration")

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

#set (FB_GUI_DISABLED 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON 1)
set(FBMAC_USE_COCOA 1)
set(FBMAC_USE_COREGRAPHICS 1)
set(FBMAC_USE_COREANIMATION 0)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 0)

# If you want to register per-machine on Windows, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)

add_boost_library(filesystem)
