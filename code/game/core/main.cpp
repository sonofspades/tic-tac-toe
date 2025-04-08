#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <btBulletCollisionCommon.h>

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
    void draw3dText(const btVector3& location, const char* textString) override { }
    void setDebugMode(int debugMode) override { }
    int getDebugMode() const override { return DBG_DrawWireframe; }
};

auto main() -> int
{
    glfwInit();

    constexpr auto window_width  = 1000;
    constexpr auto window_height = 1000;

    const auto window = glfwCreateWindow(window_width, window_height, "Tic-Tac-Toe", nullptr, nullptr);

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

    const std::vector tile_vertices
    {
       -0.5f,  0.5f, 0.0f,
        0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f
    };

    const std::vector<uint32_t> tile_elements
    {
        0, 1, 2,
        2, 3, 0
    };

    uint32_t tile_vao, tile_vbo, tile_ebo;

    glGenVertexArrays(1, &tile_vao);
    glBindVertexArray(tile_vao);

    glGenBuffers(1, &tile_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, tile_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tile_vertices.size(), tile_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &tile_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tile_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * tile_elements.size(), tile_elements.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    auto proj = glm::perspective(glm::radians(60.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);
    auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.5f));

    btCollisionWorld* world = new btCollisionWorld(new btCollisionDispatcher(new btDefaultCollisionConfiguration()), new btDbvtBroadphase(), new btDefaultCollisionConfiguration());
    world->setDebugDrawer(new PhysicsDebug());

    btCollisionShape* tile_shape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));

    btCollisionObject* tile_object = new btCollisionObject();
    tile_object->setCollisionShape(tile_shape);

    world->addCollisionObject(tile_object);

    uint32_t debug_vao, debug_vbo;

    glGenVertexArrays(1, &debug_vao);
    glBindVertexArray(debug_vao);

    glGenBuffers(1, &debug_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, debug_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * debug_vertices.size(), debug_vertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);

        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(view));

        auto model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

        glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(3, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));

        glBindVertexArray(debug_vao);
        glDrawArrays(GL_LINES, 0, static_cast<int>(debug_vertices.size()) / 3);

        for (auto row = 0; row < 3; row++)
        {
            for (auto col = 0; col < 3; col++)
            {
                constexpr auto  tile_space = 1.2f;

                const auto x = -tile_space + col * tile_space;
                const auto y =  tile_space - row * tile_space;

                auto model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

                glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(model));

                //glBindVertexArray(tile_vao);
                //glDrawElements(GL_TRIANGLES, tile_elements.size(), GL_UNSIGNED_INT, nullptr);

                if (row == col)
                {
                    glUniform3fv(3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));

                    glBindVertexArray(x_vao);
                    glDrawElements(GL_TRIANGLES, x_elements.size(), GL_UNSIGNED_INT, nullptr);
                }
                else
                {
                    glUniform3fv(3, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));

                    glBindVertexArray(o_vao);
                    glDrawElements(GL_TRIANGLES, o_elements.size(), GL_UNSIGNED_INT, nullptr);
                }
            }
        }

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &tile_vao);
    glDeleteVertexArrays(1, &o_vao);
    glDeleteVertexArrays(1, &x_vao);

    glDeleteBuffers(1, &tile_vbo);
    glDeleteBuffers(1, &tile_ebo);

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