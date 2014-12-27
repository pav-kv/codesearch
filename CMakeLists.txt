cmake_minimum_required (VERSION 2.8)

# The project
project (codesearch)
set (codesearch_VERSION_MAJOR 0)
set (codesearch_VERSION_MINOR 5)

# Add the sources and binary trees to the search path for include files.
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_BINARY_DIR}")

file (MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

########### Macros ###########
# TODO: make include file

macro (LINK_TARGET targetName linkPath)
    get_target_property (targetPath ${targetName} LOCATION)
    add_custom_command (
        TARGET ${targetName} POST_BUILD
        COMMAND ln --symbolic --force ${targetPath} ${linkPath}
        COMMENT "Making a symbolic link ${linkPath} -> ${targetPath}."
    )
endmacro (LINK_TARGET)

macro (COMPILE_PROTOBUF protoFile)
    add_custom_command (
        OUTPUT ${protoFile}.pb.h ${protoFile}.pb.cc
        COMMAND protoc -I${CMAKE_CURRENT_SOURCE_DIR} --cpp_out=. ${CMAKE_CURRENT_SOURCE_DIR}/${protoFile}.proto
    )
endmacro (COMPILE_PROTOBUF)

##############################

add_compile_options ( -O2 -std=c++11 )

add_subdirectory ( distributed )
add_subdirectory ( index       )
add_subdirectory ( indexer     )
add_subdirectory ( searcher    )
 
