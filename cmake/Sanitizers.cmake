function(enable_sanitizers project_name
    ENABLE_ADDRESS
    ENABLE_LEAK
    ENABLE_UNDEFINED_BEHAVIOR
    ENABLE_THREAD
    ENABLE_MEMORY)

    set(SANITIZERS "")

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        if(${ENABLE_ADDRESS})
            list(APPEND SANITIZERS "address")
        endif()

        if(${ENABLE_LEAK})
            list(APPEND SANITIZERS "leak")
        endif()

        if(${ENABLE_UNDEFINED_BEHAVIOR})
            list(APPEND SANITIZERS "undefined")
        endif()

        if(${ENABLE_THREAD})
            if("address" IN_LIST SANITIZERS OR "leak" IN_LIST SANITIZERS)
                message(WARNING "thread sanitizer does not work with address and leak sanitizer enabled")
            else()
                list(APPEND SANITIZERS "thread")
            endif()
        endif()

        if(${ENABLE_MEMORY} AND CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
            if("address" IN_LIST SANITIZERS OR "thread" IN_LIST SANITIZERS OR "leak" IN_LIST SANITIZERS)
                message(WARNING "memory sanitizer does not work with address, thread or leak sanitizer enabled")
            else()
                list(APPEND SANITIZERS "memory")
            endif()
        endif()
    elseif(MSVC)
        if(${ENABLE_ADDRESS})
            list(APPEND SANITIZERS "address")
        endif()
        if(${ENABLE_LEAK} OR ${ENABLE_UNDEFINED_BEHAVIOR} OR ${ENABLE_THREAD} OR ${ENABLE_MEMORY})
            message(WARNING "MSVC only supports address sanitizer")
        endif()
    endif()

    list(JOIN SANITIZERS "," SANITIZERS_LIST)

    if(SANITIZERS_LIST)
        if(NOT "${SANITIZERS_LIST}" STREQUAL "")
            if(NOT MSVC)
                target_compile_options(${project_name} INTERFACE -fsanitize=${SANITIZERS_LIST})
                target_link_options(${project_name} INTERFACE -fsanitize=${SANITIZERS_LIST})
            else()
                target_compile_options(${project_name} INTERFACE /fsanitize=${SANITIZERS_LIST} /Zi /INCREMENTAL:NO)
                target_compile_definitions(${project_name} INTERFACE _DISABLE_VECTOR_ANNOTATION _DISABLE_STRING_ANNOTATION)
                target_link_options(${project_name} INTERFACE /INCREMENTAL:NO)
            endif()
        endif()
    endif()

endfunction()