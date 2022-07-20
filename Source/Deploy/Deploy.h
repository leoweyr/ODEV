#pragma once

#include "../../Lib/File/File.h"

#include "../ProgramPool/ProgramPool.h"

#pragma comment(lib,"../../Lib/File/File.lib")

/*
#include <string>
#include <vector>

#include "../../Lib/Jsoncpp/Jsoncpp.h"
#include "../../Lib/Bitscode/Bitscode.h"

#pragma comment(lib,"../../Lib/Json/Jsoncpp.lib")
#pragma comment(lib,"../../Lib/Bitscode/Bitscode.lib")
 */

C_ProgramPool g_programPool;

/* Global Configuration */
std::string g_selfPath;
std::string g_publicPath;
std::string g_publicPath_buildWay;
std::string g_publicPath_buildRoute;
std::string g_privatePath;
std::string g_privatePath_buildWay;
std::string g_privatePath_buildRoute;
std::string g_privatePath_menu;

/* Basic Operation */
void SetGlobalConfig();
void MatchStaticBuildRoute(const std::vector<S_Route> dynamicBuildRoute, const std::string projectPath, const Json::Value &menuUnit);
void InDynamicProgram(const Json::Value program, const C_Project &attachedProject);
void OutStaticProgram(const std::vector<std::vector<C_Program>::iterator> targetPrograms, const C_Project &attachedProject, std::vector<Json::Value> &programs);
void InDynamicProject(const Json::Value project);
void OutStaticProject(const std::vector<std::vector<C_Project>::iterator> targetProjects, std::vector<Json::Value> &projects);

/* Detailed Task */
void InstallProgramPool();
void UnInstallProgramPool();