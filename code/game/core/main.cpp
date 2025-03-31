#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

auto vertex_stage_text =
"#version 450\n"
"layout(location = 0) in vec3 position;\n"
"layout(location = 0) uniform mat4 proj;\n"
"layout(location = 1) uniform mat4 view;\n"
"layout(location = 2) uniform mat4 model;\n"
"void main()\n"
"{\n"
"    gl_Position = proj * view * model * vec4(position, 1.0);\n"
"}\n";

auto fragment_stage_text =
"#version 450\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"    color = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n";

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

    const std::vector vertices
    {
       -0.5f,  0.5f, 0.0f,
        0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f
    };

    const std::vector<uint32_t> elements
    {
        0, 1, 2,
        2, 3, 0
    };

    uint32_t vao, vbo, ebo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * elements.size(), elements.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    auto proj = glm::perspective(glm::radians(60.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);
    auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);

        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(view));

        for (auto row = 0; row < 3; row++)
        {
            for (auto col = 0; col < 3; col++)
            {
                constexpr auto  tile_space = 1.1f;

                const auto x = -tile_space + col * tile_space;
                const auto y =  tile_space - row * tile_space;

                auto model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

                glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(model));

                glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, nullptr);
            }
        }

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &vao);

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}