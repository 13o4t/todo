cmake_minimum_required(VERSION 3.21)

project(tests LANGUAGES CXX)

set(sources main_test.cpp test_utils.hpp)
add_executable(tests ${sources})

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain todo::task)

setup_cxx_standard(tests)
setup_project_warnings(tests)

enable_testing()
include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)
catch_discover_tests(tests)

if(WIN32)
    add_custom_command(
        TARGET tests
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:tests> $<TARGET_FILE_DIR:tests>
        COMMAND_EXPAND_LISTS
    )

    if(USE_SANITIZER_ADDRESS)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(ASAN_DLL_NAME "clang_rt.asan_dbg_dynamic-x86_64.dll")
        else()
            set(ASAN_DLL_NAME "clang_rt.asan_dynamic-x86_64.dll")
        endif()
        get_filename_component(ASAN_DLL_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        add_custom_command(
            TARGET tests
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${ASAN_DLL_DIR}/${ASAN_DLL_NAME} $<TARGET_FILE_DIR:tests>
        )
    endif()
endif()
