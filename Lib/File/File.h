#pragma once

#include "direct.h"
#include "io.h"

#include <fstream>
#include <string>

#include "../../Lib/Jsoncpp/Jsoncpp.h"
#include "../../Lib/Bitscode/Bitscode.h"

#pragma comment(lib,"../../Lib/Jsoncpp/Jsoncpp.lib")
#pragma comment(lib,"../../Lib/Bitscode/Bitscode.lib")

namespace N_File{
    class C_File{
        private:
            std::string m_path;
            std::string m_dirPath;

        public:
            C_File(const std::string path);
            void Write(const std::string content);
            void Write(const Json::Value content);
            bool Read(std::string &content);
            bool Read(Json::Value &content);
    };
}