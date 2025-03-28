#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

auto main() -> int
{
    glfwInit();

    const auto window = glfwCreateWindow(1000, 800, "Tic-Tac-Toe", nullptr, nullptr);

    glfwMakeContextCurrent(window);

    gladLoadGL();

    const std::vector vertices
    {
        0.0f, 0.0f, 0.0f
    };

    const std::vector<uint32_t> elements
    {
        0
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

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_POINTS, elements.size(), GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}