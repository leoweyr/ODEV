#pragma once

#include <string>
#include <vector>
#include <map>

#include <windows.h>
#include "../../Lib/Jsoncpp/Jsoncpp.h"

#pragma comment(lib,"../../Lib/Json/Jsoncpp.lib")

#define PROJECT_TYPE 1
#define PROGRAM_TYPE 2

extern std::string g_publicPath_buildWay;
extern std::string g_privatePath_buildWay;

//Forward declaration.
class C_Program;
class C_Project;

struct S_Route{
    std::string from;
    std::string way;
    std::string method;
};


class C_Program{
    private:
        const int m_type = PROGRAM_TYPE;
        C_Project *m_attachedProject;
        std::vector<Json::Value (*)(Json::Value)> m_BuildWay;

    public:
        std::string m_name;
        Json::Value m_buildInstruct;
        std::map<std::string, std::vector<S_Route>> m_buildRoutes;

        C_Program(C_Project &attachedProject);
        friend std::vector<Json::Value> Build(void *projectOrProgram, const std::string direction);
};

class C_Project{
    private:
        const int m_type = PROJECT_TYPE;
        std::vector<Json::Value (*)(Json::Value)> m_BuildWay;

    public:
        std::string m_name;
        std::string m_path;
        std::vector<C_Program> m_programs;
        Json::Value m_buildInstruct;
        std::map<std::string, std::vector<S_Route>> m_buildRoutes;

        friend std::vector<Json::Value> Build(void *projectOrProgram, const std::string direction);
        void QueryProgram(const C_Program *condition,std::vector<std::vector<C_Program>::iterator> &results);
        void AddProgram(const C_Program program);
        void RemoveProgram(const std::vector<C_Program>::iterator program);
};

class C_ProgramPool{
    public:
        std::vector<C_Project> m_projects;
        void QueryProject(const C_Project *condition,std::vector<std::vector<C_Project>::iterator> &results);
        void AddProject(const C_Project project);
        void RemoveProject(const std::vector<C_Project>::iterator project);
};