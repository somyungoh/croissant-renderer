#include <glfw3.h>
#include <iostream>

int main(void)
{
    GLFWwindow* window;

    std::cout << "\nInitializing OpenGL Context...\n";

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "Croissant-Renderer (beta)", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}