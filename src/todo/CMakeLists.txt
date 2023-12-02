set(sources 
    main.cpp
    command/config.hpp
    command/manager.hpp
    command/list.hpp
)

add_executable(todo ${sources})

target_link_libraries(
    todo
    PRIVATE 
        todo::task
        fmt::fmt
        spdlog::spdlog
        CLI11::CLI11
)

target_include_directories(todo PRIVATE "${CMAKE_BINARY_DIR}")

if(WIN32)
    add_custom_command(
        TARGET todo
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:todo> $<TARGET_FILE_DIR:todo>
        COMMAND_EXPAND_LISTS
    )
endif()