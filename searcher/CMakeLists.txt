
add_library (searcherlib OBJECT searcher.h searcher.cpp)
add_executable (searcher main.cpp query.h $<TARGET_OBJECTS:searcherlib>)

