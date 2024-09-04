cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)
project(Bachelor-thesis)

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_PREFIX_PATH "home/root-install/cmake")

find_package(ROOT 6.16 CONFIG REQUIRED)

# include necessary files and directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# compile every source and header file in project directory
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# create the main program using the library.
add_executable(Bachelor-thesis main.cpp ${sources} ${headers})

# set C++ standard to C++17
target_compile_features(Bachelor-thesis PUBLIC cxx_std_17)

# add the -Zc:__cplusplus flag
#target_compile_options(Bachelor-thesis PRIVATE /Zc:__cplusplus)
if(MSVC)
    target_compile_options(Bachelor-thesis PRIVATE /Zc:__cplusplus)
else(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(Bachelor-thesis PRIVATE -Wall -Wextra -pedantic)
endif()

# link ROOT libraries to executable
target_link_libraries(Bachelor-thesis PUBLIC ROOT::Core ROOT::Graf ROOT::Hist ROOT::Tree ROOT::Gpad ROOT::RIO)