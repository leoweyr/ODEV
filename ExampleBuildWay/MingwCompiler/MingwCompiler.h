#pragma once

#include <cstring>

#include "../../Lib/AddonAPI/AddonAPI.h"
#include "../../Lib/AftermathFormat_Feedback/AftermathFormat_Feedback.h"

#pragma comment(lib, "../../Lib/AddonAPI/AddonAPI.lib")
#pragma comment(lib,"../../Lib/AftermathFormat_Feedback")

/*
#include <string>

#include "../../Lib/Bitscode/Bitscode.h"
#include "../../Lib/System/System.h"

#pragma comment(lib,"../../Lib/Bitscode/Bitscode.lib")
#pragma comment(lib, "../../Lib/System/System.lib")
 */

#define TYPE_C 0
#define TYPE_CPP 1

BUILDMETHOD(Exe);

BUILDMETHOD(Lib);

BUILDMETHOD(Dll);