
# **Vectrex32 MAME** #

[![Build Status](https://travis-ci.org/mamedev/mame.svg)](https://travis-ci.org/mamedev/mame) [![Build status](https://ci.appveyor.com/api/projects/status/te0qy56b72tp5kmo?svg=true)](https://ci.appveyor.com/project/startaq/mame) [![Join the chat at https://gitter.im/mamedev/mame](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/mamedev/mame?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

What is Vectrex32 MAME?
=======================

Vectrex32 MAME is a version of MAME that uses the Vectrex32 to display games. It's based on [Trammell Hudson's modifications to MAME 0.168](https://trmm.net/MAME) for his vector graphics board. It is intended for playing vector graphics games only.

What is MAME?
=============

MAME stands for Multiple Arcade Machine Emulator.

MAME's purpose is to preserve decades of video-game history. As gaming technology continues to rush forward, MAME prevents these important "vintage" games from being lost and forgotten. This is achieved by documenting the hardware and how it functions. The source code to MAME serves as this documentation. The fact that the games are playable serves primarily to validate the accuracy of the documentation (how else can you prove that you have recreated the hardware faithfully?).


What is MESS?
=============

MESS (Multi Emulator Super System) is the sister project of MAME. MESS documents the hardware for a wide variety of (mostly vintage) computers, video game consoles, and calculators, as MAME does for arcade games.

The MESS and MAME projects live in the same source repository and share much of the same code, but are different build targets.


License
=======

MAME is in the process of becoming a Free and Open Source project. We are still in the process of contacting all developers who have contributed in the past. We have received approval for the vast majority of contributions.

Going forward, we will be using the 3-Clause BSD license for the core, and the LGPL version 2.1 or later, and the GPL version 2.0 or later, for certain drivers. As a whole, MAME will be delivered under the GPL version 2.0 or later.
As we are still contacting developers, MAME is still distributed under the [MAME license](docs/mamelicense.txt) as of this time. If you have not been contacted yet, and believe you have contributed code to MAME in the past, please [contact us](mailto:mamedev@mamedev.org).

How to compile?
===============

If you're on a *nix system, it is as easy as typing

```
make SUBTARGET=vector
```

for an vector-only build.

For Windows users, it's more complicated. First, download version 1.1 of the MSYS2 build tools, either the [32-bit version](https://github.com/mamedev/buildtools/releases/download/1.1/msys32-2016-01-02.exe) or the [64-bit version](https://github.com/mamedev/buildtools/releases/download/1.1/msys64-2016-01-02.exe). The reason for getting the obsolete version is that it's contemporaneous with MAME 0.168.

Run the downloaded installer file. It's intended to be extracted to C:\ (so it will create a directory C:\msys32 or C:\msys64). If you decide to put it elsewhere, after you unpack double-click **autorebase.bat**

To open a non-Posix shell there is the batch file **buildtools.bat** for regular windows console.

It's important to setup your git environment first

```
git config --global core.autocrlf true
```

And if you are contributor

```
git config --global user.email youremail@something.com
git config --global user.name "Firstname Lastname"
```

To download the Vectrex32 MAME source under your Msys2 user's homedir:

```
git clone https://github.com/galacticstudios/mame
```

And finally to build:

```
make SUBTARGET=vector
```

Building MAME using Microsoft Visual Studio compilers
-----------------------------------------------------

By default, MAME is configured via the makefile to build using the MinGW gcc compiler. Although this is a nice cross-platform solution, debugging binaries built this way leaves a lot to be desired.

Alternatively, you can configure MAME to build using Visual Studio. Once you have done that, you can debug problems in MAME using the Visual Studio debugger, which is a huge step up from gdb.

Since MAME uses modern C++ features, Visual Studio 2015 with Update 3 is required (Update 3 adds minimal support for variable templates used by MAME). If you don't have a paid license, Visual Studio Community 2015 is available from Microsoft for free (but requires sign-in with a Microsoft account).

Here's how to make it work:

1. You must already have an environment that can build MAME using the MinGW tools. Although you won't be using gcc to compile, you will be using several of the other tools included in the standard MAME MinGW Development Environment
2. Switch to the directory where the root MAME makefile lives.
3. Once you've done that, simply run: make vs2015 PYTHON_EXECUTABLE=c:/msys64/mingw64/bin/python.exe and wait for it to create projects
4. In order not to specify PYTHON_EXECUTABLE all the time, just place it in system variables
5. If you are building with XP compatibility enabled run make vs2015_xp
6. Browse to the folder containing solution file and open it with Visual Studio
7. Inside Visual Studio you can change to Debug/Release x86 or x64 builds
8. While doing development please note that you need to update LUA files in scripts folders in order to add new sources and regenerate projects
9. Note that all parameters for partial compilation works so you can do make SUBTARGET=tiny vs2015 or make vs2015 SUBTARGET=drivername SOURCES=src\mame\drivers\drivername.cpp

Making a run-time environment
=============================

The build produces a file called mamevector32.exe or mamevector64.exe. To run, it needs the file libwinpthread-1.dll in the PATH. You'll find that DLL in msys64\mingw64\bin (or the 32 bit equivalent). I recommend copying mamevector64.exe and libwinpthread-1.dll into a convenient directory. Then, in that directory, create a subdirectory called roms. This is where the MAME ROMs will go. Due to copyright laws, you're on your own for obtaining those ROMs.

Where can I find out more?
=============

* [Official MAME Development Team Site](http://mamedev.org/) (includes binary downloads for MAME and MESS, wiki, forums, and more)
* [Official MESS Wiki](http://www.mess.org/)
* [MAME Testers](http://mametesters.org/) (official bug tracker for MAME and MESS)


Contributing
=============

## Coding standard

MAME source code should be viewed and edited with your editor set to use four spaces per tab. Tabs are used for initial indentation of lines, with one tab used per indentation level. Spaces are used for other alignment within a line.

Some parts of the code follow [GNU style](http://www.gnu.org/prep/standards/html_node/Formatting.html); some parts of the code follow [K&R style](https://en.wikipedia.org/wiki/Indent_style#K.26R_style) -- mostly depending on who wrote the original version. **Above all else, be consistent with what you modify, and keep whitespace changes to a minimum when modifying existing source.** For new code, the majority tends to prefer GNU style, so if you don't care much, use that.

All contributors need to either add standard header for license info (on new files) or send us their wish under which of licenses they would like their code to be published under :[BSD-3-Clause](http://spdx.org/licenses/BSD-3-Clause), or for new files in mame/ or mess/, either the [BSD-3-Clause](http://spdx.org/licenses/BSD-3-Clause) license, the [LGPL-2.1+](http://spdx.org/licenses/LGPL-2.1+), or the [GPL-2.0+](http://spdx.org/licenses/GPL-2.0+).

