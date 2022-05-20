#include "GLFW/glfw3.h"

#include "ray.h"
#include "hittable.h"
#include "camera.h"

#include <iostream>

const int render_w = 800;
const int render_h = 600;
float *pixmap = new float[render_w * render_h * 3];

//----------------------------------------------------

void render()
{
    // Camera
    float           aspectRatio = (float)render_w / render_h;
    cr::CCamera     camera = cr::CCamera(glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), aspectRatio);

    // Sphere
    cr::CSphere     sphere = cr::CSphere(glm::vec3(0, 0, 0), 0.1);

    for (size_t w = 0; w < render_w; w++) {
        for (size_t h = 0; h < render_h; h++) {
            float   u = (float)w / (render_w - 1);
            float   v = (float)h / (render_h - 1);

            cr::CRay    ray = camera.GetRay(u, v);
            cr::SHitRec rec;

            glm::vec3 color(u, v, (u + v) * 0.5f);
            if (sphere.Hit(ray, rec))
            {
                color.r = 1.0;
                color.g = 0.0;
                color.b = 0.0;
            }
            pixmap[(h * render_w + w) * 3 + 0] = color.r;
            pixmap[(h * render_w + w) * 3 + 1] = color.g;
            pixmap[(h * render_w + w) * 3 + 2] = color.b;
        }
    }
}

//----------------------------------------------------

int main(void)
{
    GLFWwindow* window;

    std::cout << "\nInitializing GLFW Context...\n";

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    window = glfwCreateWindow(render_w, render_h, "Croissant-Renderer (beta)", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    render();

    while (!glfwWindowShouldClose(window))
    {
        int fbuffer_w, fbuffer_h;
        int window_w, window_h;
        glfwGetFramebufferSize(window, &fbuffer_w, &fbuffer_h);
        glfwGetWindowSize(window, &window_w, &window_h);

        glViewport(0, 0, fbuffer_w, fbuffer_h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, window_w, 0, window_h, 0, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glClear(GL_COLOR_BUFFER_BIT);
        glRasterPos2i(0, 0);
        glPixelZoom(fbuffer_w / render_w, fbuffer_h / render_h);    // because (window size != framebuffer) size can happen
        glDrawPixels(render_w, render_h, GL_RGB, GL_FLOAT, pixmap);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    
    delete[] pixmap;

    return 0;
}
