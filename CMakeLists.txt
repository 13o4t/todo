cmake_minimum_required(VERSION 3.21)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed.")
endif()

project(todo LANGUAGES CXX DESCRIPTION "A simple todo list app.")

include(cmake/ProjectOptions.cmake)
include(Dependencies.cmake)

setup_options()
setup_dependencies()

include(cmake/GitVersion.cmake)
generate_version_file(${PROJECT_NAME} ${PROJECT_DESCRIPTION})

add_subdirectory(src)

setup_all_project()

if(NOT PROJECT_IS_TOP_LEVEL)
    return()
endif()

if(BUILD_TESTING)
    message(STATUS "Build test")
    add_subdirectory(test)
endif()

option(BUILD_PLUGINS "Build plugins" OFF)
if(BUILD_PLUGINS)
    message(STATUS "Build plugins")
    add_subdirectory(plugins)
endif()

include(cmake/ProjectPackage.cmake)
package_project(
    TARGETS todo task ${plugins}
    VERSION ${${PROJECT_NAME}_VERSION_STRING}
    CONTACT "jet.meng@outlook.com"
    INCLUDE_DIR include
    LICENSE_FILE "${CMAKE_SOURCE_DIR}/LICENSE"
    DEPENDENCIES "fmt spdlog"
    WITH_FIND_PACKAGE YES
)

message(STATUS "Project version: " ${${PROJECT_NAME}_VERSION_STRING})