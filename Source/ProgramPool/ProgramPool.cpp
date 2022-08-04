#include "ProgramPool.h"

C_Program::C_Program(C_Project &attachedProject) {
    m_attachedProject = &attachedProject;
}

int Build(void *projectOrProgram, const std::string direction, std::vector<Json::Value> &aftermaths) {
    int errorCode = 0;
    //Identify whether param<c_projectOrc_program> points to class<C_Project> object or points to class<C_Program> object, get buildRoute of the specified direction in member<m_buildRoutes>.
    C_Project *isProjectOrProgram = (C_Project*)projectOrProgram;
    std::string projectPath;
    std::vector<S_Route> buildRoute;
    if(isProjectOrProgram->m_type == TYPE_PROJECT){
        C_Project *buildObject = (C_Project*)projectOrProgram;
        projectPath = (*buildObject).m_path;
        buildRoute = (*buildObject).m_buildRoutes[direction];
    }else if(isProjectOrProgram->m_type == TYPE_PROGRAM){
        C_Program *buildObject = (C_Program*)projectOrProgram;
        projectPath = (*buildObject).m_attachedProject->m_path;
        buildRoute = (*buildObject).m_buildRoutes[direction];
    }
    //Read the execution sequence buildRoute of build way and write it into the preset queue.
    Json::Value aftermath;
    std::string wayDllPath;
    HMODULE hDLL;
    std::vector<HMODULE> hDLLs;
    Json::Value (*BuildMethod)(Json::Value);
    for(std::vector<S_Route>::iterator buildRoute_iter = buildRoute.begin(); buildRoute_iter != buildRoute.end(); buildRoute_iter++){
        if((*buildRoute_iter).from == FROM_PRIVATE){
            wayDllPath = projectPath + "\\" + g_privatePath_buildWay + "\\" + (*buildRoute_iter).way + ".dll";
        }else if((*buildRoute_iter).from == FROM_PUBLIC){
            wayDllPath = g_publicPath_buildWay + "\\" + (*buildRoute_iter).way + ".dll";
        }
        hDLL = LoadLibrary(wayDllPath.data());
        hDLLs.push_back(hDLL);
        BuildMethod = (Json::Value(*)(Json::Value))GetProcAddress(hDLL,(*buildRoute_iter).method.data());
        if(isProjectOrProgram->m_type == TYPE_PROJECT){
            C_Project *buildObject = (C_Project*)projectOrProgram;
            (*buildObject).m_BuildWay.push_back(BuildMethod);
        }else if(isProjectOrProgram->m_type == TYPE_PROGRAM){
            C_Program *buildObject = (C_Program*)projectOrProgram;
            (*buildObject).m_BuildWay.push_back(BuildMethod);
        }

    }
    //Execute each build way in sequence according to the preset queue.
    if(isProjectOrProgram->m_type == TYPE_PROJECT){
        C_Project *buildObject = (C_Project*)projectOrProgram;
        for(std::vector<Json::Value(*)(Json::Value)>::iterator BuildWay_iter = (*buildObject).m_BuildWay.begin(); BuildWay_iter != (*buildObject).m_BuildWay.end(); BuildWay_iter++){
            //Get basic data for buildInstruct.
            (*buildObject).m_buildInstruct["basic"]["projectPath"] = (*buildObject).m_path;
            (*buildObject).m_buildInstruct["basic"]["name"] = (*buildObject).m_name;
            //Get aftermath from executed buildWay.
            aftermath = (*BuildWay_iter)((*buildObject).m_buildInstruct);
            //Get basic data for aftermath.
            aftermath["basic"]["type"] = TYPE_PROJECT;
            aftermath["basic"]["name"] = (*buildObject).m_name;

            aftermaths.push_back(aftermath);
            //Cheak if buildRoute is blocked.
            if(aftermath["basic"].isMember("isBlock") == true){
                if(aftermath["basic"]["isBlock"].asBool() == true){
                    errorCode = 1;
                    break;
                }
            }
        }
    }else if(isProjectOrProgram->m_type == TYPE_PROGRAM){
        C_Program *buildObject = (C_Program*)projectOrProgram;
        for(std::vector<Json::Value(*)(Json::Value)>::iterator BuildWay_iter = (*buildObject).m_BuildWay.begin(); BuildWay_iter != (*buildObject).m_BuildWay.end(); BuildWay_iter++){
            //Get basic data for buildInstruct.
            (*buildObject).m_buildInstruct["basic"]["projectPath"] = (*buildObject).m_attachedProject->m_path;
            (*buildObject).m_buildInstruct["basic"]["name"] = (*buildObject).m_name;
            //Get aftermath from executed buildWay.
            aftermath = (*BuildWay_iter)((*buildObject).m_buildInstruct);
            //Get basic data for aftermath.
            aftermath["basic"]["type"] = TYPE_PROGRAM;
            aftermath["basic"]["name"] = (*buildObject).m_name;

            aftermaths.push_back(aftermath);
            //Cheak if buildRoute is blocked.
            if(aftermath["basic"].isMember("isBlock") == true){
                if(aftermath["basic"]["isBlock"].asBool() == true){
                    errorCode = 1;
                    break;
                }
            }
        }
    }
    //Free DLLs' handle.
    for(std::vector<HMODULE>::iterator hDLLs_iter = hDLLs.begin(); hDLLs_iter != hDLLs.end(); hDLLs_iter++){
        FreeLibrary((*hDLLs_iter));
    }
    return errorCode;
}

void C_Project::QueryProgram(const C_Program *condition, std::vector<C_Program*> &results) {
    for(std::vector<C_Program>::iterator programs_iter = m_programs.begin(); programs_iter != m_programs.end(); programs_iter++){
        if(condition != NULL){
            if((*programs_iter).m_name == condition->m_name || condition->m_name.size() == 0){
                if(condition->m_buildInstruct.size() != 0){
                    Json::Comparer comparer;
                    comparer.compare((*programs_iter).m_buildInstruct, condition->m_buildInstruct);
                    if(comparer.isIncluded() == false){
                        continue;
                    }
                }
            }else{
                continue;
            }
        }
        results.push_back(&(*programs_iter));
    }
}

void C_Project::AddProgram(const C_Program program) {
    m_programs.push_back(program);
}

void C_Project::RemoveProgram(const C_Program* program) {
    for(std::vector<C_Program>::iterator programs_iter = m_programs.begin(); programs_iter != m_programs.end(); programs_iter++){
        if((*programs_iter).m_name == program->m_name){
            if(program->m_buildInstruct.size() != 0){
                Json::Comparer comparer;
                comparer.compare((*programs_iter).m_buildInstruct, program->m_buildInstruct);
                if(comparer.isIncluded() == false){
                    continue;
                }
            }
            m_programs.erase(programs_iter);
        }
    }
}

void C_ProgramPool::QueryProject(const C_Project *condition, std::vector<C_Project*> &results){
    for(std::vector<C_Project>::iterator projects_iter = m_projects.begin(); projects_iter != m_projects.end(); projects_iter++){
        if(condition != NULL){
            if(((*projects_iter).m_name == condition->m_name || condition->m_name.size() == 0) && ((*projects_iter).m_path == condition->m_path || condition->m_path.size() == 0)){
                if(condition->m_buildInstruct.size() != 0){
                    Json::Comparer comparer;
                    comparer.compare((*projects_iter).m_buildInstruct, condition->m_buildInstruct);
                    if(comparer.isIncluded() == false){
                        continue;
                    }
                }
            }else{
                continue;
            }
        }
        results.push_back(&(*projects_iter));
    }
}

void C_ProgramPool::AddProject(const C_Project project) {
    m_projects.push_back(project);
}

void C_ProgramPool::RemoveProject(const C_Project* project) {
    for(std::vector<C_Project>::iterator projects_iter = m_projects.begin(); projects_iter != m_projects.end(); projects_iter++){
        if((*projects_iter).m_name == project->m_name && (*projects_iter).m_path == project->m_path){
            if(project->m_buildInstruct.size() != 0){
                Json::Comparer comparer;
                comparer.compare((*projects_iter).m_buildInstruct, project->m_buildInstruct);
                if(comparer.isIncluded() == false){
                    continue;
                }
            }
            m_projects.erase(projects_iter);
        }
    }
}