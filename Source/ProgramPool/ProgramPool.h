#pragma once

#include <string>
#include <vector>

#include <windows.h>
#include "../../Lib/Jsoncpp/Jsoncpp.h"

#pragma comment(lib,"../../Lib/Json/Jsoncpp.lib")

#define PROJECT_TYPE 1
#define PROGRAM_TYPE 2

extern std::string g_fromPublic;
extern std::string g_fromPrivate;

class C_Program;
class C_Project;
//Forward declaration.

struct S_Route{
    std::string from;
    std::string way;
    std::string method;
};


class C_Program{
    private:
        C_Project *m_attachedProject;
        const int m_type = PROGRAM_TYPE;
        Json::Value m_buildInstruct;
        std::vector<S_Route> m_buildRoute;
        std::vector<void (*)(Json::Value)> m_BuildWay;

    public:
        std::string m_name;

        C_Program(C_Project &attachedProject);
        friend void Build(const void *c_projectOrc_program);
};

class C_Project{
    private:
        const int m_type = PROJECT_TYPE;
        Json::Value m_buildInstruct;
        std::vector<S_Route> m_buildRoute;
        std::vector<void (*)(Json::Value)> m_BuildWay;

    public:
        std::string m_name;
        std::string m_path;
        std::vector<C_Program> m_programs;

        friend void Build(const void *c_projectOrc_program);
        void QueryProgram(const C_Program condition,std::vector<std::vector<C_Program>::iterator> &results);
        void AddProgram(const C_Program program);
        void RemoveProgram(const std::vector<C_Program>::iterator program);
};

class C_ProgramPool{
    private:
        std::vector<C_Project> m_projects;

    public:
        void QueryProject(const C_Project condition,std::vector<std::vector<C_Project>::iterator> &results);
        void AddProject(const C_Project project);
        void RemoveProject(const std::vector<C_Project>::iterator project);
};