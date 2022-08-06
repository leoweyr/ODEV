#pragma once

#include "../../Lib/File/File.h"

#include "../ProgramPool/ProgramPool.h"
#include "../AftermathList/AftermathList.h"

#pragma comment(lib,"../../Lib/File/File.lib")

/*
#include <string>
#include <vector>

#include "../../Lib/Jsoncpp/Jsoncpp.h"
#include "../../Lib/Bitscode/Bitscode.h"

#pragma comment(lib,"../../Lib/Json/Jsoncpp.lib")
#pragma comment(lib,"../../Lib/Bitscode/Bitscode.lib")
 */

/* Basic Operation */
void SetGlobalConfig();

void InDynamicProgram(const Json::Value program, const C_Project &attachedProject);

void OutStaticProgram(const std::vector<C_Program*> targetPrograms, const C_Project &attachedProject, std::vector<Json::Value> &programs);

void InDynamicProject(const Json::Value project);

void OutStaticProject(const std::vector<C_Project*> targetProjects, std::vector<Json::Value> &projects);

void InDynamicAftermath(const Json::Value aftermath);

void OutStaticAftermath(const std::vector<C_Aftermath*> targetAftermaths, std::vector<Json::Value> &aftermaths);

/* Detailed Task */
bool MatchProjectPath(const std::string &path);

void InstallProgramPool(const std::string currentProjectPath);

void UninstallProgramPool(const std::string currentProjectPath);

void InstallAftermathList(const std::string currentProjectPath);

void UninstallAftermathList(const std::string currentProjectPath);