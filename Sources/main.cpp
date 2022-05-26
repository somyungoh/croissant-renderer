#include "GLFW/glfw3.h"

#include "ray.h"
#include "material.h"
#include "hittable_list.h"
#include "camera.h"

#include <iostream>
#include <chrono>

const int render_w = 800;
const int render_h = 600;

const int nSamples = 255;
const int nSamplesW = glm::sqrt(nSamples);
const float nSamplesOffset = 0.5f / nSamplesW;

const int max_depth = 10;

float *pixmap = new float[render_w * render_h * 3];

//----------------------------------------------------

glm::vec3 raycast(const cr::CRay &ray, const cr::CHittableList &world, int depth)
{
    // max-depth reached
    if (depth <= 0) {
        return glm::vec3(0);
    }

    cr::SHitRec     hitRec;
    if (world.Hit(ray, 0.00001f, _INFINITY, hitRec))
    {
        // bounced rays
        cr::CRay    scatteredRay;
        glm::vec3   attenuation;
        if (hitRec.p_material->Scatter(ray, hitRec, attenuation, scatteredRay))
            return attenuation * raycast(scatteredRay, world, depth - 1);
        return glm::vec3(0);
    }

    // coloring
    float   t = 0.5f * (ray.m_dir.y + 1.0f);
    return glm::vec3(1.0) * (1.0f - t) + glm::vec3(0.5, 0.7, 1.0) * t;
}

//----------------------------------------------------

void render()
{
    printf("Start rendering... ");
    fflush(stdout);
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // Camera
    float           aspectRatio = (float)render_w / render_h;
    cr::CCamera     camera = cr::CCamera(glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), aspectRatio);

    // Scene
    std::shared_ptr<cr::CMaterial>  mat_labmbertGreen = std::make_shared<cr::CMaterialLambertian>(glm::vec3(0.15, 0.6, 0.09));
    std::shared_ptr<cr::CMaterial>  mat_lambertWhite = std::make_shared<cr::CMaterialLambertian>(glm::vec3(1.0f));
    std::shared_ptr<cr::CMaterial>  mat_lambertWhiteGray = std::make_shared<cr::CMaterialLambertian>(glm::vec3(0.8f));
    std::shared_ptr<cr::CMaterial>  mat_lambertBlue = std::make_shared<cr::CMaterialLambertian>(glm::vec3(0.2, 0.18, 0.87));
    std::shared_ptr<cr::CMaterial>  mat_metalGold = std::make_shared<cr::CMaterialMetal>(glm::vec3(0.8, 0.6, 0.2), 0);
    std::shared_ptr<cr::CMaterial>  mat_metalBlue = std::make_shared<cr::CMaterialMetal>(glm::vec3(0.2, 0.3, 0.8), 0);
    std::shared_ptr<cr::CMaterial>  mat_metalRose = std::make_shared<cr::CMaterialMetal>(glm::vec3(0.8, 0.3, 0.2), 0.2);
    std::shared_ptr<cr::CMaterial>  mat_glass = std::make_shared<cr::CMaterialGlass>(1.5, 0);

    cr::CHittableList   world;
    world.Add(std::make_shared<cr::CSphere>(cr::CSphere(glm::vec3(0, 0, 0), 0.1, mat_lambertWhite)));
    world.Add(std::make_shared<cr::CSphere>(cr::CSphere(glm::vec3(0.2, 0, 0), 0.1, mat_metalRose)));
    world.Add(std::make_shared<cr::CSphere>(cr::CSphere(glm::vec3(-0.2, 0, 0), 0.1, mat_metalBlue)));
    world.Add(std::make_shared<cr::CSphere>(cr::CSphere(glm::vec3(0, -5.075, 0.5), 5, mat_lambertWhiteGray)));
    
    for (size_t w = 0; w < render_w; w++) {
        for (size_t h = 0; h < render_h; h++) {
            for (size_t s = 0; s < nSamples; s++)
            {
                int     si = s % nSamplesW;
                int     sj = s / nSamplesW;
                float   u = (w + (float)si / nSamplesW + nSamplesOffset) / render_w;
                float   v = (h + (float)sj / nSamplesW + nSamplesOffset) / render_h;
                cr::CRay    ray = camera.GetRay(u, v);

                glm::vec3 color = raycast(ray, world, max_depth);

                // AA correction
                color /= nSamples;

                // gamma correction
                float scale = 1.0f / nSamples;
                color = glm::sqrt(color * scale);

                pixmap[(h * render_w + w) * 3 + 0] += color.r;
                pixmap[(h * render_w + w) * 3 + 1] += color.g;
                pixmap[(h * render_w + w) * 3 + 2] += color.b;
            }
        }
    }

    // elapsed time
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    printf("Done.\n");
    printf("Elpased: %.3fs.\n", elapsed / 1000.f);
}

//----------------------------------------------------

int main(void)
{
    GLFWwindow* window;

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
