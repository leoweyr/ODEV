#include "AftermathFormat_Feedback.h"

AFTERMATHFORMATTER(Feedback)(READ_AFTERMATH aftermath, char *feedback, bool isBlock, std::string stepTitle){
    if(std::strlen(feedback) != 0){
        if(isBlock == true){
            WRITE_AFTERMATH(aftermath)["basic"]["isBlock"] = true;
            WRITE_AFTERMATH(aftermath)["feedback"]["error"][stepTitle] = feedback;
        }else{
            WRITE_AFTERMATH(aftermath)["feedback"]["info"][stepTitle] = feedback;
        }
    }
}