cmake_minimum_required (VERSION 2.8)

# The project
project (codesearch)
set (codesearch_VERSION_MAJOR 0)
set (codesearch_VERSION_MINOR 5)

# Add the sources and binary trees to the search path for include files.
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_BINARY_DIR}")

file (MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# TODO: make include
macro (LINK_TARGET targetName linkPath)
    get_target_property (targetPath ${targetName} LOCATION)
    add_custom_command (
        TARGET ${targetName} POST_BUILD
        COMMAND ln --symbolic --force ${targetPath} ${linkPath}
        COMMENT "Making a symbolic link ${linkPath} -> ${targetPath}."
    )
endmacro (LINK_TARGET)

add_compile_options ( -O2 -std=c++11 )

add_subdirectory ( distributed )
add_subdirectory ( index       )
add_subdirectory ( indexer     )
add_subdirectory ( searcher    )
 
