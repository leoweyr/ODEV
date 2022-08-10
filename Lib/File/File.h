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
            bool isExist();
            bool Read(std::string &content);
            bool Read(Json::Value &content);
            void Write(const std::string content);
            void Write(const Json::Value content);
            bool Delete();
            int Copy(const std::string destPath);
            int Move(const std::string destPath);
    };

    class C_Dir{
        public:
            std::string m_path;
            C_Dir(const std::string path);
            bool isExist(const int mode = 0);
            std::vector<std::string> List();
    };
}