#pragma once

#include "direct.h"
#include "io.h"
#include "unistd.h"
#include "sys/stat.h"

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

#define IS_NULL 0
#define IS_FILE 1
#define IS_DIR 2

namespace N_File{
    int Is(const std::string path);

    class C_File{
        private:
            std::string m_path;
            std::string m_dirPath;

        public:
            C_File(const std::string path);
            bool IsExist();
            bool Read(std::string &content);
            bool Read(Json::Value &content);
            void Write(const std::string content);
            void Write(const Json::Value content);
            bool Delete();
            int Copy(const std::string destPath);
            int Move(const std::string destPath);
    };

    class C_Dir{
        private:
            int m_delete_roll = 0;
        public:
            std::string m_path;
            C_Dir(const std::string path);
            bool IsExist(const int mode = 0);
            std::vector<std::string> List();
            bool Delete(const std::string path = "");
    };
}