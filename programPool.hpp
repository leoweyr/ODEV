#pragma once

#include <iostream>
#include <string>

#include "../Frame/Underlayer/Bitscode.hpp"
#include "../Frame/Json/json.h"

#pragma comment(lib,"../Frame/Json/jsoncpp.lib")

//#include "../Frame/Python/Python.h"

class Program{
    public:
        std::string Name;
        std::string Extension;
        std::string CompiledInto;
        std::string Tag;
        std::string DevClass;
        std::string LogicPath;
        std::string Type;
        std::string Required;
        std::string FileName;

        Program(char* name,char* extension,char* compiledInto,char* tag,char* devClass,char* logicPath,char* type,char* required){
            Name = name;
            Extension = extension;
            CompiledInto = compiledInto;
            Tag = tag;
            DevClass = devClass;
            LogicPath = logicPath;
            Type = type;
            Required = required;
            FileName = Name + "." + Extension;
        };
};

void InsertPool_json(const char* programObject_json){ //Json structure of programPool: [{%programName%:[{%extension%, %compiledInto%, %tag%, %devClass%, %logicPath%, %type%, %required%}]}]
    Json::Reader reader;
    Json::Value programObjectList_json;
    reader.parse(programObject_json,programObjectList_json);
    std::cout << programObjectList_json["a"] << std::endl;
};

/*
int main(){
    InsertPool_json("[{\"a\":\"b\"}]");
    return 0;
}
*/