cmake_minimum_required(VERSION 3.21)

project(plugins LANGUAGES CXX)

set(sources src/example_plugin.cpp)

foreach(file ${sources})
    get_filename_component(libname ${file} NAME_WLE)
    list(APPEND plugins ${libname})
    add_library(${libname} SHARED ${file})
    target_include_directories(${libname} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
    set_target_properties(${libname}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src/todo/plugins"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src/todo/plugins"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/src/todo/plugins"
    )
    target_link_libraries(${libname} PRIVATE todo::task)
    setup_project(${libname})
endforeach()

install(TARGETS ${plugins} DESTINATION ${CMAKE_INSTALL_BINDIR}/plugins)