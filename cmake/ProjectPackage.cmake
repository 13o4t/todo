include(${CMAKE_CURRENT_LIST_DIR}/Utils.cmake)

function(package_project)
    include(CMakePackageConfigHelpers)
    include(GNUInstallDirs)

    set(options ARCH_INDEPENDENT)
    set(oneValueArgs 
        NAME 
        VERSION 
        CONTACT
        COMPATIBILITY
        LICENSE_FILE
        WITH_FIND_PACKAGE
    )
    set(multiValueArgs 
        TARGETS 
        INCLUDE_DIR
        DEPENDENCIES 
        DEPENDENCIES_CONFIGURED 
    )
    cmake_parse_arguments(PACKAGE "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    if("${PACKAGE_NAME}" STREQUAL "")
        set(PACKAGE_NAME ${PROJECT_NAME})
    endif()

    if(NOT PACKAGE_TARGETS)
        get_all_installable_targets(PACKAGE_TARGETS)
    endif()
    if(WIN32)
        foreach(target ${PACKAGE_TARGETS})
            install(FILES $<TARGET_RUNTIME_DLLS:${target}> DESTINATION "${CMAKE_INSTALL_BINDIR}")
        endforeach()
    endif()

    if("${PACKAGE_VERSION}" STREQUAL "")
        set(PACKAGE_VERSION ${PROJECT_VERSION})
    endif()

    if("${PACKAGE_COMPATIBILITY}" STREQUAL "")
        set(PACKAGE_COMPATIBILITY "SameMajorVersion")
    endif()

    if(NOT "${PACKAGE_INCLUDE_DIR}" STREQUAL "")
        foreach(include_file ${PACKAGE_INCLUDE_DIR})
            if(NOT IS_ABSOLUTE ${include_file})
                set(include_file "${CMAKE_CURRENT_SOURCE_DIR}/${include_file}")
            endif()

            if(IS_DIRECTORY ${include_file})
                install(DIRECTORY ${include_file} DESTINATION "./" COMPONENT dev)
            else()
                install(FILES ${include_file} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev)
            endif()
        endforeach()
    endif()

    if(NOT "${PACKAGE_DEPENDENCIES_CONFIGURED}" STREQUAL "")
        set(DEPENDENCIES_CONFIG)
        foreach(dep ${PACKAGE_DEPENDENCIES_CONFIGURED})
            list(APPEND DEPENDENCIES_CONFIG "${dep} CONFIG")
        endforeach()
    endif()
    list(APPEND PACKAGE_DEPENDENCIES ${DEPENDENCIES_CONFIG})

    if(PACKAGE_WITH_FIND_PACKAGE)
        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
            "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake" @ONLY
        )
        write_basic_package_version_file(
            "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake"
            VERSION ${PACKAGE_VERSION}
            COMPATIBILITY ${PACKAGE_COMPATIBILITY}
        )
        install(
            FILES "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake"
                "${PROJECT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PACKAGE_NAME}"
            COMPONENT dev
        )
    endif()

    install(
        TARGETS ${PACKAGE_TARGETS}
        EXPORT ${PACKAGE_NAME}
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT runtime NAMELINK_COMPONENT dev
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT dev
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT runtime
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PACKAGE_NAME}" COMPONENT dev
    )

    set(CPACK_PACKAGE_NAME ${PACKAGE_NAME})
    set(CPACK_PACKAGE_VERSION ${PACKAGE_VERSION})
    set(CPACK_INSTALL_PREFIX ${PACKAGE_NAME})
    set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CPACK_SOURCE_IGNORE_FILES "${PROJECT_BINARY_DIR};/\\\\..*")
    set(CPACK_RESOURCE_FILE_LICENSE ${PACKAGE_LICENSE_FILE})
    set(CPACK_PACKAGE_CONTACT ${PACKAGE_CONTACT})
    set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
    set(CPACK_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS 
        OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE
    )

    if(APPLE)
        set(CPACK_GENERATOR "DragNDrop")
    elseif(UNIX)
        set(CPACK_GENERATOR "DEB")
        set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    elseif(WIN32)
        set(CPACK_GENERATOR "NSIS")
    endif()

    include(CPack)
endfunction()

