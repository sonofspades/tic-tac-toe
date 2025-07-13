#=======================================================================================================================
target_include_directories(${PROJECT_NAME} PRIVATE ..)
 target_precompile_headers(${PROJECT_NAME} PRIVATE headers.hpp)
     target_link_libraries(${PROJECT_NAME} PRIVATE common-module graphics-module resources-module shaders-module glfw)
            target_sources(${PROJECT_NAME} PRIVATE main.cpp board.cpp)
#=======================================================================================================================