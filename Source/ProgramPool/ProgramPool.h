#pragma once

#include <string>
#include <vector>
#include <map>

#include <windows.h>
#include "../../Lib/Jsoncpp/Jsoncpp.h"

#pragma comment(lib,"../../Lib/Json/Jsoncpp.lib")

#define FROM_PRIVATE 0
#define FROM_PUBLIC 1
#define TYPE_PROGRAM 0
#define TYPE_PROJECT 1


extern std::string g_publicPath_buildWay;
extern std::string g_privatePath_buildWay;

//Forward declaration.
class C_Program;
class C_Project;
class C_ProgramPool;

struct S_Route{
    int from;
    std::string way;
    std::string method;
};


class C_Program{
    private:
        int m_type = TYPE_PROGRAM;
        C_Project *m_attachedProject;
        std::vector<Json::Value (*)(Json::Value)> m_BuildWay;

        friend int Build(void *projectOrProgram, const std::string direction, std::vector<Json::Value> &aftermaths);

    public:
        std::string m_name;
        Json::Value m_buildInstruct;
        std::map<std::string, std::vector<S_Route>> m_buildRoutes;

        C_Program(C_Project &attachedProject);
};

class C_Project{
    private:
        int m_type = TYPE_PROJECT;
        std::vector<C_Program> m_programs;
        std::vector<Json::Value (*)(Json::Value)> m_BuildWay;

        friend class C_ProgramPool;
        friend int Build(void *projectOrProgram, const std::string direction, std::vector<Json::Value> &aftermaths);

    public:
        std::string m_name;
        std::string m_path;
        Json::Value m_buildInstruct;
        std::map<std::string, std::vector<S_Route>> m_buildRoutes;

        void QueryProgram(const C_Program *condition, std::vector<C_Program*> &results);
        void AddProgram(const C_Program program);
        void RemoveProgram(const C_Program* program);
};

class C_ProgramPool{
    private:
        std::vector<C_Project> m_projects;

    public:
        void QueryProject(const C_Project *condition, std::vector<C_Project*> &results);
        void AddProject(const C_Project project);
        void RemoveProject(const C_Project* project);
};