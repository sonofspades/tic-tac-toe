#=======================================================================================================================
target_include_directories(${PROJECT_NAME} PRIVATE ${BULLET_PHYSICS_SOURCE_DIR}/src ..)
 target_precompile_headers(${PROJECT_NAME} PRIVATE private.hpp)
     target_link_libraries(${PROJECT_NAME} PRIVATE glfw assimp glm BulletCollision LinearMath
                                                   graphics-module resources-module shaders-module)
            target_sources(${PROJECT_NAME} PRIVATE main.cpp board.cpp)
#=======================================================================================================================