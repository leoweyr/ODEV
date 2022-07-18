#pragma once

#include "../../Lib/File/File.h"

#include "../ProgramPool/ProgramPool.h"

#pragma comment(lib,"../../Lib/File/File.lib")

/*
#include <string>
#include <vector>

#include "../../Lib/Jsoncpp/Jsoncpp.h"
#include "../../Lib/Bitscode/Bitscode.h"

#pragma comment(lib,"../../Lib/Json/Jsoncpp.lib
#pragma comment(lib,"../../Lib/Bitscode/Bitscode.lib")
 */

C_ProgramPool g_programPool;
std::string g_publicPath;
std::string g_publicPath_buildWay;
std::string g_publicPath_buildRoute;
std::string g_privatePath;
std::string g_privatePath_buildWay;
std::string g_privatePath_buildRoute;
std::string g_privatePath_menu;

void MatchStaticBuildRoute(const std::vector<S_Route> dynamicBuildRoute, const std::string projectPath, Json::Value &menuUnit);

void InDynamicProgram(const Json::Value program, C_Project &attachedProject);

void OutStaticProgram(std::vector<Json::Value> &programs, C_Project &attachedProject);

void InDynamicProject(const Json::Value project);

void OutStaticProject(std::vector<Json::Value> &projects);