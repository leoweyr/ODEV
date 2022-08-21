#include <iostream>

#include "../Deploy/Deploy.h"

/*
#include <string>

#include "../../Lib/File/File.h"

#include "../ProgramPool/ProgramPool.h"
#include "../AftermathList/AftermathList.h"
#include "Style.h"

#pragma comment(lib,"../../Lib/File/File.lib")
*/

extern C_ProgramPool g_programPool;
extern std::string g_selfPath;
extern std::string g_privatePath;
extern std::string g_privatePath_menu;

int main(int argc, char *argv[]){
    SetGlobalConfig();
    bool isProjectExist = true;
    int normalPrintCount = 0;
    //Check if the current working directory contains an existing project in ODEV.
    std::string currentProjectPath = CURRENT_WORKING_DIRECTORY_PATH;
    if(MatchProjectPath(currentProjectPath) == false){
        isProjectExist = false;
        CONSOLE_PRINT_NORMAL(CONSOLE_PRINT_TYPE_ERROR, "The current path contains few project!", normalPrintCount)
    }
    //Required global install operation.
    if(isProjectExist == true){
        SetCurrentWorkingDirectoryPath(currentProjectPath);
        InstallProgramPool(currentProjectPath);
        InstallAftermathList(currentProjectPath);
        CheckPrivatePath();
    }
    if(argv[1][0] == '-'){
        if(argv[1] == "-new" && argc - 1 == 2){ //link new project
            //Filing new project in ODEV.
            Json::Value projects;
            N_File::C_File projects_json(g_selfPath + "\\projects.json");
            projects_json.Read(projects);
            projects.append(currentProjectPath);
            projects_json.Write(projects);
            //Initialize private path in the local project directory.
            Json::Value project;
            project["name"] = argv[2];
            CheckPrivatePath();
            N_File::C_File(currentProjectPath + "\\" + g_privatePath_menu + "\\.self.json").Write(project);
            CONSOLE_PRINT_NORMAL(CONSOLE_PRINT_TYPE_INFO, "Have linked the current project.", normalPrintCount)
        }else if(argv[1] == "-del" && argc - 1 == 1 && isProjectExist == true){ //Unlink project
            //Remove project from ODEV.
            Json::Value projects;
            N_File::C_File projects_json(g_selfPath + "\\projects.json");
            projects_json.Read(projects);
            int projects_index = 0;
            for(Json::Value::iterator projects_iter = projects.begin(); projects_iter != projects.end(); projects_iter++){
                if((*projects_iter) == currentProjectPath){
                    projects.removeIndex(projects_index,NULL);
                }
                projects_index ++;
            }
            projects_json.Write(projects);
            //Delete private path in the local project directory.
            N_File::C_Dir privatePath(currentProjectPath + "\\" + g_privatePath);
            if(privatePath.isExist() == true){
                privatePath.Delete();
            }
            CONSOLE_PRINT_NORMAL(CONSOLE_PRINT_TYPE_INFO, "Have unlinked the current project.", normalPrintCount)
        }
    }else if(argc - 1 == 2){ //set build direction
        C_Project *project;
        std::vector<C_Project*> projects;
        project->m_path = currentProjectPath;
        g_programPool.QueryProject(project,projects);
        std::vector<Json::Value> aftermaths;
        if(argv[2] == "-self"){ //build the entire project
            Build(projects[0],argv[1]);
        }else{ //build the specified program
            C_Program *program;
            std::vector<C_Program*> programs;
            program->m_name = argv[2];
            projects[0]->QueryProgram(program,programs);
            Build(programs[0],argv[1]);
        }
    }else{
        std::string command;
        std::string space = " ";
        for(size_t argv_index = 1;argv_index < argc; argv_index++){
            command += argv[argv_index] + space;
        }
        CONSOLE_PRINT_NORMAL(CONSOLE_PRINT_TYPE_ERROR, "No command: " + command, normalPrintCount)
    }
    //Required global uninstall operation.
    if(isProjectExist == true){
        UninstallProgramPool(currentProjectPath);
        UninstallAftermathList(currentProjectPath);
    }
    CONSOLE_PRINT_NORMAL(CONSOLE_PRINT_TYPE_END, NULL, normalPrintCount)
}