# Raylib-Quickstart
A simple cross platform template for setting up a project with the bleeding edge raylib code.
Works with C or C++.

## Table of contents

- [Raylib-Quickstart](#raylib-quickstart)
- [Table of Contents](#table-of-contents)
- [Supported Platforms](#supported-platforms)
- [Naming projects](#naming-projects)
- [VSCode Users (all platforms)](#vscode-users-all-platforms)
- [Windows Users](#windows-users)
	- [MinGW-W64](#using-mingw-w64)
		- [If you have installed raylib from the installer](#if-you-have-installed-raylib-from-the-installer)
		- [Note on MinGW-64 versions](#note-on-migw-64-versions)
	- [Microsoft Visual Studio](#microsoft-visual-studio)
- [Linux Users](#linux-users)
- [MacOS Users](#macos-users)
- [Output files](#output-files)
- [Working directories and the resources folder](#working-directories-and-the-resources-folder)
- [Changing to C++](#changing-to-c)
- [Using your own code](#using-your-own-code)
- [Building for other OpenGL targets](#building-for-other-opengl-targets)
	- [For OpenGL 1.1](#for-opengl-11)
	- [For OpenGL 2.1](#for-opengl-21)
	- [For OpenGL 4.3](#for-opengl-43)
	- [For OpenGLES 2.0](#for-opengles-20)
	- [For OpenGLES 3.0](#for-opengles-30)
- [License](#license)


# Supported Platforms
Quickstart supports the main 3 desktop platforms:
* Windows
* Linux
* MacOS

# Naming projects
Do <ins>***NOT***</ins> name your game project `raylib`, it will conflict with the raylib library.

# VSCode Users (all platforms)
*Note* You must have a compiler toolchain installed in addition to vscode.

* Download the quickstart
* Rename the folder to your game name
* Open the folder in VSCode
* Run the build task ( CTRL+SHIFT+B or F5 )
* You are good to go

# Windows Users
There are two compiler toolchains available for windows, MinGW-W64 (a free compiler using GCC), and Microsoft Visual Studio
## Using MinGW-W64

* Double click the `build-MinGW-W64.bat` file
* You are good to go

### If you have installed raylib from the installer
Make sure you have added the path  
`C:\raylib\w64devkit\bin`  
to your PATH environment variable so that the compiler that came with raylib can be found.  

To make sure you've done it correctly, run `where sh` in `cmd.exe`.  
You should see something like `C:\raylib\w64devkit\bin\sh.exe`.  

DO NOT INSTALL ANOTHER MinGW-W64 from another source such as msys2, you don't need it.

### Note on MinGW-64 versions
Please make sure you have a recent version of MinGW-W64.  
The older versions from mingw.org <ins>***will not work***</ins>.  
We recommend the W64Devkit. It has everything needed to build raylib.  
It can be downloaded from here https://github.com/skeeto/w64devkit/releases


## Microsoft Visual Studio
* Run `build-VisualStudio2022.bat`
* double click the `.sln` file that is generated
* develop your game
* you are good to go

# Linux Users
* CD into the build folder
* run `./premake5 gmake`
* CD back to the root
* run `make`
* you are good to go

# MacOS Users
* CD into the build folder
* run `./premake5.osx gmake`
* CD back to the root
* run `make`
* you are good to go

# Output files
The built code will be in the bin dir

# Working directories and the resources folder
The example uses a utility function from `path_utils.h` that will find the resources dir and set it as the current working directory. This is very useful when starting out. If you wish to manage your own working directory you can simply remove the call to the function and the header.

# Changing to C++
Simply rename `src/main.c` to `src/main.cpp` and re-run the steps above and do a clean build.

# Using your own code
Simply remove `src/main.c` and replace it with your code, and re-run the steps above and do a clean build.

# Building for other OpenGL targets
If you need to build for a different OpenGL version than the default (OpenGL 3.3) you can specify an OpenGL version in your premake command line. Just modify the bat file or add the following to your command line

## For OpenGL 1.1
`--graphics=opengl11`

## For OpenGL 2.1
`--graphics=opengl21`

## For OpenGL 4.3
`--graphics=opengl43`

## For OpenGLES 2.0
`--graphics=opengles2`

## For OpenGLES 3.0
`--graphics=opengles3`

# License
Copyright (c) 2020-2025 Jeffery Myers

This software is provided "as-is", without any express or implied warranty. In no event 
will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial 
applications, and to alter it and redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not claim that you 
  wrote the original software. If you use this software in a product, an acknowledgment 
  in the product documentation would be appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be misrepresented
  as being the original software.

  3. This notice may not be removed or altered from any source distribution.
