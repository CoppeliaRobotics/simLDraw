cmake_minimum_required(VERSION 3.16.3)
project(simLDraw)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MACOSX_RPATH 1)

if(NOT COPPELIASIM_INCLUDE_DIR)
    if(DEFINED ENV{COPPELIASIM_ROOT_DIR})
        set(COPPELIASIM_INCLUDE_DIR $ENV{COPPELIASIM_ROOT_DIR}/programming/include)
    else()
        set(COPPELIASIM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    endif()
endif()
list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
    ${COPPELIASIM_INCLUDE_DIR}/cmake)
find_package(CoppeliaSim 4.5.1.7 REQUIRED)
find_package(Eigen3 REQUIRED)

include(FetchContent)
FetchContent_Declare(ldrawloader
    GIT_REPOSITORY https://github.com/fferri/ldrawloader
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/ldrawloader
)
FetchContent_GetProperties(ldrawloader)
if(NOT ldrawloader_POPULATED)
    FetchContent_Populate(ldrawloader)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated)
include_directories(${ldrawloader_SOURCE_DIR}/src)

coppeliasim_generate_stubs(${CMAKE_CURRENT_BINARY_DIR}/generated XML_FILE ${CMAKE_CURRENT_SOURCE_DIR}/sourceCode/callbacks.xml LUA_FILE ${CMAKE_CURRENT_SOURCE_DIR}/lua/simLDraw.lua)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sourceCode/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

coppeliasim_add_plugin(simLDraw SOURCES sourceCode/plugin.cpp ${ldrawloader_SOURCE_DIR}/src/ldrawloader.cpp)
target_link_libraries(simLDraw PRIVATE Eigen3::Eigen)
coppeliasim_add_addon("addOns/LDraw importer.lua")
