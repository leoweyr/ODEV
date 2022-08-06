#include <iostream>

#include "../Deploy/Deploy.h"

/*
#include <string>

#include "../../Lib/File/File.h"

#include "../ProgramPool/ProgramPool.h"
#include "../AftermathList/AftermathList.h"

#pragma comment(lib,"../../Lib/File/File.lib")
*/

extern C_ProgramPool g_programPool;

int main(int argc, char *argv[]){
    SetGlobalConfig();
    //Check if the current working directory contains an existing project in ODEV.
    std::string currentProjectPath = CURRENT_WORKING_DIRECTORY_PATH;
    if(MatchProjectPath(currentProjectPath) == false){
        //TODO: If the current working directory does not contain an existing project in ODEV.
        ;
    }
    //Required global install operation.
    InstallProgramPool(currentProjectPath);
    InstallAftermathList(currentProjectPath);

    if(argv[1][0] == '-'){
        //TODO: Other command interaction features.
        ;
    }else{ //set build direction
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
    }
    //Required global uninstall operation.
    UninstallProgramPool(currentProjectPath);
    UninstallAftermathList(currentProjectPath);
}