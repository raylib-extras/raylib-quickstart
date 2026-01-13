# Raylib-Quickstart
A simple cross platform template for setting up a project with the bleeding edge raylib code.
Works with C or C++.

# Basic Setup
Download this repository to get started.

You can download the zip file of the repository from the Green Code button on github. This is the simplest way to get the template to start from.
Once you have downloaded the template, rename it to your project name.

or

Clone the repository with git, from the url
```
https://github.com/raylib-extras/raylib-quickstart.git
```

If you are using a command line git client you can use the command below to download and rename the template in one step
```
git clone https://github.com/raylib-extras/raylib-quickstart.git [name-for-your-project-here]
```

# Naming projects
* Replace the placeholder with your desired project name when running the git clone command above.
* __Do not name your game project 'raylib', it will conflict with the raylib library.__
* If you have used custom game name with __git clone__, there is no need to rename it again.


## Supported Platforms
Quickstart supports the main 3 desktop platforms:
* Windows
* Linux
* MacOS

# VSCode Users (all platforms)
*Note* You must have a compiler toolchain installed in addition to vscode.

1. Download the quickstart
2. Rename the folder to your game name
3. Open the folder in VSCode
4. Run the build task ( CTRL+SHIFT+B or F5 )
5. You are good to go

# Windows Users
There are two compiler toolchains available for windows, MinGW-W64 (a free compiler using GCC), and Microsoft Visual Studio
## Using MinGW-W64
* Rename the folder to your game name
* Double click the `build-MinGW-W64.bat` file
* CD into the folder in your terminal
  * if you are using the W64devkit and have not added it to your system path environment variable, you must use the W64devkit.exe terminal, not CMD.exe
  * If you want to use cmd.exe or any other terminal, please make sure that gcc/mingw-W64 is in your path environment variable.
* run `make`
* You are good to go

### Note on MinGW-64 versions
Make sure you have a modern version of MinGW-W64 (not mingw).
The best place to get it is from the W64devkit from
https://github.com/skeeto/w64devkit/releases

or the version installed with the raylib installer

#### If you have installed raylib from the installer
Make sure you have added the path

`C:\raylib\w64devkit\bin`

To your path environment variable so that the compiler that came with raylib can be found.

DO NOT INSTALL ANOTHER MinGW-W64 from another source such as msys2, you don't need it.

## Microsoft Visual Studio
* Rename the folder to your game name
* Run `build-VisualStudio2022.bat`
* double click the `.sln` file that is generated
* develop your game
* you are good to go

# Linux Users
* Rename the folder to your game name
* CD into the build folder
* run `./premake5 gmake`
* CD back to the root
* run `make`
* you are good to go

# MacOS Users
* Rename the folder to your game name
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

## For Software Rendering
`--graphics=software`

*Note*
Sofware rendering does not work with glfw, use Win32 or SDL platforms
`--backend=win32`

# Adding External Libraries 

Quickstart is intentionally minimal — it only includes what is required to compile and run a basic raylib project.  
If you want to use extra libraries, you can add them to the `build/premake5.lua` file yourself using the links function.

You can find the documentation for the links function here https://premake.github.io/docs/links/

### Example: adding the required libraries for tinyfiledialogs on Windows
tinyfiledialogs requires extra Windows system libraries.
The premake file uses filters to define options that are platform specific
https://premake.github.io/docs/Filters/

Using the windows filter adds these libraries only to the windows build.
```
filter "system:windows"
    links {
        "Comdlg32",
        "User32",
        "Ole32",
        "Shell32"
    }
```

### Cross-platform reminder
If you add a library, make sure to add its required dependencies for all platforms you plan to support (Windows, Linux, MacOS).
Different libraries will have different dependencies on different platforms.


# License
Raylib-Quickstart by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

