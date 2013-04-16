# The Shotgun Browser Plugin

This is a browser plugin that is an extension for the [Shotgun production tracking system](http://www.shotgunsoftware.com/).
It implements the [local file integration](https://support.shotgunsoftware.com/entries/235640-Linking-to-Local-Files) and
[pipeline integration](https://tank.shotgunsoftware.com/entries/22126502-What-is-Tank-and-how-do-I-Install-it-) features
of Shotgun.

## Table of Contents

* [Background](#background)
* [Supported Browsers](#supported-browsers)
* [Supported Platforms](#supported-platforms)
* [Security](#security)
* [Installation](#installation)
   * [Installing on OSX](#installing-on-osx)
   * [Installing on Windows](#installing-on-windows)
   * [Installing on Linux](#installing-on-linux)
   * [Verifying Installation](#verifying-installation)
* [Building](#building)
   * [Building on OSX](#building-on-osx)
   * [Building on Windows](#building-on-windows)
   * [Building on Linux](#building-on-linux)
* [License](#license)
* [Support](#support)
* [Changelog](#changelog)

## Background

This plugin replaces Shotgun's previous implementations of local file linking and pipeline integration.
The previous implementation was based on a Java applet, and due to the various security issues surrounding
Java we wrote this native plugin.  It uses the
[firebreath](http://www.firebreath.org/display/documentation/FireBreath+Home) library as a foundation
and has been written with security in mind.

## Supported Browsers

This plugin is supported for all browsers the Shotgun supports.  The detailed list is [here](https://support.shotgunsoftware.com/entries/52237-Supported-Browsers),
but most versions of Safari, Chrome, and Firefox should work.

## Supported Platforms

This plugin is supported on OSX (10.6+), Windows (XP+), and Linux.

## Security

Without any customization, this plugin only exposes its functionality to web pages on hosted shotgun sites.
The methods it implements will not be available except for pages served from shotgunstudio.com over https.

If you run a local installation of shotgun, you can set two environment variables to override this default security:
* **SHOTGUN_PLUGIN_PROTOCOL_RESTRICTION** - The protocol portion of a url must match this.  Defaults to "https".
* **SHOTGUN_PLUGIN_DOMAIN_RESTRICTION** - The host portion of a url must match this.  Defaults to "*.shotgunstudio.com"

## Installation

### Installing on OSX

A pre-built binary is available [here](https://github.com/shotgunsoftware/browser-plugin/blob/binary/ShotgunIntegration.dmg?raw=true).

Double clicking on the dmg will bring up this window:

<img src="https://github.com/shotgunsoftware/browser-plugin/blob/binary/dmg.png?raw=true" width="40%" alt="folder"/>

Simply drag the plugin into the linked folder and restart your browser to enable the plugin.
The folder is */Library/Internet Plug-Ins/*, which will install the plugin for all users.  If you do not
have administrative privledges or want to install the plugin only for yourself, you can copy the plugin
to *~/Library/Internet Plug-Ins/* instead.

### Installing on Windows

A pre-built binary is available [here](https://github.com/shotgunsoftware/browser-plugin/blob/binary/ShotgunIntegration.msi?raw=true).

Double clicking on the installer will bring up a standard installer window, which will
install without any further prompting:

<img src="https://github.com/shotgunsoftware/browser-plugin/blob/binary/windows_installer.png?raw=true" width="40%" alt="installer"/>

### Installing on Linux

A pre-built binary is available [here](https://github.com/shotgunsoftware/browser-plugin/blob/binary/npShotgunIntegration.so?raw=true).

This binary was built on CentOS 6.3 and has fairly minimal library dependencies.  There is a good chance it will
work on any Linux system based on a 2.6 kernel.  To install it copy the binary to */usr/lib64/mozilla/plugins* or
*~/.mozilla/plugins*.

Your install location may differ depending on your linux flavor and system architecture.

### Verifying Installation

Once the plugin is installed and you have restarted your browser, you should see the plugin listed in the browser
plugin list.  For example, in Chrome you will see a plugin named "Shotgun Integration":

<img src="https://github.com/shotgunsoftware/browser-plugin/blob/binary/chrome_plugins.png?raw=true" width="40%" alt="installer"/>

In addition, in the javascript console when visiting a Shotgun page where local file linking or pipeline
integration is turned on, you should see a message like the following:

<img src="https://github.com/shotgunsoftware/browser-plugin/blob/binary/security_scope.png?raw=true" width="40%" alt="installer"/>

If you do not see that message, then the plugin is not working.

If you see the message, but instead of reading *"Protected Security Scope"* it reads *"Public Security Scope"*,
then you need to update your environment to activate the plugin.  See the section on [security](#security) above
for instructions on how to do this.

## Building

The [firebreath build instructions](http://www.firebreath.org/display/documentation/Building+FireBreath+Plugins)
are very thorough.  You should be able to build this plugin by following those instructions.

### Building on OSX

This plugin has been built on OSX 10.6, 10.7, and 10.8.  Firebreath's DMG creation functionality is currently
experimental, but functions well.  The one issue with it is that rebuilding the source will raise an error
at the DMG generation if the previous DMG has not been deleted or moved aside.

### Building on Windows

This plugin has been built on Windows XP and Windows 7 using Visual Studio Express 2012.  The extra functionality
for creating an MSI installer has been turned on, which requires WiX to be installed.  The firebreath instructions
on how to build for Windows has detailed instructions on how to install these dependencies.

### Building on Linux

Building on Linux if fairly simple.  There is a bug with firebreath where the prepmake.sh script which generates
the Makefiles needs to be run twice to correctly generate a build directory.  The only package dependency for
building should be on libgtk2.

## License

This software is released un the Apache License Version 2.0.  The full text is available
[here](https://github.com/shotgunsoftware/browser-plugin/blob/master/LICENSE.txt?raw=true).

## Support

Email <support@shotgunsoftware.com> with any questions or issues.

## Changelog

**v0.0.1 - 2013 Apr 19**

+ added: initial release
