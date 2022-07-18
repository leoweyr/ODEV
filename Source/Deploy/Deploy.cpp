#include "Deploy.h"

g_publicPath = "public";
g_publicPath_buildWay = g_publicPath + "\\" + "buildWay";
g_publicPath_buildRoute = g_publicPath + "\\" + "buildRoute";
g_privatePath = ".ODEV";
g_privatePath_buildWay = g_privatePath + "\\" + "graph\\way";
g_privatePath_buildRoute = g_privatePath + "\\" + "graph\\route";
g_privatePath_menu = g_privatePath + "\\" + "graph\\menu";

void MatchStaticBuildRoute(const std::vector<S_Route> dynamicBuildRoute, const std::string projectPath, Json::Value &menuUnit){
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
        N_File::C_Dir buildRoutesDir(buildRoutesDir_path);
        buildRoutes = buildRoutesDir.List();
        for(std::vector<std::string>::iterator buildRoutes_iter = buildRoutes.begin(); buildRoutes_iter != buildRoutes.end(); buildRoutes_iter++){
            if(StringSplit((*buildRoutes_iter),".")[-1] == "json"){
                N_File::C_File buildRoute(buildRoutesDir_path + "\\" + (*buildRoutes_iter));
                buildRoute.Read(route_static);
                if(route_static == route_dynamic){
                    menuUnit["buildRoute"]["from"] = (matchStep < 1) ? ("private") : ("public");
                    menuUnit["buildRoute"]["route"] = StringSplit((*buildRoutes),".")[-2];
                    isStaticBuildRouteExist = true;
                    break;
                }
            }
        }
    }
    if(isStaticBuildRouteExist == false){
        N_File::C_File staticBuildRoute(projectPath + "\\" + g_privatePath_buildRoute + "\\" + menuUnit["name"].asString() + "_used.json");
        staticBuildRoute.Write(route_dynamic);
        menuUnit["buildRoute"]["from"] = "private";
        menuUnit["buildRoute"]["route"] = menuUnit["name"].asString() + "_used";
    }
}

void InDynamicProgram(const Json::Value program, C_Project &attachedProject){
    C_Program cProgram(attachedProject);
    cProgram.m_name = program["name"];
    if(program.isMember("buildInstruct") == true){
        cProgram.m_buildInstruct = program["buildInstruct"];
    }
    if(program.isMember("buildRoute") == true){
        Json::Value route;
        S_Route buildRoute;
        if(program["buildRoute"]["from"] == "public"){
            N_File::C_File file(g_publicPath_buildRoute + "\\" + program["buildRoute"]["route"].asString() + ".json");
            file.Read(route);
        }else if(program["buildRoute"]["from"] == "private"){
            N_File::C_File file(attachedProject.m_path + "\\" + g_privatePath_buildRoute + "\\" + program["buildRoute"]["route"].asString() + ".json");
            file.Read(route);
        }
        for(Json::Value iter = route.begin(); iter != route.end(); iter++){
            buildRoute.from = (*iter)["from"];
            buildRoute.way = (*iter)["way"];
            buildRoute.method = (*iter)["method"];
            cProgram.m_buildRoute.push_back(buildRoute);
        }
    }
    //Entered program replaces the same one which already exists in the attached project of global programPool.
    std::vector<std::vector<C_Program>::iterator> samePrograms;
    attachedProject.QueryProgram(cProgram,samePrograms);
    if(samePrograms.size() != 0){
        for(std::vector<std::vector<C_Program>::iterator>::iterator iter = samePrograms.begin();iter != samePrograms.end();iter++){
            attachedProject.RemoveProgram(*iter);
        }
    }
    attachedProject.AddProgram(cProgram);
}

void OutStaticProgram(std::vector<Json::Value> &programs, C_Project &attachedProject){
    Json::Value program;
    for(std::vector<C_Program>::iterator programs_iter = attachedProject.m_programs.begin(); programs_iter != attachedProject.m_programs.end(); programs_iter++){
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
        Json::Value routes;
        S_Route route;
        if(project["buildRoute"]["from"] == "public"){
            N_File::C_File file(g_publicPath_buildRoute + "\\" + project["buildRoute"]["route"].asString() + ".json");
            file.Read(routes);
        }else if(project["buildRoute"]["from"] == "private"){
            N_File::C_File file(cProject.m_path + "\\" + g_privatePath_buildRoute + "\\" + project["buildRoute"]["route"].asString() + ".json");
            file.Read(routes);
        }
        for(Json::Value routes_iter = routes.begin(); routes_iter != routes.end(); routes_iter++){
            route.from = (*routes_iter)["from"];
            route.way = (*routes_iter)["way"];
            route.method = (*routes_iter)["method"];
            cProject.m_buildRoute.push_back(route);
        }
    }
    //Enter programs of project.
    N_File::C_Dir menu(cProject.m_path + "\\" + g_privatePath_menu);
    std::vector<std::string> menus = menu.List();
    Json::Value program;
    for(std::vector<std::string>::iterator menus_iter = menus.begin();menus_iter != menus.end();menus_iter++){
        if(StringSplit((*menus_iter),".") == "json" && (*menus_iter) != ".self.json"){
            N_File::C_File programMenu(cProject.m_path + "\\" + g_privatePath_menu + "\\" + (*menus_iter));
            programMenu.Read(program);
            InDynamicProgram(program, cProject);
        }
    }
    //Entered project replaces the same one which already exists in global programPool.
    std::vector<std::vector<C_Project>::iterator> sameProjects;
    attachedProject.QueryProgram(cProject,sameProjects);
    if(sameProjects.size() != 0){
        for(std::vector<std::vector<C_Project>::iterator>::iterator sameProjects_iter = sameProjects.begin(); sameProjects_iter != sameProjects.end(); sameProjects_iter++){
            g_programPool.RemoveProgram(*sameProjects_iter);
        }
    }
    g_programPool.AddProgram(cProgram);
}

void OutStaticProject(std::vector<Json::Value> &projects){
    Json::Value project;
    for(std::vector<C_Project>::iterator projects_iter = g_programPool.m_projects.begin(); projects_iter != g_programPool.m_projects.end();projects_iter++){
        project["name"] = (*projects_iter).m_name;
        project["path"] = (*projects_iter).m_path;
        project["buildInstruct"] = (*projects_iter).m_buildInstruct;
        //Matches the static storage of project's buildRoute which is in the global programPool.
        MatchStaticBuildRoute((*projects_iter).m_buildRoute,(*projects_iter).m_path,project);

        projects.push_back(project);
    }
}