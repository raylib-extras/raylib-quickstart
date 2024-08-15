/**********************************************************************************************
*
*   raylibExtras * Utilities and Shared Components for Raylib
*
*   Resource Dir * function to help find resource dir in common locations
*
*   LICENSE: MIT
*
*   Copyright (c) 2022 Jeffery Myers
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
*
**********************************************************************************************/

#pragma once

#include "raylib.h"

#if defined(__cplusplus)
extern "C" {            // Prevents name mangling of functions
#endif
    /// <summary>
    /// Looks for the specified resource dir in several common locations
    /// The working dir
    /// The app dir
    /// Up to 3 levels above the app dir
    /// When found the dir will be set as the working dir so that assets can be loaded relative to that.
    /// </summary>
    /// <param name="folderName">The name of the resources dir to look for</param>
    /// <returns>True if a dir with the name was found, false if no change was made to the working dir</returns>
    inline static bool SearchAndSetResourceDir(const char* folderName)
    {
        // check the working dir
        if (DirectoryExists(folderName))
        {
            ChangeDirectory(TextFormat("%s/%s", GetWorkingDirectory(), folderName));
            return true;
        }

        const char* appDir = GetApplicationDirectory();
       
        // check the applicationDir
        const char* dir = TextFormat("%s%s", appDir, folderName);
        if (DirectoryExists(dir))
        {
            ChangeDirectory(dir);
            return true;
        }

        // check one up from the app dir
        dir = TextFormat("%s../%s", appDir, folderName);
        if (DirectoryExists(dir))
        {
            ChangeDirectory(dir);
            return true;
        }

        // check two up from the app dir
        dir = TextFormat("%s../../%s", appDir, folderName);
        if (DirectoryExists(dir))
        {
            ChangeDirectory(dir);
            return true;
        }

        // check three up from the app dir
        dir = TextFormat("%s../../../%s", appDir, folderName);
        if (DirectoryExists(dir))
        {
            ChangeDirectory(dir);
            return true;
        }

        return false;
    }

#if defined(__cplusplus)
}
#endif