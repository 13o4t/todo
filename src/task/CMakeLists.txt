set(sources parser.cpp date.cpp filter.cpp manager.cpp plugin.cpp)

add_library(task ${sources})
add_library(todo::task ALIAS task)

target_include_directories(task PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> 
                                        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

set_target_properties(
    task
    PROPERTIES 
        CXX_VISIBILITY_PRESET hidden 
        VISIBILITY_INLINES_HIDDEN YES
)

target_link_libraries(
    task
    PRIVATE
        fmt::fmt
        spdlog::spdlog
)

include(GenerateExportHeader)
generate_export_header(task EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/task/task_export.h)

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(task PUBLIC SAMPLE_LIBRARY_STATIC_DEFINE)
endif()

