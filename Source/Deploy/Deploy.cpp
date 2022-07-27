#include "Deploy.h"

void SetGlobalConfig(){
    g_selfPath = CURRENT_EXE_PATH;
    g_publicPath = g_selfPath + "\\" + "public";
    g_publicPath_buildWay = g_publicPath + "\\" + "buildWay";
    g_publicPath_buildRoute = g_publicPath + "\\" + "buildRoute";
    g_privatePath = ".ODEV";
    g_privatePath_buildWay = g_privatePath + "\\" + "graph\\way";
    g_privatePath_buildRoute = g_privatePath + "\\" + "graph\\route";
    g_privatePath_menu = g_privatePath + "\\" + "graph\\menu";
}

void InDynamicBuildRoutes(const std::string projectPath, const Json::Value staticBuildRoutes, const std::map<std::string, std::vector<S_Route>> &dynamicBuildRoutes){
    Json::Value route;
    S_Route buildRouteStep;
    std::vector<S_Route> buildRoute;
    for(Json::Value::iterator buildRoutes_iter = const_cast<Json::Value &>(staticBuildRoutes).begin(); buildRoutes_iter != staticBuildRoutes.end(); buildRoutes_iter++){
        if((*buildRoutes_iter)["from"] == FROM_PRIVATE){
            N_File::C_File(projectPath + "\\" + g_privatePath_buildRoute + "\\" + (*buildRoutes_iter)["route"].asString() + ".json").Read(route);
        }else if((*buildRoutes_iter)["from"] == FROM_PUBLIC){
            N_File::C_File(g_publicPath_buildRoute + "\\" + (*buildRoutes_iter)["route"].asString() + ".json").Read(route);
        }
        for(Json::Value::iterator route_iter = route.begin(); route_iter != route.end(); route_iter++){
            buildRouteStep.from = (*route_iter)["from"].asInt();
            buildRouteStep.way = (*route_iter)["way"].asString();
            buildRouteStep.method = (*route_iter)["method"].asString();
            buildRoute.push_back(buildRouteStep);
        }
        //Remove duplicate direction of buildRoute in buildRoutes.
        while (dynamicBuildRoutes.count((*buildRoutes_iter)["direction"].asString()) != 0){
            std::map<std::string, std::vector<S_Route>>::iterator sameDirection = dynamicBuildRoutes.find((*buildRoutes_iter)["direction"].asString());
            const_cast<std::map<std::string, std::vector<S_Route>> &>(dynamicBuildRoutes).erase(sameDirection);
        }

        const_cast<std::map<std::string, std::vector<S_Route>> &>(dynamicBuildRoutes).insert(std::pair<std::string, std::vector<S_Route>>((*buildRoutes_iter)["direction"].asString(), buildRoute));
    }
}

void MatchStaticBuildRoute(const std::vector<S_Route> dynamicBuildRoute, const std::string projectPath, const Json::Value &menuUnit){
    Json::Value route_dynamic, routeUnit, route_static;
    std::vector<std::string> buildRoutes;
    std::string buildRoutesDir_path;
    bool isStaticBuildRouteExist = false;
    for(std::vector<S_Route>::iterator buildRoute_iter = const_cast<std::vector<S_Route> &>(dynamicBuildRoute).begin(); buildRoute_iter != dynamicBuildRoute.end(); buildRoute_iter++){
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
                    const_cast<Json::Value &>(menuUnit)["buildRoute"]["from"] = (matchStep < 1) ? (FROM_PRIVATE) : (FROM_PUBLIC);
                    const_cast<Json::Value &>(menuUnit)["buildRoute"]["route"] = StringSplit((*buildRoutes),".")[-2];
                    isStaticBuildRouteExist = true;
                    break;
                }
            }
        }
    }
    if(isStaticBuildRouteExist == false){
        N_File::C_File(projectPath + "\\" + g_privatePath_buildRoute + "\\" + menuUnit["name"].asString() + "_used.json").Write(route_dynamic);
        menuUnit["buildRoute"]["from"] = FROM_PRIVATE;
        menuUnit["buildRoute"]["route"] = menuUnit["name"].asString() + "_used";
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
    Json::Value program;
    for(std::vector<C_Program*>::iterator targetPrograms_iter = const_cast<std::vector<C_Program*> &>(targetPrograms).begin(); targetPrograms_iter != targetPrograms.end(); targetPrograms_iter++){
        program["name"] = (*targetPrograms_iter)->m_name;
        program["buildInstruct"] = (*targetPrograms_iter)->m_buildInstruct;
        //Matches the static storage of program's buildRoute which is in the global programPool.
        MatchStaticBuildRoute((*targetPrograms_iter)->m_buildRoute, attachedProject.m_path, program);

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
    <std::vector<C_Project*> sameProjects;
    g_programPool.QueryProject(&cProject,sameProjects);
    if(sameProjects.size() != 0){
        for(std::vector<C_Project*>::iterator sameProjects_iter = sameProjects.begin(); sameProjects_iter != sameProjects.end(); sameProjects_iter++){
            g_programPool.RemoveProject(*sameProjects_iter);
        }
    }
    g_programPool.AddProject(cProject);
}

void OutStaticProject(const std::vector<C_Project*> targetProjects, std::vector<Json::Value> &projects){
    Json::Value project;
    for(std::vector<C_Project*>::iterator targetProjects_iter = targetProjects.begin(); targetProjects_iter != targetProjects.end(); targetProjects_iter++){
        project["name"] = (*targetProjects_iter)->m_name;
        project["path"] = (*targetProjects_iter)->m_path;
        project["buildInstruct"] = (*targetProjects_iter)->m_buildInstruct;
        //Matches the static storage of project's buildRoute which is in the global programPool.
        MatchStaticBuildRoute((*targetProjects_iter)->m_buildRoute, (*targetProjects_iter)->m_path, project);

        projects.push_back(project);
    }
}

bool MatchProjectPath(const std::string &path){
    Json::Value projectPaths;
    std::vector<std::string> splitProjectPathArray, splitPathArray, splitPathArray_specifiedDepth;
    std::string path_specifiedDepth;
    N_File::C_File projectPaths_json(g_selfPath + "\\" + "projects.json");
    projectPaths_json.Read(projectPaths);
    int projectPaths_index = 0;
    for(Json::Value::iterator projectPaths_iter = projectPaths.begin(); projectPaths_iter != projectPaths.end(); projectPaths_iter++){
        splitProjectPathArray = StringSplit(currentProjectPath,"\\");
        splitPathArray = StringSplit(path,"\\");
        for(int i=0; i<splitProjectPathArray.size(); i++){
            splitPathArray_specifiedDepth.push_back(splitPathArray[i]);
        }
        path_specifiedDepth = StringUnite(splitPathArray_specifiedDepth,"\\");
        if(path_specifiedDepth == currentProjectPath){
            if(N_File::C_Dir(currentProjectPath).isExist() == true){
                const_cast<std::string &>(path) = currentProjectPath;
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
    cProject = &cProjects[0];
    std::vector<std::string> menus = N_File::C_Dir(currentProjectPath + "\\" + g_privatePath_menu).List();
    for(std::vector<std::string>::iterator menus_iter = menus.begin(); menus_iter != menus.end(); menus_iter++){
        if(StringSplit((*menus_iter),".")[(*menus_iter).size() - 1] == "json" && (*menus_iter) != ".self.json"){
            N_File::C_File(currentProjectPath + "\\" + g_privatePath_menu + "\\" + (*menus_iter)).Read(program);
            InDynamicProgram(program, *cProject);
        }
    }
}

void UnInstallProgramPool(const std::string currentProjectPath){
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
    cProject = &cProjects[0];
    cProject->QueryProgram(NULL,cPrograms);
    OutStaticProgram(cPrograms, *cProject, programs_json);
    for(std::vector<Json::Value>::iterator programs_json_iter = programs_json.begin(); programs_json_iter != programs_json.end(); programs_json_iter++){
        N_File::C_File(currentProjectPath + "\\" + g_privatePath_menu + "\\" + (*programs_json_iter)["name"].asString() + ".json").Write(*programs_json_iter);
    }
}