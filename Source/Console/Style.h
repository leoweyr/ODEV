#pragma once

#include <iostream>

#include "../../Lib/Stream/Stream.h"

#define CONSOLE_PRINT_TYPE_END 0
#define CONSOLE_PRINT_TYPE_INFO 1
#define CONSOLE_PRINT_TYPE_ERROR 2

#define CONSOLE_PRINT_NORMAL(printType, content, normalPrintCount)                                                                                  \
    for(int isPrint = 0; isPrint < 2; isPrint++){                                                                                                   \
        for(int i = 0; i < 120 && (normalPrintCount == 0 || (printType == CONSOLE_PRINT_TYPE_END && normalPrintCount > 0)) && isPrint == 0; i ++){  \
            if(i == 0 && normalPrintCount == 0){                                                                                                    \
                std::cout << "ODEV";                                                                                                                \
                i += 3;                                                                                                                             \
            }else if (i < 120){                                                                                                                     \
                std::cout << "-";                                                                                                                   \
            }else{                                                                                                                                  \
                std::cout << "-" << PRINT_NONE << std::endl;                                                                                        \
            }                                                                                                                                       \
        }                                                                                                                                           \
        if(isPrint == 0){                                                                                                                           \
            if(printType == CONSOLE_PRINT_TYPE_INFO){                                                                                               \
                std::cout << "\n" << "[INFO] " << content << std::endl;                                                                             \
            }else if(printType == CONSOLE_PRINT_TYPE_ERROR){                                                                                        \
                std::cout << "\n" << PRINT_RED << "[ERROR] " << content << PRINT_NONE<< std::endl;                                                  \
            }                                                                                                                                       \
            normalPrintCount ++;                                                                                                                    \
        }                                                                                                                                           \
    }

#define CONSOLE_PRINT_BUILD(errorCode, buildRoute, from_public, buildRouteStep, type, name, buildRouteStep_max, contain)                 \
    for(int isExecute = 0;isExecute < 2;isExecute++){                                                                                    \
        /*Print dividing line.*/                                                                                                         \
        for(int i=0;i<120;i++){                                                                                                          \
            if(i == 0 && isExecute == 0){                                                                                                \
                std::cout << "BEGIN";                                                                                                    \
                i += 4;                                                                                                                  \
            }else if(i == 0 && errorCode == 1){                                                                                          \
                std::cout << PRINT_RED << "BLOCK";                                                                                       \
                i += 4;                                                                                                                  \
            }else if(i == 0 && isExecute == 1){                                                                                          \
                std::cout << "END";                                                                                                      \
                i += 2;                                                                                                                  \
            }else if(i < 120){                                                                                                           \
                std::cout << "-";                                                                                                        \
            }else{                                                                                                                       \
                std::cout << "-" << PRINT_NONE << std::endl;                                                                             \
            }                                                                                                                            \
        }                                                                                                                                \
        if(isExecute == 0){                                                                                                              \
            /*Print basic build info, alternative Display Format Schemes: | TYPE | NAME | DIREACTION | ROUNT | STEP | WAY | METHOD |.*/  \
            std::string buildRoute_isPublic = (buildRoute->from == from_public)?("PUB "):("");                                           \
            std::string buildWay_isPublic = (buildRoute->routeSteps[buildRouteStep - 1].from == from_public)?("PUB "):("");              \
            std::cout << "| TYPE: " << type                                                                                              \
            << " | NAME: " << *name                                                                                                      \
            << " | ROUTE: " << buildRoute_isPublic << buildRoute->name << " " << buildRouteStep << "/" << buildRouteStep_max             \
            << " | WAY: " << buildWay_isPublic << buildRoute->routeSteps[buildRouteStep - 1].way                                         \
            << " | METHOD: " << buildRoute->routeSteps[buildRouteStep - 1].method                                                        \
            << " |" << std::endl;                                                                                                        \
        }else{                                                                                                                           \
            break;                                                                                                                       \
        }                                                                                                                                \
        contain                                                                                                                          \
    }