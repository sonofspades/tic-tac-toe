#=======================================================================================================================
               add_library(glad)
#=======================================================================================================================
target_include_directories(glad PUBLIC include)
            target_sources(glad PUBLIC glad.cpp)
#=======================================================================================================================