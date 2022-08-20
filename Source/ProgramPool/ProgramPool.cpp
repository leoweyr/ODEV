#include "ProgramPool.h"

extern C_AftermathList g_aftermathList;

int Build(void *projectOrProgram, const std::string direction) {
    int errorCode = 0;
    //Get basic data pointers of buildObject and buildRoute of the specified direction in member<m_buildRoutes>.
    C_Project *isProjectOrProgram = (C_Project*)projectOrProgram;
    int *type;
    std::string *projectPath;
    std::string *name;
    Json::Value *buildInstruct;
    S_Route *buildRoute;
    std::vector<Json::Value (*)(Json::Value)> *buildWays;
    if(isProjectOrProgram->m_type == TYPE_PROJECT){
        C_Project *buildObject = (C_Project*)projectOrProgram;
        type = &(*buildObject).m_type;
        projectPath = &(*buildObject).m_path;
        name = &(*buildObject).m_name;
        buildInstruct = &(*buildObject).m_buildInstruct;
        for(std::vector<S_Route>::iterator buildRoutes_iter = (*buildObject).m_buildRoutes.begin(); buildRoutes_iter != (*buildObject).m_buildRoutes.end(); buildRoutes_iter++){
            if((*buildRoutes_iter).direction == direction){
                buildRoute = &(*buildRoutes_iter);
                break;
            }
        }
        buildWays = &(*buildObject).m_BuildWays;
    }else if(isProjectOrProgram->m_type == TYPE_PROGRAM){
        C_Program *buildObject = (C_Program*)projectOrProgram;
        type = &(*buildObject).m_type;
        projectPath = &(*buildObject).m_attachedProject->m_path;
        name = &(*buildObject).m_name;
        for(std::vector<S_Route>::iterator buildRoutes_iter = (*buildObject).m_buildRoutes.begin(); buildRoutes_iter != (*buildObject).m_buildRoutes.end(); buildRoutes_iter++){
            if((*buildRoutes_iter).direction == direction){
                buildRoute = &(*buildRoutes_iter);
                break;
            }
        }
        buildWays = &(*buildObject).m_BuildWays;
    }
    //Read the execution sequence buildRoute of build way and write it into the preset queue.
    std::string wayDllPath;
    HMODULE hDLL;
    std::vector<HMODULE> hDLLs;
    Json::Value (*BuildMethod)(Json::Value);
    int buildRouteStep_max = 0;
    for(std::vector<S_RouteStep>::iterator buildRouteSteps_iter = buildRoute->routeSteps.begin(); buildRouteSteps_iter != buildRoute->routeSteps.end(); buildRouteSteps_iter++){
        if((*buildRouteSteps_iter).from == FROM_PRIVATE){
            wayDllPath = *projectPath + "\\" + g_privatePath_buildWay + "\\" + (*buildRouteSteps_iter).way + ".dll";
        }else if((*buildRouteSteps_iter).from == FROM_PUBLIC){
            wayDllPath = g_publicPath_buildWay + "\\" + (*buildRouteSteps_iter).way + ".dll";
        }
        hDLL = LoadLibrary(wayDllPath.data());
        hDLLs.push_back(hDLL);
        BuildMethod = (Json::Value(*)(Json::Value))GetProcAddress(hDLL,(*buildRouteSteps_iter).method.data());
        if(*type == TYPE_PROJECT){
            C_Project *buildObject = (C_Project*)projectOrProgram;
            (*buildObject).m_BuildWays.push_back(BuildMethod);
        }else if(*type == TYPE_PROGRAM){
            C_Program *buildObject = (C_Program*)projectOrProgram;
            (*buildObject).m_BuildWays.push_back(BuildMethod);
        }
        buildRouteStep_max ++;
    }
    //Execute each build way in sequence according to the preset queue.
    int buildRouteStep = 1;
    for(std::vector<Json::Value(*)(Json::Value)>::iterator BuildWay_iter = (*buildWays).begin(); BuildWay_iter != (*buildWays).end(); BuildWay_iter++){
        std::string typeDisplay = (*type == TYPE_PROGRAM)?("PROGRAM"):("PROJECT");
        CONSOLE_PRINT_BUILD(
                errorCode,
                buildRoute,
                FROM_PUBLIC,
                buildRouteStep,
                typeDisplay,
                name,
                buildRouteStep_max,
                Json::Value aftermath;
                //Get basic data for buildInstruct.
                (*buildInstruct)["basic"]["projectPath"] = *projectPath;
                (*buildInstruct)["basic"]["name"] = *name;
                //Get aftermath from executed buildWay.

                aftermath = (*BuildWay_iter)(buildInstruct);

                //Get basic data for aftermath.
                aftermath["basic"]["type"] = TYPE_PROJECT;
                aftermath["basic"]["name"] = *name;

                g_aftermathList.Handle(aftermath);
                //Cheak if buildRoute is blocked.
                if(aftermath["basic"].isMember("isBlock") == true){
                    if(aftermath["basic"]["isBlock"].asBool() == true){
                        errorCode = 1;
                    }
                }
                )
        if(errorCode == 1){
            break;
        }
        buildRouteStep ++;
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

C_Program::C_Program(C_Project &attachedProject) {
    m_attachedProject = &attachedProject;
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