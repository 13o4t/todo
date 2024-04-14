set(VERSION_TAG_PREFIX v)
set(VERSION_BETA_FLAG beta)
set(VERSION_RELEASE_FLAG rc)
set(VERSION_HOTFIX_FLAG hotfix)
set(RELEASE_BRANCH_PREFIX release)
set(HOTFIX_BRANCH_PREFIX hotfix)

function(get_version_info prefix)
    set(${prefix}_VERSION_MAJOR 0)
    set(${prefix}_VERSION_MINOR 0)
    set(${prefix}_VERSION_PATCH 0)
    set(${prefix}_VERSION_TWEAK 0)
    set(${prefix}_VERSION_BRANCH unknown)
    set(${prefix}_VERSION_FLAG unknown)
    set(${prefix}_VERSION_STRING 0.0.0-unknown)
    set(${prefix}_VERSION_ISDIRTY 0)

    find_package(Git)
    if(NOT GIT_FOUND)
        message(STATUS "Version Info: Git not found.")
        return()
    endif()

    # get git tag
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --match "${VERSION_TAG_PREFIX}[0-9].[0-9]*"
        RESULT_VARIABLE result
        OUTPUT_VARIABLE GIT_TAG_VERSION
        ERROR_VARIABLE error_out
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # extract version major, minor, patch, distance
    if(result EQUAL 0)
        if(GIT_TAG_VERSION MATCHES "^${VERSION_TAG_PREFIX}?([0-9]+)\\.([0-9]+)(\\.([0-9]+))?(-([0-9]+))?.*$")
            set(${prefix}_VERSION_MAJOR ${CMAKE_MATCH_1})
            set(${prefix}_VERSION_MINOR ${CMAKE_MATCH_2})
            if(NOT ${CMAKE_MATCH_4} STREQUAL "")
                set(${prefix}_VERSION_PATCH ${CMAKE_MATCH_4})
            endif()
            if(NOT ${CMAKE_MATCH_6} STREQUAL "")
                set(${prefix}_VERSION_TWEAK ${CMAKE_MATCH_6})
            endif()
        else()
            execute_process(COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
                RESULT_VARIABLE result
                OUTPUT_VARIABLE GIT_DISTANCE
                ERROR_VARIABLE error_out
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            if(result EQUAL 0)
                set(${prefix}_VERSION_TWEAK ${GIT_DISTANCE})
            endif()
        endif()
    endif()

    # check local modifications
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --always --dirty
        RESULT_VARIABLE result
        OUTPUT_VARIABLE GIT_ALWAYS_VERSION
        ERROR_VARIABLE error_out
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(result EQUAL 0)
        if(GIT_ALWAYS_VERSION MATCHES "^.*-dirty$")
            set(${prefix}_VERSION_ISDIRTY 1)
        endif()
    endif()

    # check branch
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        RESULT_VARIABLE result
        OUTPUT_VARIABLE GIT_BRANCH
        ERROR_VARIABLE error_out
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(result EQUAL 0)
        set(${prefix}_VERSION_BRANCH "${GIT_BRANCH}")

        # check in release branch
        string(LENGTH ${RELEASE_BRANCH_PREFIX} PREFIX_LEN)
        string(SUBSTRING ${GIT_BRANCH} 0 ${PREFIX_LEN} COMPARE_PREFIX)
        string(COMPARE EQUAL ${RELEASE_BRANCH_PREFIX} ${COMPARE_PREFIX} ON_RELEASE_BRANCH)
        # check in hotfix branch
        string(LENGTH ${HOTFIX_BRANCH_PREFIX} PREFIX_LEN)
        string(SUBSTRING ${GIT_BRANCH} 0 ${PREFIX_LEN} COMPARE_PREFIX)
        string(COMPARE EQUAL ${HOTFIX_BRANCH_PREFIX} ${COMPARE_PREFIX} ON_HOTFIX_BRANCH)
        # check in master branch
        string(COMPARE EQUAL "master" ${GIT_BRANCH} ON_MASTER)
        # check in HEAD
        string(COMPARE EQUAL "HEAD" ${GIT_BRANCH} ON_HEAD)

        if(ON_RELEASE_BRANCH)
            set(${prefix}_VERSION_FLAG ${VERSION_RELEASE_FLAG})
            set(RELEASE_VERSION_MAJOR 0)
            set(RELEASE_VERSION_MINOR 0)
            set(RELEASE_VERSION_PATCH 0)
            # get release version from release branch name
            if(GIT_BRANCH MATCHES "^${RELEASE_BRANCH_PREFIX}.*([0-9]+)\\.([0-9]+)(\\.([0-9]+))?.*$")
                set(RELEASE_VERSION_MAJOR ${CMAKE_MATCH_1})
                set(RELEASE_VERSION_MINOR ${CMAKE_MATCH_2})
                if(NOT ${CMAKE_MATCH_4} STREQUAL "")
                    set(RELEASE_VERSION_PATCH ${CMAKE_MATCH_4})
                endif()
            endif()

            if("${RELEASE_VERSION_MAJOR}.${RELEASE_VERSION_MINOR}.${RELEASE_VERSION_PATCH}" VERSION_GREATER
                "${${prefix}_VERSION_MAJOR}.${${prefix}_VERSION_MINOR}.${${prefix}_VERSION_PATCH}")
                set(${prefix}_VERSION_MAJOR ${RELEASE_VERSION_MAJOR})
                set(${prefix}_VERSION_MINOR ${RELEASE_VERSION_MINOR})
                set(${prefix}_VERSION_PATCH ${RELEASE_VERSION_PATCH})
            else()
                MATH(EXPR ${prefix}_VERSION_MINOR "${RELEASE_VERSION_MINOR}+1")
                set(${prefix}_VERSION_PATCH 0)
            endif()
        elseif(ON_HOTFIX_BRANCH)
            set(${prefix}_VERSION_FLAG ${VERSION_HOTFIX_FLAG})
            set(HOTFIX_VERSION_MAJOR 0)
            set(HOTFIX_VERSION_MINOR 0)
            set(HOTFIX_VERSION_PATCH 0)
            # get hotfix version from hotfix branch name
            if(GIT_BRANCH MATCHES "^${HOTFIX_BRANCH_PREFIX}.*([0-9]+)\\.([0-9]+)(\\.([0-9]+))?.*$")
                set(HOTFIX_VERSION_MAJOR ${CMAKE_MATCH_1})
                set(HOTFIX_VERSION_MINOR ${CMAKE_MATCH_2})
                if(NOT ${CMAKE_MATCH_4} STREQUAL "")
                    set(HOTFIX_VERSION_PATCH ${CMAKE_MATCH_4})
                endif()
            endif()

            if("${HOTFIX_VERSION_MAJOR}.${HOTFIX_VERSION_MINOR}.${HOTFIX_VERSION_PATCH}" VERSION_GREATER
                "${${prefix}_VERSION_MAJOR}.${${prefix}_VERSION_MINOR}.${${prefix}_VERSION_PATCH}")
                set(${prefix}_VERSION_MAJOR ${HOTFIX_VERSION_MAJOR})
                set(${prefix}_VERSION_MINOR ${HOTFIX_VERSION_MINOR})
                set(${prefix}_VERSION_PATCH ${HOTFIX_VERSION_PATCH})
            else()
                MATH(EXPR ${prefix}_VERSION_PATCH "${{$prefix}_VERSION_PATCH}+1")
            endif()
        elseif(ON_MASTER)
            set(${prefix}_VERSION_FLAG "")
        elseif(ON_HEAD)
            set(${prefix}_VERSION_FLAG "")
        endif()
    endif()

    if(NOT ON_MASTER AND NOT ON_RELEASE_BRANCH AND NOT ON_HOTFIX_BRANCH AND NOT ON_HEAD)
        MATH(EXPR ${prefix}_VERSION_MINOR "${${prefix}_VERSION_MINOR}+1")
        set(${prefix}_VERSION_PATCH 0)
        set(${prefix}_VERSION_FLAG ${VERSION_BETA_FLAG})
    endif()

    if("${${prefix}_VERSION_BRANCH}" STREQUAL "unknown" OR "${${prefix}_VERSION_BRANCH}" STREQUAL "")
        set(${prefix}_VERSION_BRANCH "not-within-git-repo")
    endif()

    # commit hash
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        RESULT_VARIABLE result
        OUTPUT_VARIABLE GIT_SHORT_HASH
        ERROR_VARIABLE error_out
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(result EQUAL 0)
        set(${prefix}_VERSION_SHORTHASH ${GIT_SHORT_HASH})
    else()
        set(${prefix}_VERSION_SHORTHASH "unknown")
    endif()

    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        RESULT_VARIABLE result
        OUTPUT_VARIABLE GIT_FULL_HASH
        ERROR_VARIABLE error_out
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(result EQUAL 0)
        set(${prefix}_VERSION_FULLHASH ${GIT_FULL_HASH})
    else()
        set(${prefix}_VERSION_FULLHASH "unknown")
    endif()

    # build version string
    set(VERSION_STRING "${${prefix}_VERSION_MAJOR}.${${prefix}_VERSION_MINOR}")
    if(NOT ${${prefix}_VERSION_PATCH} EQUAL 0)
        set(VERSION_STRING "${VERSION_STRING}.${${prefix}_VERSION_PATCH}")
    endif()
    if(NOT ${${prefix}_VERSION_FLAG} STREQUAL "")
        set(VERSION_STRING "${VERSION_STRING}-${${prefix}_VERSION_FLAG}")
    endif()
    if(NOT ON_MASTER OR (NOT ON_MASTER AND NOT ${${prefix}_VERSION_TWEAK} EQUAL 0))
        set(VERSION_STRING "${VERSION_STRING}.${${prefix}_VERSION_TWEAK}")
    endif()
    set(${prefix}_VERSION_STRING "${VERSION_STRING}")

    # export
    set(${prefix}_VERSION_STRING ${${prefix}_VERSION_STRING} PARENT_SCOPE)
    set(${prefix}_VERSION_MAJOR ${${prefix}_VERSION_MAJOR} PARENT_SCOPE)
    set(${prefix}_VERSION_MINOR ${${prefix}_VERSION_MINOR} PARENT_SCOPE)
    set(${prefix}_VERSION_PATCH ${${prefix}_VERSION_PATCH} PARENT_SCOPE)
    set(${prefix}_VERSION_BRANCH ${${prefix}_VERSION_BRANCH} PARENT_SCOPE)
    set(${prefix}_VERSION_FLAG ${${prefix}_VERSION_FLAG} PARENT_SCOPE)
    set(${prefix}_VERSION_TWEAK ${${prefix}_VERSION_TWEAK} PARENT_SCOPE)
    set(${prefix}_VERSION_SHORTHASH ${${prefix}_VERSION_SHORTHASH} PARENT_SCOPE)
    set(${prefix}_VERSION_FULLHASH ${${prefix}_VERSION_FULLHASH} PARENT_SCOPE)
endfunction()

function(generate_version_file target description)
    get_version_info("${target}")

    # export
    set(${target}_VERSION_STRING ${${target}_VERSION_STRING} PARENT_SCOPE)
    set(${target}_VERSION_MAJOR ${${target}_VERSION_MAJOR} PARENT_SCOPE)
    set(${target}_VERSION_MINOR ${${target}_VERSION_MINOR} PARENT_SCOPE)
    set(${target}_VERSION_PATCH ${${target}_VERSION_PATCH} PARENT_SCOPE)
    set(${target}_VERSION_BRANCH ${${target}_VERSION_BRANCH} PARENT_SCOPE)
    set(${target}_VERSION_FLAG ${${target}_VERSION_FLAG} PARENT_SCOPE)
    set(${target}_VERSION_TWEAK ${${target}_VERSION_TWEAK} PARENT_SCOPE)
    set(${target}_VERSION_SHORTHASH ${${target}_VERSION_SHORTHASH} PARENT_SCOPE)
    set(${target}_VERSION_FULLHASH ${${target}_VERSION_FULLHASH} PARENT_SCOPE)

    set(TARGET_NAME ${target})
    set(TARGET_DESCRIPTION ${description})
    set(VERSION_STRING ${${target}_VERSION_STRING})
    set(VERSION_MAJOR ${${target}_VERSION_MAJOR})
    set(VERSION_MINOR ${${target}_VERSION_MINOR})
    set(VERSION_PATCH ${${target}_VERSION_PATCH})
    set(VERSION_TWEAK ${${target}_VERSION_TWEAK})
    set(VERSION_BRANCH ${${target}_VERSION_BRANCH})
    set(VERSION_FLAG ${${target}_VERSION_FLAG})
    set(VERSION_SHORTHASH ${${target}_VERSION_SHORTHASH})
    set(VERSION_FULLHASH ${${target}_VERSION_FULLHASH})

    if(NOT "${target}" STREQUAL "")
        set(TARGET_NAMESPACE "${target}::")
        set(VERSION_FILE_PREFIX "${target}_")
    else()
        set(TARGET_NAMESPACE "")
        set(VERSION_FILE_PREFIX "")
    endif()

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/build_info.hpp.in
        "${CMAKE_BINARY_DIR}/internal_use_only/include/build_info.hpp" ESCAPE_QUOTES
    )
endfunction()