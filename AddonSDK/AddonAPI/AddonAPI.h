#pragma once

#include <windows.h>
#include "../../Lib/Jsoncpp/Jsoncpp.h"
#include "../../Lib/File/File.h"

#pragma comment(lib, "../../Lib/Jsoncpp/Jsoncpp.lib")
#pragma comment(lib,"../../Lib/File/File.lib")

/*
#include <string>
 */

#define BUILDMETHOD extern "C" __declspec(dllexport) Json::Value
#define AFTERMATHHANDLER extern "C" __declspec(dllexport) void

#define WRITE_BUILDINSTRUCT(var) const_cast<Json::Value &>(var)
#define ITER_BUILDINSTRUCT(var) const_cast<Json::Value &>(var)
#define WRITE_AFTERMATH(var) const_cast<Json::Value &>(var)
#define ITER_AFTERMATH(var) const_cast<Json::Value &>(var)

typedef Json::Value BUILDINSTRUCT_TYPE, AFTERMATH_TYPE;
typedef const Json::Value& BUILDINSTRUCT, AFTERMATH;

std::string SetBuildPath(BUILDINSTRUCT buildInstruct);