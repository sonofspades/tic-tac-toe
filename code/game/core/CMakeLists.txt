#=======================================================================================================================
target_include_directories(${PROJECT_NAME} PRIVATE ${BULLET_PHYSICS_SOURCE_DIR}/src ..)
     target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad assimp glm BulletCollision LinearMath)
            target_sources(${PROJECT_NAME} PRIVATE main.cpp)
#=======================================================================================================================