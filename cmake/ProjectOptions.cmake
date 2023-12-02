include(CMakeDependentOption)
include(${CMAKE_CURRENT_LIST_DIR}/Cache.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/CompilerWarnings.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Coverage.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/IPO.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Sanitizers.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/StaticAnalyzers.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Utils.cmake)

macro(support_sanitizers)
    if((CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*") AND NOT WIN32)
        set(SUPPORTS_UBSAN ON)
    else()
        set(SUPPORTS_UBSAN OFF)
    endif()

    if((CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*") AND WIN32)
        set(SUPPORTS_ASAN OFF)
    else()
        set(SUPPORTS_ASAN ON)
    endif()
endmacro()

macro(init_options)
    support_sanitizers()

    option(BUILD_SHARED_LIBS "Build shared libs" ON)
    option(BUILD_TESTING "Build tests" ON)

    if(NOT PROJECT_IS_TOP_LEVEL OR PACKAGING_MODE)
        option(WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
        option(ENABLE_CACHE "Enable ccache" OFF)
        option(ENABLE_IPO "Enable IPO" OFF)
        option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
        option(ENABLE_CPPCHECK "Enable cpp-check" OFF)

        option(USE_SANITIZER_ADDRESS "Use address sanitizers" OFF)
        option(USE_SANITIZER_LEAK "Use leak sanitizers" OFF)
        option(USE_SANITIZER_UNDEFINED_BEHAVIOR "Use undefined behavior sanitizers" OFF)
        option(USE_SANITIZER_THREAD "Use thread sanitizers" OFF)
        option(USE_SANITIZER_MEMORY "Use memory sanitizers" OFF)

        option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
    else()
        option(WARNINGS_AS_ERRORS "Treat warnings as errors" ON)
        option(ENABLE_CACHE "Enable ccache" ON)
        option(ENABLE_IPO "Enable IPO" ON)
        option(ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
        option(ENABLE_CPPCHECK "Enable cpp-check" ON)

        option(USE_SANITIZER_ADDRESS "Use address sanitizers" ${SUPPORTS_ASAN})
        option(USE_SANITIZER_LEAK "Use leak sanitizers" ON)
        option(USE_SANITIZER_UNDEFINED_BEHAVIOR "Use undefined behavior sanitizers" ${SUPPORTS_UBSAN})
        option(USE_SANITIZER_THREAD "Use thread sanitizers" OFF)
        option(USE_SANITIZER_MEMORY "Use memory sanitizers" OFF)

        option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
    endif()

endmacro()

function(setup_project_warnings target)
    setup_warnings(${target} ${WARNINGS_AS_ERRORS})
endfunction()

function(setup_project_analyzer target)
    enable_sanitizers(${target}
        ${USE_SANITIZER_ADDRESS} 
        ${USE_SANITIZER_LEAK} 
        ${USE_SANITIZER_UNDEFINED_BEHAVIOR}
        ${USE_SANITIZER_THREAD}
        ${USE_SANITIZER_MEMORY}
    )

    if(${ENABLE_COVERAGE})
        enable_coverage(${target})
    endif()
endfunction()

function(setup_clang_tidy target)
    if(ENABLE_CLANG_TIDY)
        enable_clang_tidy(${target} ${WARNINGS_AS_ERRORS})
    endif()
endfunction()

function(setup_cxx_standard target)
    set_target_properties(${target} PROPERTIES CXX_STANDARD 20)
endfunction()

function(setup_project target)
    setup_cxx_standard(${target})
    setup_project_warnings(${target})
    setup_project_analyzer(${target})
    setup_clang_tidy(${target})
endfunction()

function(setup_all_project)
    get_all_targets(targets)
    foreach(target ${targets})
        get_target_property(source_dir ${target} SOURCE_DIR)
        string(FIND "${source_dir}" "${CMAKE_BINARY_DIR}" res)
        if("${res}" EQUAL 0)
            continue()
        endif()
        setup_project(${target})
    endforeach()
endfunction()


function(setup_options)
    init_options()

    if(ENABLE_CACHE)
        enable_cache()
    endif()

    if(ENABLE_IPO)
        enable_ipo()
    endif()

    if(ENABLE_CPPCHECK)
        enable_cppcheck(${WARNINGS_AS_ERRORS} "")
    endif()
endfunction()
