#include "Feedback.h"

AFTERMATHHANDLER Info(AFTERMATH aftermath){
    if(aftermath.isMember("feedback")){
        if(aftermath["feedback"].isMember("info")){
            Json::Value::Members infoKeys = aftermath["feedback"]["info"].getMemberNames();
            Json::Value::iterator info_iter = ITER_AFTERMATH(aftermath)["feedback"]["info"].begin();
            for(Json::Value::Members::iterator infoKeys_iter = infoKeys.begin(); infoKeys_iter != infoKeys.end(); infoKeys_iter++,info_iter++){
                std::cout << PRINT_YELLOW << "[INFO # " << (*infoKeys_iter) << "]" << PRINT_NONE << std::endl;
                std::cout << (*info_iter) << std::endl;
            }
        }
    }
}

AFTERMATHHANDLER Error(AFTERMATH aftermath){
    if(aftermath.isMember("feedback")){
        if(aftermath["feedback"].isMember("error")){
            Json::Value::Members errorKeys = aftermath["feedback"]["error"].getMemberNames();
            Json::Value::iterator error_iter = ITER_AFTERMATH(aftermath)["feedback"]["error"].begin();
            for(Json::Value::Members::iterator errorKeys_iter = errorKeys.begin(); errorKeys_iter != errorKeys.end(); errorKeys_iter++,error_iter++){
                std::cout << PRINT_RED << "[ERROR # " << (*errorKeys_iter) << "]" << PRINT_NONE << std::endl;
                std::cout << (*error_iter) << std::endl;
            }
        }
    }
}