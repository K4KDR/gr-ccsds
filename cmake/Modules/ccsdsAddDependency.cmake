find_package(Git QUIET)

macro(ccsds_add_dependency)
    set(options "")
    set(oneValueArgs NAME DESCRIPTION DEFAULT_VALUE)
    set(multiValueArgs "")
    cmake_parse_arguments(DEPENDENCY "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN} )
    
    string(TOUPPER ${DEPENDENCY_NAME} DEPENDENCY_NAME_UPPER)
    set(DEPENDENCY_OPTION_NAME "CCSDS_DEPS_${DEPENDENCY_NAME_UPPER}")
    set(DEPENDENCY_TARGET "${DEPENDENCY_NAME}::${DEPENDENCY_NAME}")
    
    option(${DEPENDENCY_OPTION_NAME} "Download and build external ${DEPENDENCY_NAME} dependency" ${DEPENDENCY_DEFAULT_VALUE})
    
    message(STATUS "=======================================================================")
    message(STATUS "Enabling thrid party dependency ${DEPENDENCY_NAME}:")
    message(STATUS "  Name ........ ${DEPENDENCY_NAME}")
    message(STATUS "  Description . ${DEPENDENCY_DESCRIPTION}")
    message(STATUS "  Target name . ${DEPENDENCY_TARGET}")
    message(STATUS "  option name . ${DEPENDENCY_OPTION_NAME}")
    message(STATUS "  default value ${DEPENDENCY_DEFAULT_VALUE}")
    message(STATUS "  actual value  ${${DEPENDENCY_OPTION_NAME}}")
    message(STATUS "-----------------------------------------------------------------------")
    
        
    if(${DEPENDENCY_OPTION_NAME})
        
        if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
            message(STATUS "Submodule update")
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init extern/${DEPENDENCY_NAME}
                            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                            RESULT_VARIABLE GIT_SUBMOD_RESULT)
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "'git submodule update --init extern/${DEPENDENCY_NAME}' failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
            endif()
            
            if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/${DEPENDENCY_NAME}/CMakeLists.txt")
                message(FATAL_ERROR "The submodule ${DEPENDENCY_NAME} was not downloaded! GIT_SUBMODULE failed. Please update submodules and try again.")
            endif()
        else()
            message(FATAL_ERROR "The option ${DEPENDENCY_OPTION_NAME} is enabled, but either the project is not a git repository, or the git binary could not be found.")
        endif()
        
        #file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/extern/${DEPENDENCY_NAME}/build)
        
        #add_subdirectory(${PROJECT_SOURCE_DIR}/extern/${DEPENDENCY_NAME} ${PROJECT_SOURCE_DIR}/extern/${DEPENDENCY_NAME}/build)
        
        add_subdirectory(${PROJECT_SOURCE_DIR}/extern/${DEPENDENCY_NAME} ${CMAKE_CURRENT_BINARY_DIR}/extern/${DEPENDENCY_NAME})
    else()
        message(STATUS "Looking for ${DEPENDENCY_NAME} in system paths")
        find_package(${DEPENDENCY_NAME} REQUIRED)
        if(TARGET ${DEPENDENCY_TARGET})
            get_target_property(_loc ${DEPENDENCY_TARGET} LOCATION)
            message(STATUS  "Found ${DEPENDENCY_NAME} at ${_loc}")
        endif()
        
    endif()

    if(NOT TARGET ${DEPENDENCY_TARGET})
        MESSAGE(FATAL_ERROR "${DEPENDENCY_DESCR} not found")
    endif()
    
    message(STATUS "=======================================================================")
    
    mark_as_advanced(
        DEPENDENCY_NAME
        DEPENDENCY_DESCRIPTION
        DEPENDENCY_TARGET
        DEPENDENCY_GIT_URL
        DEPENDENCY_GIT_TAG
        DEPENDENCY_OPTION_NAME
        DEPENDENCY_DEFAULT_VALUE
        DEPENDENCY_NAME_UPPER
    )
endmacro()
