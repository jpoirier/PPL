// Copyright (c) 2013, Philipp Muenzel mail@philippmuenzel.de
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// The views and conclusions contained in the software and documentation are those
// of the authors and should not be interpreted as representing official policies,
// either expressed or implied, of the FreeBSD Project.

#include <string>
#include <cstring>

#if APL
#if defined(__MACH__)
#include <CoreFoundation/CoreFoundation.h>
#endif
#endif

#include "XPLMUtilities.h"
#include "XPLMPlanes.h"

#include "pluginpath.h"

using namespace PPLNAMESPACE;

std::string PluginPath::plugin_directory = "";

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::string PluginPath::prependXPlanePath(const std::string& file)
{
#ifdef BUILD_FOR_STANDALONE
    return file;
#else
    char path[512];
    XPLMGetSystemPath(path);
    std::string absolute_path(path);
#if APL && __MACH__
    int result = ConvertPath(absolute_path.c_str(), path, 512);
    if (result == 0)
        absolute_path = std::string(path);
    else
        throw PathSetupError("Critical error - cannot convert Mac-HFS-format path to unix-format path");
#endif
    absolute_path.append(file);
    return absolute_path;
#endif
}

std::string PluginPath::prependPluginPath(const std::string& file)
{
#ifdef BUILD_FOR_STANDALONE
    return file;
#else
    std::string path = "/Resources/plugins/";
    path.append(plugin_directory).append("/").append(file);
    return prependXPlanePath(path);
#endif
}

std::string PluginPath::prependPluginResourcesPath(const std::string& file)
{
    std::string res_path("Resources/");
    res_path.append(file);
#ifdef BUILD_FOR_STANDALONE
    return res_path;
#else
    return prependPluginPath(res_path);
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::string PluginPath::prependPlanePath(const std::string& file)
{
#ifdef BUILD_FOR_STANDALONE
    return file;
#else
    char name[512];
    char path[512];
    XPLMGetNthAircraftModel(0, name, path);
    std::string absolute_path(path);
    std::size_t pos = absolute_path.find(name);
    absolute_path = absolute_path.substr(0, pos);
#if APL && __MACH__
    int result = ConvertPath(absolute_path.c_str(), path, 512);
    if (result == 0)
        absolute_path = std::string(path);
    else
        throw PathSetupError("Critical error - cannot convert Mac-HFS-format path to unix-format path");
#endif
    absolute_path.append(file);
    return absolute_path;
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void PluginPath::setPluginDirectoryName(const std::string& name)
{
    PluginPath::plugin_directory = name;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if APL && __MACH__
int PluginPath::ConvertPath(const char * inPath, char * outPath, int outPathMaxLen)
{
    CFStringRef inStr = CFStringCreateWithCString(kCFAllocatorDefault, inPath ,kCFStringEncodingMacRoman);
    if (inStr == NULL)
        return -1;
    CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, inStr, kCFURLHFSPathStyle,0);
    CFStringRef outStr = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
    if (!CFStringGetCString(outStr, outPath, outPathMaxLen, kCFURLPOSIXPathStyle))
        return -1;
    CFRelease(outStr);
    CFRelease(url);
    CFRelease(inStr);
    return 0;
}
#endif

