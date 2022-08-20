#include "MingwCompiler.h"

BUILDMETHOD(Exe){
    std::string buildPath = SetBuildPath(buildInstruct);
    std::string stepTitle;
    std::string cmd;
    char feedback[BUFFER_SIZE];
    bool isBlock = false;
    AFTERMATH aftermath;
    //Get source files.
    std::string sourceFiles;
    std::vector<std::string> sourceFiles_split;
    int sourceType;
    for(BUILDINSTRUCT::iterator source_iter = WRITE_BUILDINSTRUCT(buildInstruct)["basic"]["source"].begin(); source_iter != buildInstruct["basic"]["source"].end(); source_iter++){
        sourceFiles_split = StringSplit((*source_iter).asString(),".");
        if(sourceFiles_split[sourceFiles_split.size() - 1] == "cpp"){ //Determine the type of source files.
            sourceType = TYPE_CPP;
        }
        if(sourceFiles_split[sourceFiles_split.size() - 1] == "cpp" || sourceFiles_split[sourceFiles_split.size() - 1] == "c"){
            sourceFiles += " " + (*source_iter).asString();
        }
    }
    //Get dependency files.
    std::string dependencyFiles;
    std::vector<std::string> dependencyFiles_split;
    for(BUILDINSTRUCT::iterator dependency_iter = WRITE_BUILDINSTRUCT(buildInstruct)["basic"]["dependency"].begin(); dependency_iter != buildInstruct["basic"]["dependency"].end(); dependency_iter++){
        dependencyFiles_split = StringSplit((*dependency_iter).asString(),".");
        if(dependencyFiles_split[dependencyFiles_split.size() - 1] == "lib"){
            dependencyFiles += " " + (*dependency_iter).asString();
        }
    }
    //Compile source files into an executable file.
    stepTitle = "Compile source files into an executable file";
    cmd = (sourceType == TYPE_C)?("gcc" + sourceFiles + dependencyFiles + " -o " + buildPath + "\\" + buildInstruct["basic"]["name"].asString()):("g++" + sourceFiles + dependencyFiles + " -o " + buildPath + "\\" + buildInstruct["basic"]["name"].asString());
    ExeCmd(cmd.data(),feedback);
    //Check if the compilation was successful.
    isBlock = !(N_File::C_File(buildPath + "\\" + buildInstruct["basic"]["name"].asString() + ".exe").isExist());
    AFTERMATHFORMAT(Feedback)(aftermath,feedback,isBlock,stepTitle);
    return aftermath;
}

BUILDMETHOD(Lib){
    std::string buildPath = SetBuildPath(buildInstruct);
    std::string stepTitle;
    std::string cmd;
    char feedback[BUFFER_SIZE];
    bool isBlock = false;
    AFTERMATH aftermath;
    std::string objectFiles;
    //Compile source files into object files.
    stepTitle = "Compile source files into object files";
    std::vector<std::string> sourceFiles_split, sourceFiles_split_name;
    std::string sourceFileName;
    int sourceType;
    for(BUILDINSTRUCT::iterator source_iter = WRITE_BUILDINSTRUCT(buildInstruct)["basic"]["source"].begin(); source_iter != buildInstruct["basic"]["source"].end(); source_iter++){
        sourceFiles_split = StringSplit((*source_iter).asString(),".");
        sourceFiles_split_name = StringSplit(sourceFiles_split[sourceFiles_split.size() - 2],"\\");
        sourceFileName = sourceFiles_split_name[sourceFiles_split_name.size() - 1];
        if(sourceFiles_split[sourceFiles_split.size() - 1] == "cpp"){ //Determine the type of source files.
            sourceType = TYPE_CPP;
        }
        if(sourceFiles_split[sourceFiles_split.size() - 1] == "cpp" || sourceFiles_split[sourceFiles_split.size() - 1] == "c"){
            cmd = (sourceType == TYPE_C)?("gcc -c" + (*source_iter).asString()):("g++ -c" + (*source_iter).asString());
            ExeCmd(cmd.data(),feedback);
            N_File::C_File sourceFileObject(CURRENT_WORKING_DIRECTORY_PATH + sourceFileName + ".o");
            if(sourceFileObject.isExist() == false){
                isBlock = true;
            }else{
                sourceFileObject.Move(buildPath);
                objectFiles += " " + buildPath + "\\" + sourceFileName + ".o";
            }
            AFTERMATHFORMAT(Feedback)(aftermath,feedback,isBlock,stepTitle + " <- " + (*source_iter).asString());
        }
        if(isBlock == true){
            break;
        }
    }
    if(isBlock == true){
        return aftermath;
    }
    //Restore dependent static libraries to object files.
    stepTitle = "Restore dependent static libraries to object files";
    std::vector<std::string> dependencyFiles_split, dependencyFiles_split_name, currentWorkingDirectoryFiles, currentWorkingDirectoryFiles_split;
    std::string dependencyFileName;
    int objectFilesCount;
    for(BUILDINSTRUCT::iterator dependency_iter = WRITE_BUILDINSTRUCT(buildInstruct)["basic"]["dependency"].begin(); dependency_iter != buildInstruct["basic"]["dependency"].end(); dependency_iter++){
        dependencyFiles_split = StringSplit((*dependency_iter).asString(),".");
        dependencyFiles_split_name = StringSplit(dependencyFiles_split[dependencyFiles_split.size() - 2],"\\");
        dependencyFileName = dependencyFiles_split_name[dependencyFiles_split_name.size() - 1];
        if(dependencyFiles_split[dependencyFiles_split.size() - 1] == "lib"){
            cmd = "ar -x " + (*dependency_iter).asString();
            ExeCmd(cmd.data(),feedback);
            currentWorkingDirectoryFiles = N_File::C_Dir(CURRENT_WORKING_DIRECTORY_PATH).List();
            objectFilesCount = 0;
            for(std::vector<std::string>::iterator currentWorkingDirectoryFiles_iter = currentWorkingDirectoryFiles.begin(); currentWorkingDirectoryFiles_iter != currentWorkingDirectoryFiles.end(); currentWorkingDirectoryFiles_iter++){
                currentWorkingDirectoryFiles_split = StringSplit((*currentWorkingDirectoryFiles_iter),".");
                if(currentWorkingDirectoryFiles_split[currentWorkingDirectoryFiles_split.size() - 1] == "o"){
                    N_File::C_File((*currentWorkingDirectoryFiles_iter)).Move(buildPath);
                    objectFiles += " " + buildPath + "\\" + dependencyFileName + ".o";
                    objectFilesCount ++;
                }
            }
            if(objectFilesCount == 0){
                isBlock = true;
            }
            AFTERMATHFORMAT(Feedback)(aftermath,feedback,isBlock,stepTitle + " <- " + (*dependency_iter).asString());
        }
        if(isBlock == true){
            break;
        }
    }
    if(isBlock == true){
        return aftermath;
    }
    //Compile all object files into one static library.
    stepTitle = "Compile all object files into one static library";
    cmd = "ar cqs " + buildPath + "\\" + buildInstruct["basic"]["name"].asString() + ".lib" + objectFiles;
    ExeCmd(cmd.data(),feedback);
    isBlock = !(N_File::C_File(buildPath + "\\" + buildInstruct["basic"]["name"].asString() + ".lib").isExist());
    AFTERMATHFORMAT(Feedback)(aftermath,feedback,isBlock,stepTitle);
    if(isBlock == true){
        return aftermath;
    }
    //Delete all object files.
    std::vector<std::string> objectFiles_split = StringSplit(objectFiles," ");
    for(std::vector<std::string>::iterator objectFiles_split_iter = objectFiles_split.begin(); objectFiles_split_iter != objectFiles_split.end(); objectFiles_split_iter++){
        N_File::C_File objectFile((*objectFiles_split_iter));
        if(objectFile.isExist() == true){
            objectFile.Delete();
        }
    }
    //TODO: Integrate included header files and remove the line<#pragma comment(lib, "...")>.
    return aftermath;
}

BUILDMETHOD(Dll){
    std::string buildPath = SetBuildPath(buildInstruct);
    std::string stepTitle;
    std::string cmd;
    char feedback[BUFFER_SIZE];
    bool isBlock = false;
    AFTERMATH aftermath;
    //Get source files.
    std::string sourceFiles;
    std::vector<std::string> sourceFiles_split;
    int sourceType;
    for(BUILDINSTRUCT::iterator source_iter = WRITE_BUILDINSTRUCT(buildInstruct)["basic"]["source"].begin(); source_iter != buildInstruct["basic"]["source"].end(); source_iter++){
        sourceFiles_split = StringSplit((*source_iter).asString(),".");
        if(sourceFiles_split[sourceFiles_split.size() - 1] == "cpp"){ //Determine the type of source files.
            sourceType = TYPE_CPP;
        }
        if(sourceFiles_split[sourceFiles_split.size() - 1] == "cpp" || sourceFiles_split[sourceFiles_split.size() - 1] == "c"){
            sourceFiles += " " + (*source_iter).asString();
        }
    }
    //Get dependency files.
    std::string dependencyFiles;
    std::vector<std::string> dependencyFiles_split;
    for(BUILDINSTRUCT::iterator dependency_iter = WRITE_BUILDINSTRUCT(buildInstruct)["basic"]["dependency"].begin(); dependency_iter != buildInstruct["basic"]["dependency"].end(); dependency_iter++){
        dependencyFiles_split = StringSplit((*dependency_iter).asString(),".");
        if(dependencyFiles_split[dependencyFiles_split.size() - 1] == "lib"){
            dependencyFiles += " " + (*dependency_iter).asString();
        }
    }
    //Compile source files into a dynamic link library file.
    stepTitle = "Compile source files into a dynamic link library file";
    cmd = (sourceType == TYPE_C)?("gcc" + sourceFiles + dependencyFiles + " -shared -o " + buildPath + "\\" + buildInstruct["basic"]["name"].asString() + ".dll"):("g++" + sourceFiles + dependencyFiles + " -shared -o " + buildPath + "\\" + buildInstruct["basic"]["name"].asString() + ".dll");
    ExeCmd(cmd.data(),feedback);
    //Check if the compilation was successful.
    isBlock = !(N_File::C_File(buildPath + "\\" + buildInstruct["basic"]["name"].asString() + ".dll").isExist());
    AFTERMATHFORMAT(Feedback)(aftermath,feedback,isBlock,stepTitle);
    //TODO: Integrate included header files and remove the line<#pragma comment(lib, "...")>.
    return aftermath;
}