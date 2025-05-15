#=======================================================================================================================
target_include_directories(${PROJECT_NAME} PRIVATE ${BULLET_PHYSICS_SOURCE_DIR}/src ..)
 target_precompile_headers(${PROJECT_NAME} PRIVATE private.hpp)
     target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad assimp glm BulletCollision LinearMath graphics-module graphics-module)
            target_sources(${PROJECT_NAME} PRIVATE main.cpp)
#=======================================================================================================================