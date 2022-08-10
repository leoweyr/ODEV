#pragma once

#include <windows.h>

#include "../../Lib/File/File.h"
#include "../../Lib/System/System.h"

#pragma comment(lib,"../../Lib/File/File.lib")
#pragma comment(lib,"../../Lib/System/System.lib")

/*
#include <string>

#include "../../Lib/Jsoncpp/Jsoncpp.h"

#pragma comment(lib, "../../Lib/Jsoncpp/Jsoncpp.lib")
 */

#define BUILDMETHOD(buildMethod) extern "C" __declspec(dllexport) Json::Value buildMethod(const Json::Value &buildInstruct)
#define AFTERMATHHANDLER(aftermathHandler) extern "C" __declspec(dllexport) void aftermathHandler(const Json::Value &aftermath)
#define AFTERMATHFORMATTER(aftermathHandler) void AftermathFormat_##aftermathHandler

#define READ_BUILDINSTRUCT const Json::Value &
#define READ_AFTERMATH const Json::Value &
#define WRITE_BUILDINSTRUCT(var) const_cast<Json::Value &>(var)
#define WRITE_AFTERMATH(var) const_cast<Json::Value &>(var)
#define AFTERMATHFORMAT(aftermathHandler) AftermathFormat_##aftermathHandler

typedef Json::Value BUILDINSTRUCT, AFTERMATH;

std::string SetBuildPath(READ_BUILDINSTRUCT buildInstruct);