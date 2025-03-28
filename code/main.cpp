#include <GLFW/glfw3.h>

auto main() -> int
{
    glfwInit();

    const auto window = glfwCreateWindow(1000, 800, "Tic-Tac-Toe", nullptr, nullptr);

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}