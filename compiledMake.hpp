#pragma once

#include <windows.h>

#include "programPool.hpp"
#include "../Frame/Underlayer/SystemOperate.hpp"

/*
#include <string>
#include "../Frame/Bitscode/Bitscode.hpp"
#include <io.h>
#include <vector>
 */

bool Dll(Program *program,char* compiledPath,char* sourcePath){
    std::string compiledPath_str = compiledPath;
    std::string sourcePath_str = sourcePath;
    std::string compiledFilePath = compiledPath_str + "\\" + program->Name + ".dll";
    if(program->Extension == "c"){
        char debug[1];
        std::string cmd_compiledCDll = "gcc " + sourcePath_str + "\\" + program->FileName + " -shared -o " + compiledPath_str + "\\" + program.Name + ".dll";
        ExeCmd((char*)cmd_compiledCDll.data(),debug);
        if(_access(compiledFilePath.data(),0) == -1){
            File debugLog(compiledPath_str,"error-compiled-dll-" + program->Name + ".log");
            debugLog.Write(debug);
            return false;
        }else{
            return true;
        }
    }else if(program->Extension == "cpp"){
        char debug[1];
        std::string cmd_compiledCppDll = "g++ " + sourcePath_str + "\\" + program->FileName + " -shared -o " + compiledPath_str + "\\" + program.Name + ".dll";
        ExeCmd((char*)cmd_compiledCppDll.data(),debug);
        if(_access(compiledFilePath.data(),0) == -1){
            File debugLog(compiledPath_str,"error-compiled-dll-" + program->Name + ".log");
            debugLog.Write(debug);
            return false;
        }else{
            return true;
        }
    }else if(program->Extension == "py"){
        char debug[1];
        std::string compiledFilePath_pyd = compiledPath_str + "\\" + program->Name + "\\" + program->Name + ".cp37-win_amd64" + ".pyd";
        if(CopyFile(compiledPath_str + "\\" + program->Name,sourcePath_str + "\\" + program->FileName) == 0) {
            return false;
        }
        File pydSetup(compiledPath_str,program->Name + "\\setup.py");
        pydSetup.Write("from distutils.core import setup\nfrom Cython.Build import cythonize\nsetup(ext_modules=cythonize(" + program->FileName + "\"))");
        std::string cmd_compiledPyDll = "python " + compiledPath_str + "\\" + program->Name + "\\setup.py" + " build_ext --inplace";
        ExeCmd((char*)cmd_compiledPyDll.data(),debug);
        bool isPyd = false;
        std::string pydFileName;
        std::vector<std::string> compiledDirFiles = DirList(compiledPath_str + "\\" + program->Name);
        for(std::vector<std::string>::iterator iter = compiledDirFiles.begin();iter != compiledDirFiles.end(); ++iter){
            if(StringSplit(*iter,".")[StringSplit(*iter,".").size() - 1] == "pyd"){
                pydFileName = *iter;
                isPyd = true;
            }
        }
        if(isPyd == false){
            File debugLog(compiledPath_str,"error-compiled-pyd-" + program->Name + ".log")
            debugLog.Write(debug);
            std::string compiledDir = compiledPath_str + "\\" + program->Name;
            RemoveDirectoryA(compiledDir.data());
            return false;
        }else{
            if(CopyFile(compiledPath_str + "\\" + program->Name + ".pyd",compiledPath_str + "\\" + program->Name + "\\" + pydFileName) == false){
                return false;
            }
            if(RemoveDirectoryA(compiledDir.data()) == false){
                return false;
            }
            return true;
        }
    }
}

bool Exe(Program *program,char* compiledPath,char* sourcePath){
    std::string compiledPath_str = compiledPath;
    std::string sourcePath_str = sourcePath;
    std::string compiledFilePath = compiledPath_str + "\\" + program->Name + ".exe";
    if(program->Extension = "c"){
        char debug[1];
        std::string cmd_compiledCExe = "gcc -o" + compiledPath_str + "\\" + program->Name + ".exe" + sourcePath_str + "\\" + program->FileName;
        ExeCmd((char*)cmd_compiledCExe.data(),debug);
        if(_access(compiledFilePath.data(),0) == -1){
            File debugLog(compiledPath_str,"error-compiled-exe-" + program->Name + ".log");
            debugLog.Write(debug);
            return false;
        }else{
            return true;
        }
    }else if(program->Extension = "cpp"){
        char debug[1];
        std::string cmd_compiledCExe = "g++ -o" + compiledPath_str + "\\" + program->Name + ".exe" + sourcePath_str + "\\" + program->FileName;
        ExeCmd((char*)cmd_compiledCExe.data(),debug);
        if(_access(compiledFilePath.data(),0) == -1){
            File debugLog(compiledPath_str,"error-compiled-exe-" + program->Name + ".log");
            debugLog.Write(debug);
            return false;
        }else{
            return true;
        }
    }
}