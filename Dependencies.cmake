include(cmake/CPM.cmake)

function(setup_dependencies)

    if (NOT TARGET fmtlib::fmtlib)
        CPMAddPackage("gh:fmtlib/fmt#10.1.1")
    endif()

    if(NOT TARGET spdlog::spdlog)
        CPMAddPackage(
            NAME spdlog
            VERSION 1.12.0
            GITHUB_REPOSITORY "gabime/spdlog"
            OPTIONS "SPDLOG_FMT_EXTERNAL ON"
        )
    endif()

    if(NOT TARGET CLI11::CLI11)
        CPMAddPackage("gh:CLIUtils/CLI11@2.3.2")
    endif()

    if(BUILD_TESTING AND NOT TARGET Catch2::Catch2WithMain)
        CPMAddPackage("gh:catchorg/Catch2@3.3.2")
    endif()

endfunction()