cmake_minimum_required(VERSION 3.22)

project(ODEV)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC_FILES_ODEV
        "Source/Console/*.h"
        "Source/Console/*.cpp"
        "Source/Deploy/*.h"
        "Source/Deploy/*.cpp"
        "Source/ProgramPool/*.h"
        "Source/ProgramPool/*.cpp"
        "Source/AftermathList/*.h"
        "Source/AftermathList/*.cpp"
        "Source/lib_template_SortData/*.h"
        "Source/lib_template_SortData/*.cpp")

add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES_ODEV})

target_link_libraries(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCE_DIR}/Lib/Bitscode/Bitscode.lib)
target_link_libraries(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCE_DIR}/Lib/Jsoncpp/Jsoncpp.lib)
target_link_libraries(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCE_DIR}/Lib/File/File.lib)

#subProject AddonSDK
add_subdirectory(AddonSDK)

#subProjects ExampleBuildWay
add_subdirectory(ExampleBuildWay/MingwCompiler)