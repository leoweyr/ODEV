#include "ProgramPool.h"

C_Program::C_Program(C_Project &attachedProject) {
    m_attachedProject = &attachedProject;
}

void Build(const void *c_projectOrc_program) {
    C_Project *isC_projectOrC_program = (C_Project*)c_projectOrc_program;
    std::string projectPath;
    if(isC_projectOrC_program->m_type == PROJECT_TYPE){
        C_Project *isObject = (C_Project*)c_projectOrc_program;
        projectPath = (*isObject).path;
    }else if(isC_projectOrC_program->m_type == PROGRAM_TYPE){
        C_Program *isObject = (C_Program*)c_projectOrc_program;
        projectPath = (*isObject).m_attachedProject->path;
    }
    std::string wayDllPath;
    HMODULE hDLL;
    void (*wayFunction)(Json::Value);
    for(std::vector<S_Route>::iterator iter = (*isObject).m_buildRoute.begin(); iter != (*isObject).m_buildRoute.end(); iter++){
        if((*iter).from == "public"){
            wayDllPath = g_fromPublic + "\\" + way + ".dll";
        } else if((*iter).from == "private"){
            wayDllPath = projectPath + "\\" + g_fromPrivate + "\\" + way + ".dll";
        }
        LoadLibrary(wayDllPath);
        wayFunction = (void(*)(Json::Value))GetProcAddress(hDLL,(*iter).method);
        (*isObject).m_BuildWay.push_back(wayFunction);
        FreeLibrary(hDLL);
    }
    for(std::vector<void(*)(Json::Value)>::iterator iter = (*isObject).m_BuildWay.begin(); iter != (*isObject).m_BuildWay.end(); iter++){
        (*iter)((*isObject).m_buildInstruct);
    }
}

void C_Project::QueryProgram(const C_Program condition, std::vector <std::vector<C_Program>::iterator> &results) {
    for(std::vector<C_Program>::iterator iter = m_programs.begin();iter != m_programs.end();iter++){ //TODO: Add search condition about m_buildInstruct.
        if((*iter).m_name == condition.m_name || condition.m_name.size() == 0){
            results.push_back(iter);
            return;
        }
    }
}

void C_Project::AddProgram(const C_Program program) {
    m_programs.push_back(program);
}

void C_Project::RemoveProgram(const std::vector<C_Program>::iterator program) {
    m_programs.erase(program);
}

void C_ProgramPool::QueryProject(const C_Project condition,std::vector<std::vector<C_Project>::iterator> &results){
    for(std::vector<C_Project>::iterator iter = m_projects.begin();iter != m_projects.end();iter++){
        if(((*iter).m_name == condition.m_name || condition.m_name.size() == 0)&&((*iter).m_path == condition.m_path || condition.m_path.size() == 0)&&((*iter).m_programs == condition.m_programs || condition.m_programs.size() == 0)){ //TODO: Add search condition about m_buildInstruct.
            results.push_back(iter);
            return;
        }
    }
}

void C_ProgramPool::AddProjects(const C_Project project) {
    m_projects.push_back(project);
}

void C_ProgramPool::RemoveProject(const std::vector<C_Project>::iterator project) {
    m_projects.erase(project);
}