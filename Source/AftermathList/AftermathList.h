#pragma once

#include <vector>
#include <string>

#include "../../Lib/Jsoncpp/Jsoncpp.h"
#include "../../Lib/SortData/SortData.h"
#include <windows.h>

#pragma comment(lib,"../../Lib/Jsoncpp/Jsoncpp.lib")
#pragma comment(lib,"../../Lib/SortData/SortData.lib")

#define FROM_PRIVATE 0
#define FROM_PUBLIC 1

extern std::string g_currentProjectPath;
extern std::string g_publicPath_aftermath;
extern std::string g_privatePath_aftermath;

class C_AftermathList;

class C_Aftermath{
    private:
        void (*m_AftermathHandler)(Json::Value);

        friend class C_AftermathList;

    public:
        int m_priority;
        int m_from;
        std::string m_aftermath;
        std::string m_method;
};

class C_AftermathList{
    private:
        std::vector<C_Aftermath> m_aftermaths;

    public:
        void QueryAftermath(const C_Aftermath *condition, std::vector<C_Aftermath*> &results);
        void AddAftermath(const C_Aftermath aftermath);
        void RemoveProgram(const C_Aftermath* aftermath);
        void Adjust();
        void Handle(const Json::Value buildWayReturn);
};