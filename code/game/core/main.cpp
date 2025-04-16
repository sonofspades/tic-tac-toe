#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <btBulletCollisionCommon.h>

#include <iostream>
#include <vector>

auto vertex_stage_text =
"#version 450\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 0) uniform mat4 proj;\n"
"layout (location = 1) uniform mat4 view;\n"
"layout (location = 2) uniform mat4 model;\n"
"void main()\n"
"{\n"
"    gl_Position = proj * view * model * vec4(position, 1.0);\n"
"}\n";

auto fragment_stage_text =
"#version 450\n"
"out vec4 color;\n"
"layout (location = 3) uniform vec3 u_color;\n"
"void main()\n"
"{\n"
"    color = vec4(u_color, 1.0);\n"
"}\n";

std::vector<float> debug_vertices;

class PhysicsDebug : public btIDebugDraw
{
public:
    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
    {
        debug_vertices.push_back(from.x());
        debug_vertices.push_back(from.y());
        debug_vertices.push_back(from.z());

        debug_vertices.push_back(to.x());
        debug_vertices.push_back(to.y());
        debug_vertices.push_back(to.z());
    }

    void clearLines() override
    {
        debug_vertices.clear();
    }

    void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override { }
    void reportErrorWarning(const char* warningString) override { }
    void setDebugMode(int debugMode) override { }
    int getDebugMode() const override { return DBG_DrawWireframe; }
};

auto is_editor = false;

int32_t tiles[3][3] { };
auto x_turn = true;
auto is_end = false;

btCollisionWorld* world;

glm::mat4 view;
glm::mat4 proj;

enum tile_type
{
    tile_empty,
    tile_x,
    tile_o
};

auto check_row(const int32_t row, const int32_t type) -> bool
{
    return tiles[row][0] == type &&
           tiles[row][1] == type &&
           tiles[row][2] == type;
}

auto check_col(const int32_t col, const int32_t type) -> bool
{
    return tiles[0][col] == type &&
           tiles[1][col] == type &&
           tiles[2][col] == type;
}

auto check_diagonals(const int32_t type) -> bool
{
    return tiles[0][0] == type &&
           tiles[1][1] == type &&
           tiles[2][2] == type ||
           tiles[0][2] == type &&
           tiles[1][1] == type &&
           tiles[2][0] == type;
}

auto check_win(const int32_t type) ->  void
{
    for (auto row = 0; row < 3; row++)
    {
        is_end = check_row(row, type);

        if (is_end)
        {
            return;
        }
    }

    for (auto col = 0; col < 3; col++)
    {
        is_end = check_col(col, type);

        if (is_end)
        {
            return;
        }
    }

    is_end = check_diagonals(type);
}

auto main() -> int
{
    glfwInit();

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    constexpr auto window_width  = 1000;
    constexpr auto window_height = 1000;

    const auto window = glfwCreateWindow(window_width, window_height, "Tic-Tac-Toe", nullptr, nullptr);

    glfwSetKeyCallback(window, [](GLFWwindow*, const int key, int, const int action, int) -> void
    {
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        {
            for (auto row = 0; row < 3; row++)
            {
                for (auto col = 0; col < 3; col++)
                {
                    tiles[row][col] = tile_empty;
                }
            }

            x_turn = true;
            is_end = false;
        }

        if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
        {
            is_editor = !is_editor;
        }
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window_ptr, const int button, const int action, const int) -> void
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !is_end)
        {
            double x, y;

            glfwGetCursorPos(window_ptr, &x, &y);

            float ndcX =        (2.0f * x) / window_width - 1.0f;
            float ndcY = 1.0f - (2.0f * y) / window_height;

            glm::mat4 invProj = glm::inverse(proj);
            glm::mat4 invView = glm::inverse(view);

            glm::vec4 nearPointNDC(ndcX, ndcY, -1.0f, 1.0f);
            glm::vec4  farPointNDC(ndcX, ndcY,  1.0f, 1.0f);

            glm::vec4 nearPointWorld = invView * (invProj * nearPointNDC);
            glm::vec4 farPointWorld  = invView * (invProj *  farPointNDC);

            nearPointWorld /= nearPointWorld.w;
             farPointWorld /=  farPointWorld.w;

            glm::vec3 rayStart = glm::vec3(nearPointWorld);
            glm::vec3 rayEnd   = glm::vec3(farPointWorld);
            glm::vec3 rayDir   = glm::normalize(rayEnd - rayStart);

            glm::vec3 ray_end = rayStart + rayDir * 1000.0f;

            const btVector3 rayFrom(rayStart.x, rayStart.y, rayStart.z);
            const btVector3 rayTo(ray_end.x, ray_end.y, ray_end.z);

            btCollisionWorld::ClosestRayResultCallback result(rayFrom, rayTo);

            world->rayTest(rayFrom, rayTo, result);

            if (result.hasHit())
            {
                auto row = result.m_collisionObject->getUserIndex();
                auto col = result.m_collisionObject->getUserIndex2();

                if (tiles[row][col] == tile_empty)
                {
                    if (x_turn)
                    {
                        tiles[row][col] = tile_x;
                        check_win(tile_x);
                    }
                    else
                    {
                        tiles[row][col] = tile_o;
                        check_win(tile_o);
                    }

                    x_turn = !x_turn;
                }
            }
        }
    });

    glfwMakeContextCurrent(window);

    gladLoadGL();

    const auto vertex_stage = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_stage, 1, &vertex_stage_text, nullptr);
    glCompileShader(vertex_stage);

    const auto fragment_stage = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_stage, 1, &fragment_stage_text, nullptr);
    glCompileShader(fragment_stage);

    const auto shader = glCreateProgram();
    glAttachShader(shader, vertex_stage);
    glAttachShader(shader, fragment_stage);
    glLinkProgram(shader);

    std::vector<float>    o_vertices;
    std::vector<uint32_t> o_elements;

    Assimp::Importer o_importer;

    const auto o_scene = o_importer.ReadFile("o.obj", 0);
    const auto o_mesh  = o_scene->mMeshes[0];

    for (auto i = 0; i < o_mesh->mNumVertices; i++)
    {
        auto vertex = o_mesh->mVertices[i];

        o_vertices.push_back(vertex.x);
        o_vertices.push_back(vertex.y);
        o_vertices.push_back(vertex.z);
    }

    for (auto i = 0; i < o_mesh->mNumFaces; i++)
    {
        const auto face = o_mesh->mFaces[i];

        o_elements.push_back(face.mIndices[0]);
        o_elements.push_back(face.mIndices[1]);
        o_elements.push_back(face.mIndices[2]);
    }

    uint32_t o_vao, o_vbo, o_ebo;

    glGenVertexArrays(1, &o_vao);
    glBindVertexArray(o_vao);

    glGenBuffers(1, &o_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, o_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * o_vertices.size(), o_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &o_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * o_elements.size(), o_elements.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::vector<float>    x_vertices;
    std::vector<uint32_t> x_elements;

    Assimp::Importer x_importer;

    const auto x_scene = x_importer.ReadFile("x.obj", 0);
    const auto x_mesh  = x_scene->mMeshes[0];

    for (auto i = 0; i < x_mesh->mNumVertices; i++)
    {
        auto vertex = x_mesh->mVertices[i];

        x_vertices.push_back(vertex.x);
        x_vertices.push_back(vertex.y);
        x_vertices.push_back(vertex.z);
    }

    for (auto i = 0; i < x_mesh->mNumFaces; i++)
    {
        const auto face = x_mesh->mFaces[i];

        x_elements.push_back(face.mIndices[0]);
        x_elements.push_back(face.mIndices[1]);
        x_elements.push_back(face.mIndices[2]);
    }

    uint32_t x_vao, x_vbo, x_ebo;

    glGenVertexArrays(1, &x_vao);
    glBindVertexArray(x_vao);

    glGenBuffers(1, &x_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, x_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * x_vertices.size(), x_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &x_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, x_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * x_elements.size(), x_elements.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::vector<float>    grid_vertices;
    std::vector<uint32_t> grid_elements;

    Assimp::Importer grid_importer;

    const auto grid_scene = grid_importer.ReadFile("grid.obj", 0);
    const auto grid_mesh  = grid_scene->mMeshes[0];

    for (auto i = 0; i < grid_mesh->mNumVertices; i++)
    {
        auto vertex = grid_mesh->mVertices[i];

        grid_vertices.push_back(vertex.x);
        grid_vertices.push_back(vertex.y);
        grid_vertices.push_back(vertex.z);
    }

    for (auto i = 0; i < grid_mesh->mNumFaces; i++)
    {
        const auto face = grid_mesh->mFaces[i];

        grid_elements.push_back(face.mIndices[0]);
        grid_elements.push_back(face.mIndices[1]);
        grid_elements.push_back(face.mIndices[2]);
    }

    uint32_t grid_vao, grid_vbo, grid_ebo;

    glGenVertexArrays(1, &grid_vao);
    glBindVertexArray(grid_vao);

    glGenBuffers(1, &grid_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * grid_vertices.size(), grid_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &grid_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grid_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * grid_elements.size(), grid_elements.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    constexpr auto tile_space = 1.5f;

    proj = glm::perspective(glm::radians(60.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);

    world = new btCollisionWorld(new btCollisionDispatcher(new btDefaultCollisionConfiguration()), new btDbvtBroadphase(), new btDefaultCollisionConfiguration());
    world->setDebugDrawer(new PhysicsDebug());

    for (auto row = 0; row < 3; row++)
    {
        for (auto col = 0; col < 3; col++)
        {
            const auto x = -tile_space + col * tile_space;
            const auto y =  tile_space - row * tile_space;

            btTransform transform;
            transform.setIdentity();
            transform.setOrigin(btVector3(x, y, 0.0f));

            btCollisionShape* tile_shape = new btBoxShape(btVector3(0.5f, 0.5f, 0.105f));

            auto tile_object = new btCollisionObject();
            tile_object->setCollisionShape(tile_shape);
            tile_object->setWorldTransform(transform);
            tile_object->setUserIndex(row);
            tile_object->setUserIndex2(col);

            world->addCollisionObject(tile_object);
        }
    }

    uint32_t debug_vao, debug_vbo;

    glGenVertexArrays(1, &debug_vao);
    glBindVertexArray(debug_vao);

    glGenBuffers(1, &debug_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, debug_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * debug_vertices.size(), debug_vertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);

    glm::vec3 x_color    { 1.0f, 0.8392156862745098f, 0.22745098039215686f };
    glm::vec3 o_color    { 0.9686274509803922f, 0.35294117647058826f, 0.35294117647058826f };
    glm::vec3 grid_color { 1.0f, 0.6627450980392157f, 0.3333333333333333f };

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        world->debugDrawWorld();

        glBindBuffer(GL_ARRAY_BUFFER, debug_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * debug_vertices.size(), debug_vertices.data(), GL_DYNAMIC_DRAW);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        glUseProgram(shader);

        view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f));

        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(view));

        if (is_editor)
        {
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            auto model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(3, 1, glm::value_ptr(grid_color));

            glBindVertexArray(grid_vao);
            glDrawElements(GL_TRIANGLES, grid_elements.size(), GL_UNSIGNED_INT, nullptr);

            glUniform3fv(3, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));

            glBindVertexArray(debug_vao);
            glDrawArrays(GL_LINES, 0, static_cast<int>(debug_vertices.size()) / 3);

            for (auto row = 0; row < 3; row++)
            {
                for (auto col = 0; col < 3; col++)
                {
                    const auto x = -tile_space + col * tile_space;
                    const auto y =  tile_space - row * tile_space;

                    model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

                    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(model));

                    if (tiles[row][col] == tile_x)
                    {
                        glUniform3fv(3, 1, glm::value_ptr(x_color));

                        glBindVertexArray(x_vao);
                        glDrawElements(GL_TRIANGLES, x_elements.size(), GL_UNSIGNED_INT, nullptr);
                    }
                    else if (tiles[row][col] == tile_o)
                    {
                        glUniform3fv(3, 1, glm::value_ptr(o_color));

                        glBindVertexArray(o_vao);
                        glDrawElements(GL_TRIANGLES, o_elements.size(), GL_UNSIGNED_INT, nullptr);
                    }
                }
            }
        }
        else
        {
            glClearColor(0.42745098039215684f, 0.8823529411764706f, 0.8235294117647058f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(3, 1, glm::value_ptr(grid_color));

            glBindVertexArray(grid_vao);
            glDrawElements(GL_TRIANGLES, grid_elements.size(), GL_UNSIGNED_INT, nullptr);

            for (auto row = 0; row < 3; row++)
            {
                for (auto col = 0; col < 3; col++)
                {
                    const auto x = -tile_space + col * tile_space;
                    const auto y =  tile_space - row * tile_space;

                    model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

                    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(model));

                    if (tiles[row][col] == tile_x)
                    {
                        glUniform3fv(3, 1, glm::value_ptr(x_color));

                        glBindVertexArray(x_vao);
                        glDrawElements(GL_TRIANGLES, x_elements.size(), GL_UNSIGNED_INT, nullptr);
                    }
                    else if (tiles[row][col] == tile_o)
                    {
                        glUniform3fv(3, 1, glm::value_ptr(o_color));

                        glBindVertexArray(o_vao);
                        glDrawElements(GL_TRIANGLES, o_elements.size(), GL_UNSIGNED_INT, nullptr);
                    }
                }
            }
        }

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &o_vao);
    glDeleteVertexArrays(1, &x_vao);

    glDeleteBuffers(1, &o_vbo);
    glDeleteBuffers(1, &o_ebo);
    glDeleteBuffers(1, &x_vbo);
    glDeleteBuffers(1, &x_ebo);

    glDeleteShader(vertex_stage);
    glDeleteShader(fragment_stage);

    glDeleteProgram(shader);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}