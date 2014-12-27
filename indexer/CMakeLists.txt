
add_library (lister OBJECT lister.h lister.cpp)

add_executable (indexer main.cpp $<TARGET_OBJECTS:lister>)
target_link_libraries (indexer writer)

