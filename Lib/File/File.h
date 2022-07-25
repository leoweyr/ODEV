#pragma once

#include "direct.h"
#include "io.h"
#include "unistd.h"

#include <fstream>

#include "../../Lib/Jsoncpp/Jsoncpp.h"
#include "../../Lib/Bitscode/Bitscode.h"
#include <windows.h>

#pragma comment(lib,"../../Lib/Jsoncpp/Jsoncpp.lib")
#pragma comment(lib,"../../Lib/Bitscode/Bitscode.lib")

#define CURRENT_EXE_PATH N_File::GetCurrentExePath()
#define CURRENT_WORKING_DIRECTORY_PATH N_File::GetCurrentWorkingDirectoryPath()

/*
#include <string>
#include <vector>
 */

namespace N_File{
    class C_File{
        private:
            std::string m_path;
            std::string m_dirPath;

        public:
            C_File(const std::string path);
            bool Read(std::string &content);
            bool Read(Json::Value &content);
            void Write(const std::string content);
            void Write(const Json::Value content);
    };

    class C_Dir{
        public:
            std::string m_path;
            C_Dir(const std::string path);
            bool isExist();
            std::vector<std::string> List();
    };

    std::string GetCurrentExePath();
    std::string GetCurrentWorkingDirectoryPath();
}