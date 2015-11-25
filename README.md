# The Shotgun Browser Plugin (Beta)

This is a browser plugin that is an extension for the [Shotgun production tracking system](http://www.shotgunsoftware.com/).
It implements the [local file integration](https://support.shotgunsoftware.com/entries/235640) and
[pipeline integration](https://tank.shotgunsoftware.com/entries/22126502) features
of Shotgun.

Note that the plugin is being replaced by functionality provided by Shotgun's Desktop application.  The newer implementation works for browsers that have removed support for NPAPI plugins like Chrome.  For more details [click here](https://support.shotgunsoftware.com/entries/95402178).

## Table of Contents

* [Background](#background)
* [Supported Browsers](#supported-browsers)
* [Supported Platforms](#supported-platforms)
* [Security](#security)
   * [Example - Configuring the browser plugin for a local Shotgun server](#example---configuring-the-browser-plugin-for-a-local-shotgun-server)
* [Installation](#installation)
   * [Installing on OSX](#installing-on-osx)
   * [Installing on Windows](#installing-on-windows)
   * [Installing on Linux](#installing-on-linux)
   * [Verifying Installation](#verifying-installation)
* [Launching via a Wrapper](#launching-via-a-wrapper)
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

This plugin is supported for all browsers the Shotgun supports.  The detailed list is [here](https://support.shotgunsoftware.com/entries/23533197),
but most versions of Safari, Chrome, and Firefox should work.

Note: Chrome does not support CentOS 6, and there are solutions for getting it working that use an alternate libc,
which will crash the plugin.  A working Chrome RPM can be found [here](http://people.centos.org/hughesjr/chromium/6/x86_64/RPMS).
See [issue #7](https://github.com/shotgunsoftware/browser-plugin/issues/7) for the gory details.

## Supported Platforms

This plugin is supported on OSX (10.6+), Windows (XP+), and Linux.

## Security

Without any customization, this plugin only exposes its functionality to web pages on hosted shotgun sites.
The methods it implements will not be available except for pages served from shotgunstudio.com over https.

If you run a local installation of shotgun, you can set two environment variables to override this default security:
* **SHOTGUN_PLUGIN_PROTOCOL_RESTRICTION** - The protocol portion of a url must match this.  Defaults to "https".
* **SHOTGUN_PLUGIN_DOMAIN_RESTRICTION** - The host portion of a url must match this.  Defaults to "*.shotgunstudio.com"

These environment variables can contain wildcards to match multiple values.
SHOTGUN_PLUGIN_DOMAIN_RESTRICTION can be a comma separated set of wildcarded domains to
match.

### Example - Configuring the browser plugin for a local Shotgun server

If you using the browser plugin with a hosted Shotgun installation (e.g. your Shotgun url ends with `shotgunstudio.com`), things 
should work out of the box using the browser plugin's default settings. However, if you are using the browser plugin
with a locally installed shotgun client, things will not work out of the box. You need to tell the browser plugin which 
urls you use for your internal site. 

Here's an example how to set things up when using a local Shotgun. Imagine your shotgun site can be reached
using the following addresses:

- `http://shotgun`
- `http://shotgun.mystudio.int`

We then need to set the following two environment variables:

```
SHOTGUN_PLUGIN_PROTOCOL_RESTRICTION = "http"
SHOTGUN_PLUGIN_DOMAIN_RESTRICTION = "shotgun,shotgun.mystudio.int"
```

Environment variables are set differently on different operating systems. On Linux, they are 
typically added to a login script. On windows, they can be modified using the control panel and
on Macosx they are typically added to a special configuration file (`/etc/launchd.conf`).




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

A pre-built binary is available [here](https://github.com/shotgunsoftware/browser-plugin/blob/binary/ShotgunIntegration.exe?raw=true).

Double clicking on the installer will bring up a standard installer window:

<img src="https://github.com/shotgunsoftware/browser-plugin/blob/binary/windows_installer.png?raw=true" width="40%" alt="installer"/>

If you manage an environment that does not support distribution via the installer, you can use regsvr32 and [this dll](https://github.com/shotgunsoftware/browser-plugin/blob/binary/npShotgunIntegration.dll?raw=true)
as an alternative.

### Installing on Linux

A pre-built binary is available [here](https://github.com/shotgunsoftware/browser-plugin/blob/binary/npShotgunIntegration.so?raw=true).  For it to run you need to have libgtk2 installed.

This binary was built on CentOS 6.3 and has fairly minimal library dependencies (libgtk2).  There is a good chance it will
work on any Linux system based on a 2.6 kernel.  To install it copy the binary to */usr/lib64/mozilla/plugins* or
*~/.mozilla/plugins*.

Your install location may differ depending on your linux flavor and system architecture.

Note that on CentOS 7, SELinux is configured to be more restrictive by default and blocks the plugin's access to the files system.  If you are running Gnome 3, you should see a notification about this when trying to use the plugin.  You will need to add an exception for the plugin to run correctly.

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

## Launching via a Wrapper

By default the browser will use the operating system default to launch a command (the equivalent of
xdg-open on linux, open on osx, and run on windows).  If you wish to provide your own wrapper for
launching files you can set the **SHOTGUN_PLUGIN_LAUNCHER** environment variable to point to your custom wrapper.
If that is set, then the wrapper will be called with the path as its only argument.

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

This software is released under the Apache License Version 2.0.  The full text is available
[here](https://github.com/shotgunsoftware/browser-plugin/blob/master/LICENSE.txt?raw=true).

## Support

You can report issues with the plugin [here](https://github.com/shotgunsoftware/browser-plugin/issues).

## Changelog

**v0.0.5 - 2014 May 19**

+ fixed: The plug-in no longer hangs when launching various Applications using Toolkit.  This problem was previously affecting 3ds Max & Hiero under certain conditions as well as custom wrapper scripts that output large amounts of debug during execution
+ fixed: Files containing non-ascii characters are now supported correctly when using local file linking.  Previously these could cause instability when linked/opened.

**v0.0.4 - 2013 July 30**

+ fixed: Revert to CreateProcess to avoid security messages when running Tank off of a mount not tagged as intranet

**v0.0.3 - 2013 July 10**

+ added: support for Pipeline Toolkit rename
+ fixed: use ShellExecuteEx on Windows to run commands in a proper batch environment
+ fixed: fixed a dangling reference to the host that was causing plugin crashes on tab switching

**v0.0.2 - 2013 May 22**

+ added: ability to use a custom launcher via SHOTGUN_PLUGIN_LAUNCHER
+ fixed: run Toolkit commands hidden on Windows
+ fixed: crashes if Toolkit command or directory is misconfigured.  issue [#5](https://github.com/shotgunsoftware/browser-plugin/issues/5).
+ added: ability to set SHOTGUN_PLUGIN_DOMAIN_RESTRICTION to a comma separated list of values

**v0.0.1 - 2013 Apr 19**

+ added: initial release


[![githalytics.com alpha](https://cruel-carlota.pagodabox.com/f9b8d249edd1f7c23fe82c98c5b10823 "githalytics.com")](http://githalytics.com/shotgunsoftware/browser-plugin)
