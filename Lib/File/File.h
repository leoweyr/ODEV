#pragma once

#include "direct.h"
#include "io.h"

#include <fstream>

#include "../../Lib/Jsoncpp/Jsoncpp.h"
#include "../../Lib/Bitscode/Bitscode.h"

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
            bool Read(std::string &content);
            bool Read(Json::Value &content);
            void Write(const std::string content);
            void Write(const Json::Value content);
    };

    class C_Dir{
        private:
            std::string m_path;

        public:
            C_Dir(const std::string path);
            std::vector<std::string> List();
    };
}