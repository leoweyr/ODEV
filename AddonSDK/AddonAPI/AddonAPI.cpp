#include "AddonAPI.h"

std::string SetBuildPath(READ_BUILDINSTRUCT buildInstruct){
    std::string buildPath = buildInstruct["projectPath"].asString() + "\\" + "built" + "\\" + buildInstruct["name"].asString();
    N_File::C_Dir(buildPath).IsExist(1);
    return buildPath;
}