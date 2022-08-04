#include "AddonAPI.h"

std::string SetBuildPath(BUILDINSTRUCT buildInstruct){
    std::string buildPath = buildInstruct["projectPath"].asString() + "\\" + "built" + "\\" + buildInstruct["name"].asString();
    N_File::C_Dir(buildPath).isExist(1);
    return buildPath;
}