macro(enable_cppcheck WARNINGS_AS_ERRORS CPPCHECK_OPTIONS)
    find_program(CPPCHECK cppcheck)
    if(NOT CPPCHECK)
        message(WARNING "cppcheck not found")
        return()
    endif()

    if(CMAKE_GENERATOR MATCHES ".*Visual Studio.*")
        set(CPPCHECK_TEMPLATE "vs")
    else()
        set(CPPCHECK_TEMPLATE "gcc")
    endif()

    if("${CPPCHECK_OPTIONS}" STREQUAL "")
        set(CMAKE_CXX_CPPCHECK
            ${CPPCHECK}
            --template=${CPPCHECK_TEMPLATE}
            --enable=style,performance,warning,portability
            --inline-suppr
            --suppress=cppcheckError
            --suppress=internalAstError
            --suppress=unmatchedSuppression
            --suppress=passedByValue
            --suppress=syntaxError
            --suppress=preprocessorErrorDirective
            --inconclusive
        )
    else()
        set(CMAKE_CXX_CPPCHECK ${CPPCHECK} --template=${CPPCHECK_TEMPLATE} ${CPPCHECK_OPTIONS})
    endif()

    if(NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
        set(CMAKE_CXX_CPPCHECK ${CMAKE_CXX_CPPCHECK} --std=c++${CMAKE_CXX_STANDARD})
    endif()

    if(${WARNINGS_AS_ERRORS})
        list(APPEND CMAKE_CXX_CPPCHECK --error-exitcode=2)
    endif()
endmacro()

macro(enable_clang_tidy target WARNINGS_AS_ERRORS)
    find_program(CLANGTIDY clang-tidy)
    if(NOT CLANGTIDY)
        message(WARNING "clang-tidy not found")
        return()
    endif()

    if(NOT CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        get_target_property(TARGET_PCH ${target} INTERFACE_PRECOMPILE_HEADERS)

        if("${TARGET_PCH}" STREQUAL "TARGET_PCH-NOTFOUND")
            get_target_property(TARGET_PCH ${target} PRECOMPILE_HEADERS)
        endif()

        if(NOT "${TARGET_PCH}" STREQUAL "TARGET_PCH-NOTFOUND")
            message(SEND_ERROR "clang-tidy cannot be enabled with non-clang compiler and PCH, clang-tidy fails to handle gcc's PCH file")
        endif()
    endif()

    set(CLANG_TIDY_OPTIONS
        ${CLANGTIDY}
        -extra-arg=-Wno-unknown-warning-option
        -extra-arg=-Wno-ignored-optimization-argument
        -extra-arg=-Wno-unused-command-line-argument
        -p
    )

    if(NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
        if("${CLANG_TIDY_OPTIONS_DRIVER_MODE}" STREQUAL "cl")
            set(CLANG_TIDY_OPTIONS ${CLANG_TIDY_OPTIONS} -extra-arg=/std:c++${CMAKE_CXX_STANDARD})
        else()
            set(CLANG_TIDY_OPTIONS ${CLANG_TIDY_OPTIONS} -extra-arg=-std=c++${CMAKE_CXX_STANDARD})
        endif()
    endif()

    if(${WARNINGS_AS_ERRORS})
        list(APPEND CLANG_TIDY_OPTIONS -warnings-as-errors=*)
    endif()

    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_OPTIONS})
endmacro()