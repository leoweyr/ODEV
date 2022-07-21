#include "Deploy.h"

void SetGlobalConfig(){
    g_selfPath = N_File::C_Dir().m_path;
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
    for(Json::Value::iterator buildRoutes_iter = staticBuildRoutes.begin(); buildRoutes_iter != staticBuildRoutes.end(); buildRoutes_iter++){
        if((*buildRoutes_iter)["from"] == "private"){
            N_File::C_File(projectPath + "\\" + g_privatePath_buildRoute + "\\" + (*buildRoutes_iter)["route"].asString() + ".json").Read(route);
        }else if((*buildRoutes_iter)["from"] == "public"){
            N_File::C_File(g_publicPath_buildRoute + "\\" + (*buildRoutes_iter)["route"].asString() + ".json").Read(route);
        }
        for(Json::Value::iterator route_iter = route.begin(); route_iter != route.end(); route_iter++){
            buildRouteStep.from = (*route_iter)["from"];
            buildRouteStep.way = (*route_iter)["way"];
            buildRouteStep.method = (*route_iter)["method"];
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
    for(std::vector<S_Route>::iterator buildRoute_iter = dynamicBuildRoute.begin(); buildRoute_iter != dynamicBuildRoute.end(); buildRoute_iter++){
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
                    const_cast<Json::Value &>(menuUnit)["buildRoute"]["from"] = (matchStep < 1) ? ("private") : ("public");
                    const_cast<Json::Value &>(menuUnit)["buildRoute"]["route"] = StringSplit((*buildRoutes),".")[-2];
                    isStaticBuildRouteExist = true;
                    break;
                }
            }
        }
    }
    if(isStaticBuildRouteExist == false){
        N_File::C_File(projectPath + "\\" + g_privatePath_buildRoute + "\\" + menuUnit["name"].asString() + "_used.json").Write(route_dynamic);
        menuUnit["buildRoute"]["from"] = "private";
        menuUnit["buildRoute"]["route"] = menuUnit["name"].asString() + "_used";
    }
}

void InDynamicProgram(const Json::Value program, const C_Project &attachedProject){
    C_Program cProgram(const_cast<C_Project &>(attachedProject));
    cProgram.m_name = program["name"];
    if(program.isMember("buildInstruct") == true){
        cProgram.m_buildInstruct = program["buildInstruct"];
    }
    if(program.isMember("buildRoutes") == true){
        InDynamicBuildRoutes(attachedProject.m_path, program["buildRoutes"], cProgram.m_buildRoutes);
    }
    //Entered program replaces the same one which already exists in the attached project of global programPool.
    std::vector<std::vector<C_Program>::iterator> samePrograms;
    const_cast<C_Project &>(attachedProject).QueryProgram(&cProgram,samePrograms);
    if(samePrograms.size() != 0){
        for(std::vector<std::vector<C_Program>::iterator>::iterator iter = samePrograms.begin();iter != samePrograms.end();iter++){
            const_cast<C_Project &>(attachedProject).RemoveProgram(*iter);
        }
    }
    const_cast<C_Project &>(attachedProject).AddProgram(cProgram);
}

void OutStaticProgram(const std::vector<std::vector<C_Program>::iterator> targetPrograms, const C_Project &attachedProject, std::vector<Json::Value> &programs){
    Json::Value program;
    std::vector<C_Program>::iterator programs_iter;
    for(std::vector<std::vector<C_Program>::iterator>::iterator programs_iter_iter = targetPrograms.begin(); programs_iter_iter != targetPrograms.m_programs.end(); programs_iter_iter++){
        programs_iter = (*programs_iter_iter);
        program["name"] = (*programs_iter).m_name;
        program["buildInstruct"] = (*programs_iter).m_buildInstruct;
        //Matches the static storage of program's buildRoute which is in the global programPool.
        MatchStaticBuildRoute((*programs_iter).m_buildRoute,attachedProject.m_path, program);

        programs.push_back(program);
    }
}

void InDynamicProject(const Json::Value project){
    C_Project cProject;
    cProject.m_name = project["name"];
    cProject.m_path = project["path"];
    if(project.isMember("buildInstruct") == true){
        cProject.m_buildInstruct = project["buildInstruct"];
    }
    if(project.isMember("buildRoute") == true){
        InDynamicBuildRoutes(cProject.m_path, project["buildRoutes"], cProject.m_buildRoutes);
    }
    //Entered project replaces the same one which already exists in global programPool.
    std::vector<std::vector<C_Project>::iterator> sameProjects;
    g_programPool.QueryProject(&cProject,sameProjects);
    if(sameProjects.size() != 0){
        for(std::vector<std::vector<C_Project>::iterator>::iterator sameProjects_iter = sameProjects.begin(); sameProjects_iter != sameProjects.end(); sameProjects_iter++){
            g_programPool.RemoveProject(*sameProjects_iter);
        }
    }
    g_programPool.AddProject(cProject);
}

void OutStaticProject(const std::vector<std::vector<C_Project>::iterator> targetProjects, std::vector<Json::Value> &projects){
    Json::Value project;
    std::vector<C_Project>::iterator projects_iter;
    for(std::vector<std::vector<C_Project>::iterator>::iterator projects_iter_iter = g_programPool.m_projects.begin(); projects_iter_iter != g_programPool.m_projects.end();projects_iter_iter++){
        projects_iter = (*projects_iter_iter);
        project["name"] = (*projects_iter).m_name;
        project["path"] = (*projects_iter).m_path;
        project["buildInstruct"] = (*projects_iter).m_buildInstruct;
        //Matches the static storage of project's buildRoute which is in the global programPool.
        MatchStaticBuildRoute((*projects_iter).m_buildRoute,(*projects_iter).m_path,project);

        projects.push_back(project);
    }
}

void InstallProgramPool(){
    SetGlobalConfig();
    //Enter projects.
    Json::Value projectPaths, project, program;
    std::vector<std::string> menus;
    std::vector<std::vector<C_Project>::iterator> project_iters;
    N_File::C_File(g_selfPath + "\\" + "projects.json").Read(projectPaths);
    for(Json::Value::iterator projectPaths_iter = projectPaths.begin(); projectPaths_iter != projectPaths.end(); projectPaths_iter++){
        if(N_File::C_Dir((*projectPaths_iter).asString()).isExist() == true){
            N_File::C_File((*projectPaths_iter).asString() + "\\" + g_privatePath_menu + "\\" + ".self.json").Read(project);
            project["path"] = (*projectPaths_iter).asString();
            InDynamicProject(project);
            //Enter programs of project.
            C_Project cProject;
            cProject.m_path = project["path"];
            g_programPool.QueryProject(&cProject,project_iters);
            cProject = (*project_iter[0]);
            menus = N_File::C_Dir((*projectPaths_iter).asString() + "\\" + g_privatePath_menu).List();
            for(std::vector<std::string>::iterator menus_iter = menus.begin();menus_iter != menus.end();menus_iter++){
                if(StringSplit((*menus_iter),".") == "json" && (*menus_iter) != ".self.json"){
                    N_File::C_File((*projectPaths_iter).asString() + "\\" + g_privatePath_menu + "\\" + (*menus_iter)).Read(program);
                    InDynamicProgram(program, cProject);
                }
            }
        }else{
            //TODO:If the project does not exist.
        }
    }
}

void UnInstallProgramPool(){
    //Outer projects.
    std::vector<std::vector<C_Project>::iterator> projects_iters, project_iters;
    std::vector<std::vector<C_Program>::iterator> programs_iters;
    std::vector<Json::Value> projects, programs;
    Json::Value projectPaths;
    std::string projectPath;
    g_programPool.QueryProject(NULL, projects_iters);
    OutStaticProject(projects_iters, projects);
    for(std::vector<Json::Value>::iterator projects_iter = projects.begin(); projects_iters != projects.end(); projects_iters++){
        projectPath = (*projects_iter)["path"].asString();
        projectPaths.append(projectPath);
        (*projects_iter).removeMember("path");
        N_File::C_File(projectPath + "\\" + g_privatePath_menu + "\\" + ".self.json").Write(*projects_iter);
        //Outer programs of project.
        C_Project cProject;
        cProject.m_path = projectPath;
        g_programPool.QueryProject(&cProject,project_iters);
        cProject = (*project_iters[0]);
        cProject.QueryProgram(NULL,programs_iters)
        OutStaticProgram(programs_iters, cProject, programs);
        for(std::vector<Json::Value>::iterator programs_iter = programs.begin(); programs_iter != programs.end(); programs_iter++){
            N_File::C_File(projectPath + "\\" + g_privatePath_menu + "\\" + (*programs_iter)["name"].asString() + ".json").Write(*programs_iter);
        }
    }
    N_File::C_File(g_selfPath + "\\" + "projects.json").Write(projectPaths);
}