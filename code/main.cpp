#include <glad/glad.h>
#include <GLFW/glfw3.h>

auto main() -> int
{
    glfwInit();

    const auto window = glfwCreateWindow(1000, 800, "Tic-Tac-Toe", nullptr, nullptr);

    glfwMakeContextCurrent(window);

    gladLoadGL();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}