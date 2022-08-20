#include "Deploy.h"

//global dynamics
C_ProgramPool g_programPool;
C_AftermathList g_aftermathList;

// global configuration
std::string g_selfPath;
std::string g_currentProjectPath;
std::string g_publicPath;
std::string g_publicPath_buildWay;
std::string g_publicPath_buildRoute;
std::string g_publicPath_aftermath;
std::string g_privatePath;
std::string g_privatePath_buildWay;
std::string g_privatePath_buildRoute;
std::string g_privatePath_menu;
std::string g_privatePath_aftermath;

void InDynamicBuildRoutes(const std::string projectPath, const Json::Value staticBuildRoutes, const std::vector<S_Route> &dynamicBuildRoutes){ //non-shared
    Json::Value route;
    S_RouteStep buildRouteStep;
    std::vector<S_RouteStep> buildRouteSteps;
    for(Json::Value::iterator staticBuildRoutes_iter = const_cast<Json::Value &>(staticBuildRoutes).begin(); staticBuildRoutes_iter != staticBuildRoutes.end(); staticBuildRoutes_iter++){
        if((*staticBuildRoutes_iter)["from"] == FROM_PRIVATE){
            N_File::C_File(projectPath + "\\" + g_privatePath_buildRoute + "\\" + (*staticBuildRoutes_iter)["route"].asString() + ".json").Read(route);
        }else if((*staticBuildRoutes_iter)["from"] == FROM_PUBLIC){
            N_File::C_File(g_publicPath_buildRoute + "\\" + (*staticBuildRoutes_iter)["route"].asString() + ".json").Read(route);
        }
        for(Json::Value::iterator route_iter = route.begin(); route_iter != route.end(); route_iter++){
            buildRouteStep.from = (*route_iter)["from"].asInt();
            buildRouteStep.way = (*route_iter)["way"].asString();
            buildRouteStep.method = (*route_iter)["method"].asString();
            buildRouteSteps.push_back(buildRouteStep);
        }
        //Remove duplicate direction of buildRouteStep in buildRouteSteps.
        for(std::vector<S_Route>::iterator dynamicstaticBuildRoutes_iter = const_cast<std::vector<S_Route> &>(dynamicBuildRoutes).begin(); dynamicstaticBuildRoutes_iter != dynamicBuildRoutes.end(); dynamicstaticBuildRoutes_iter++){
            if((*dynamicstaticBuildRoutes_iter).direction == (*staticBuildRoutes_iter)["direction"].asString()){
                const_cast<std::vector<S_Route> &>(dynamicBuildRoutes).erase(dynamicstaticBuildRoutes_iter);
            }
        }
        //Enter buildRoute.
        S_Route buildRoute;
        buildRoute.direction = (*staticBuildRoutes_iter)["direction"].asString();
        if((*staticBuildRoutes_iter)["from"] == FROM_PRIVATE){
            buildRoute.from = FROM_PRIVATE;
        }else if((*staticBuildRoutes_iter)["from"] == FROM_PUBLIC){
            buildRoute.from = FROM_PUBLIC;
        }
        buildRoute.name = (*staticBuildRoutes_iter)["route"].asString();
        buildRoute.routeSteps = buildRouteSteps;
        const_cast<std::vector<S_Route> &>(dynamicBuildRoutes).push_back(buildRoute);
    }
}

void MatchStaticBuildRoute(const std::vector<S_RouteStep> dynamicBuildRoute, const std::string projectPath, const Json::Value &staticBuildRoute){ //non-shared
    Json::Value route_dynamic, routeUnit, route_static;
    std::vector<std::string> buildRoutes;
    std::string buildRoutesDir_path;
    bool isStaticBuildRouteExist = false;
    for(std::vector<S_RouteStep>::iterator buildRoute_iter = const_cast<std::vector<S_RouteStep> &>(dynamicBuildRoute).begin(); buildRoute_iter != dynamicBuildRoute.end(); buildRoute_iter++){
        routeUnit["from"] = (*buildRoute_iter).from;
        routeUnit["way"] = (*buildRoute_iter).way;
        routeUnit["method"] = (*buildRoute_iter).method;
        route_dynamic.append(routeUnit);
    }
    for(int matchStep = 0; matchStep < 2; matchStep++){
        buildRoutesDir_path = (matchStep < 1) ? (projectPath + "\\" + g_privatePath_buildRoute) : (g_publicPath_buildRoute);
        buildRoutes = N_File::C_Dir(buildRoutesDir_path).List();
        for(std::vector<std::string>::iterator buildRoutes_iter = buildRoutes.begin(); buildRoutes_iter != buildRoutes.end(); buildRoutes_iter++){
            if(StringSplit((*buildRoutes_iter),".")[-1] == "json"){
                N_File::C_File(buildRoutesDir_path + "\\" + (*buildRoutes_iter)).Read(route_static);
                if(route_static == route_dynamic){
                    const_cast<Json::Value &>(staticBuildRoute)["from"] = (matchStep < 1) ? (FROM_PRIVATE) : (FROM_PUBLIC);
                    const_cast<Json::Value &>(staticBuildRoute)["route"] = StringSplit((*buildRoutes_iter), ".")[-2];
                    isStaticBuildRouteExist = true;
                    break;
                }
            }
        }
    }
    if(isStaticBuildRouteExist == false){
        N_File::C_File(projectPath + "\\" + g_privatePath_buildRoute + "\\" + staticBuildRoute["name"].asString() + "_used.json").Write(route_dynamic);
        const_cast<Json::Value &>(staticBuildRoute)["from"] = FROM_PRIVATE;
        const_cast<Json::Value &>(staticBuildRoute)["route"] = staticBuildRoute["name"].asString() + "_used";
    }
}

void InDynamicProgram(const Json::Value program, const C_Project &attachedProject){
    C_Program cProgram(const_cast<C_Project &>(attachedProject));
    cProgram.m_name = program["name"].asString();
    if(program.isMember("buildInstruct") == true){
        cProgram.m_buildInstruct = program["buildInstruct"];
    }
    if(program.isMember("buildRoutes") == true){
        InDynamicBuildRoutes(attachedProject.m_path, program["buildRoutes"], cProgram.m_buildRoutes);
    }
    //Entered program replaces the same one which already exists in the attached project of global programPool.
    std::vector<C_Program*> samePrograms;
    const_cast<C_Project &>(attachedProject).QueryProgram(&cProgram,samePrograms);
    if(samePrograms.size() != 0){
        for(std::vector<C_Program*>::iterator samePrograms_iter = samePrograms.begin(); samePrograms_iter != samePrograms.end(); samePrograms_iter++){
            const_cast<C_Project &>(attachedProject).RemoveProgram(*samePrograms_iter);
        }
    }
    const_cast<C_Project &>(attachedProject).AddProgram(cProgram);
}

void OutStaticProgram(const std::vector<C_Program*> targetPrograms, const C_Project &attachedProject, std::vector<Json::Value> &programs){
    Json::Value program, buildRoute;
    for(std::vector<C_Program*>::iterator targetPrograms_iter = const_cast<std::vector<C_Program*> &>(targetPrograms).begin(); targetPrograms_iter != targetPrograms.end(); targetPrograms_iter++){
        program["name"] = (*targetPrograms_iter)->m_name;
        program["buildInstruct"] = (*targetPrograms_iter)->m_buildInstruct;
        //Matches the static storage of program's buildRoute which is in the global programPool.
        for(std::vector<S_Route>::iterator buildRoutes_iter = (*targetPrograms_iter)->m_buildRoutes.begin(); buildRoutes_iter != (*targetPrograms_iter)->m_buildRoutes.end(); buildRoutes_iter++){
            MatchStaticBuildRoute((*buildRoutes_iter).routeSteps, attachedProject.m_path, buildRoute);
            buildRoute["direction"] = (*buildRoutes_iter).direction;
            program["buildRoutes"].append(buildRoute);
        }

        programs.push_back(program);
    }
}

void InDynamicProject(const Json::Value project){
    C_Project cProject;
    cProject.m_name = project["name"].asString();
    cProject.m_path = project["path"].asString();
    if(project.isMember("buildInstruct") == true){
        cProject.m_buildInstruct = project["buildInstruct"];
    }
    if(project.isMember("buildRoute") == true){
        InDynamicBuildRoutes(cProject.m_path, project["buildRoutes"], cProject.m_buildRoutes);
    }
    //Entered project replaces the same one which already exists in global programPool.
    std::vector<C_Project*> sameProjects;
    g_programPool.QueryProject(&cProject,sameProjects);
    if(sameProjects.size() != 0){
        for(std::vector<C_Project*>::iterator sameProjects_iter = sameProjects.begin(); sameProjects_iter != sameProjects.end(); sameProjects_iter++){
            g_programPool.RemoveProject(*sameProjects_iter);
        }
    }
    g_programPool.AddProject(cProject);
}

void OutStaticProject(const std::vector<C_Project*> targetProjects, std::vector<Json::Value> &projects){
    Json::Value project, buildRoute;
    for(std::vector<C_Project*>::iterator targetProjects_iter = const_cast<std::vector<C_Project*> &>(targetProjects).begin(); targetProjects_iter != targetProjects.end(); targetProjects_iter++){
        project["name"] = (*targetProjects_iter)->m_name;
        project["path"] = (*targetProjects_iter)->m_path;
        project["buildInstruct"] = (*targetProjects_iter)->m_buildInstruct;
        //Matches the static storage of project's buildRoute which is in the global programPool.
        for(std::vector<S_Route>::iterator buildRoutes_iter = (*targetProjects_iter)->m_buildRoutes.begin(); buildRoutes_iter != (*targetProjects_iter)->m_buildRoutes.end(); buildRoutes_iter++){
            MatchStaticBuildRoute((*buildRoutes_iter).routeSteps, (*targetProjects_iter)->m_path, buildRoute);
            buildRoute["direction"] = (*buildRoutes_iter).direction;
            project["buildRoutes"].append(buildRoute);
        }

        projects.push_back(project);
    }
}

void InDynamicAftermath(const Json::Value aftermath){
    C_Aftermath cAftermath;
    cAftermath.m_priority = aftermath["priority"].asInt();
    cAftermath.m_from = aftermath["from"].asInt();
    cAftermath.m_name = aftermath["name"].asString();
    cAftermath.m_method = aftermath["method"].asString();
    //Entered aftermath replaces the same one which already exists in global aftermathList.
    std::vector<C_Aftermath*> sameAftermaths;
    g_aftermathList.QueryAftermath(&cAftermath,sameAftermaths);
    if(sameAftermaths.size() != 0){
        for(std::vector<C_Aftermath*>::iterator sameAftermaths_iter = sameAftermaths.begin(); sameAftermaths_iter != sameAftermaths.end(); sameAftermaths_iter++){
            g_aftermathList.RemoveProgram(*sameAftermaths_iter);
        }
    }
    g_aftermathList.AddAftermath(cAftermath);
}

void OutStaticAftermath(const std::vector<C_Aftermath*> targetAftermaths, std::vector<Json::Value> &aftermaths){
    Json::Value aftermath;
    for(std::vector<C_Aftermath*>::iterator targetAftermaths_iter = const_cast<std::vector<C_Aftermath*> &>(targetAftermaths).begin(); targetAftermaths_iter != targetAftermaths.end(); targetAftermaths_iter++){
        aftermath["priority"] = (*targetAftermaths_iter)->m_priority;
        aftermath["from"] = (*targetAftermaths_iter)->m_from;
        aftermath["name"] = (*targetAftermaths_iter)->m_name;
        aftermath["method"] = (*targetAftermaths_iter)->m_method;
        aftermaths.push_back(aftermath);
    }
}

void SetGlobalConfig(){
    g_selfPath = CURRENT_EXE_PATH;
    g_publicPath = g_selfPath + "\\" + "public";
    g_publicPath_buildWay = g_publicPath + "\\" + "buildWay";
    g_publicPath_buildRoute = g_publicPath + "\\" + "buildRoute";
    g_publicPath_aftermath = g_publicPath + "\\" + "aftermath";
    g_privatePath = ".ODEV";
    g_privatePath_buildWay = g_privatePath + "\\" + "graph\\way";
    g_privatePath_buildRoute = g_privatePath + "\\" + "graph\\route";
    g_privatePath_menu = g_privatePath + "\\" + "graph\\menu";
    g_privatePath_aftermath = g_privatePath + "\\" + "aftermath";
}

bool MatchProjectPath(const std::string &path){
    Json::Value projectPaths;
    std::vector<std::string> splitProjectPathArray, splitPathArray, splitPathArray_specifiedDepth;
    std::string path_specifiedDepth;
    N_File::C_File projectPaths_json(g_selfPath + "\\" + "projects.json");
    projectPaths_json.Read(projectPaths);
    int projectPaths_index = 0;
    for(Json::Value::iterator projectPaths_iter = projectPaths.begin(); projectPaths_iter != projectPaths.end(); projectPaths_iter++){
        splitProjectPathArray = StringSplit(projectPaths_iter->asString(),"\\");
        splitPathArray = StringSplit(path,"\\");
        for(int i=0; i<splitProjectPathArray.size(); i++){
            splitPathArray_specifiedDepth.push_back(splitPathArray[i]);
        }
        path_specifiedDepth = StringUnite(splitPathArray_specifiedDepth,"\\");
        if(path_specifiedDepth == projectPaths_iter->asString()){
            if(N_File::C_Dir(projectPaths_iter->asString()).isExist() == true){
                const_cast<std::string &>(path) = projectPaths_iter->asString();
                g_currentProjectPath = projectPaths_iter->asString();
                return true;
            } else{
                projectPaths.removeIndex(projectPaths_index,NULL);
                projectPaths_json.Write(projectPaths);
            }
        }
        projectPaths_index ++;
    }
    return false;
}

void InstallProgramPool(const std::string currentProjectPath){
    Json::Value project, program;
    //Enter project.
    N_File::C_File(currentProjectPath + "\\" + g_privatePath_menu + "\\" + ".self.json").Read(project);
    project["path"] = currentProjectPath;
    InDynamicProject(project);
    //Enter programs of project.
    C_Project *cProject;
    std::vector<C_Project*> cProjects;
    cProject->m_path = currentProjectPath;
    g_programPool.QueryProject(cProject,cProjects);
    cProject = cProjects[0];
    std::vector<std::string> menus = N_File::C_Dir(currentProjectPath + "\\" + g_privatePath_menu).List();
    for(std::vector<std::string>::iterator menus_iter = menus.begin(); menus_iter != menus.end(); menus_iter++){
        if(StringSplit((*menus_iter),".")[(*menus_iter).size() - 1] == "json" && (*menus_iter) != ".self.json"){
            N_File::C_File(currentProjectPath + "\\" + g_privatePath_menu + "\\" + (*menus_iter)).Read(program);
            InDynamicProgram(program, *cProject);
        }
    }
}

void UninstallProgramPool(const std::string currentProjectPath){
    //Outer project.
    C_Project *cProject;
    std::vector<C_Project*> cProjects;
    std::vector<Json::Value> projects_json;
    cProject->m_path = currentProjectPath;
    g_programPool.QueryProject(cProject, cProjects);
    OutStaticProject(cProjects, projects_json);
    projects_json[0].removeMember("path");
    N_File::C_File(currentProjectPath + "\\" + g_privatePath_menu + "\\" + ".self.json").Write(projects_json[0]);
    //Outer programs of project.
    std::vector<C_Program*> cPrograms;
    std::vector<Json::Value> programs_json;
    cProject = cProjects[0];
    cProject->QueryProgram(NULL,cPrograms);
    OutStaticProgram(cPrograms, *cProject, programs_json);
    for(std::vector<Json::Value>::iterator programs_json_iter = programs_json.begin(); programs_json_iter != programs_json.end(); programs_json_iter++){
        N_File::C_File(currentProjectPath + "\\" + g_privatePath_menu + "\\" + (*programs_json_iter)["name"].asString() + ".json").Write(*programs_json_iter);
    }
}

void InstallAftermathList(const std::string currentProjectPath){
    Json::Value aftermaths;
    std::string aftermathsPath;
    for(int installStep = 0; installStep < 2; installStep++){
        aftermathsPath = (installStep < 1)?(g_publicPath_aftermath + "\\" + "aftermaths.json"):(currentProjectPath + "\\" + g_privatePath_aftermath + "\\" + "aftermaths.json");
        N_File::C_File(aftermathsPath).Read(aftermaths);
        for(Json::Value::iterator aftermaths_iter = aftermaths.begin(); aftermaths_iter != aftermaths.end(); aftermaths_iter++){
            (*aftermaths_iter)["from"] = (installStep < 1)?(FROM_PRIVATE):(FROM_PUBLIC);
            InDynamicAftermath(*aftermaths_iter);
        }
    }
}

void UninstallAftermathList(const std::string currentProjectPath){
    std::vector<C_Aftermath*> cAftermaths;
    Json::Value aftermath, aftermaths_public, aftermaths_private;
    std::vector<Json::Value> aftermaths;
    g_aftermathList.QueryAftermath(NULL,cAftermaths);
    OutStaticAftermath(cAftermaths, aftermaths);
    for(std::vector<Json::Value>::iterator aftermaths_iter = aftermaths.begin(); aftermaths_iter != aftermaths.end(); aftermaths_iter++){
        if((*aftermaths_iter)["from"] == FROM_PUBLIC){
            (*aftermaths_iter).removeMember("from");
            aftermaths_public.append((*aftermaths_iter));
        }else if((*aftermaths_iter)["from"] == FROM_PRIVATE){
            (*aftermaths_iter).removeMember("from");
            aftermaths_private.append((*aftermaths_iter));
        }
    }
    N_File::C_File(g_publicPath_aftermath + "\\" + "aftermaths.json").Write(aftermaths_public);
    N_File::C_File(currentProjectPath + "\\" + g_privatePath_aftermath + "\\" + "aftermaths.json").Write(aftermaths_private);
}

void CheckPrivatePath(){
    N_File::C_Dir(g_currentProjectPath + "\\" + g_privatePath_menu).isExist(1);
    N_File::C_Dir(g_currentProjectPath + "\\" + g_privatePath_buildRoute).isExist(1);
    N_File::C_Dir(g_currentProjectPath + "\\" + g_privatePath_buildWay).isExist(1);
    N_File::C_Dir(g_currentProjectPath + "\\" + g_privatePath_aftermath).isExist(1);
}