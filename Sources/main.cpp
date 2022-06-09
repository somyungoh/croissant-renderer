#include "GLFW/glfw3.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "ray.h"
#include "material.h"
#include "hittable_list.h"
#include "camera.h"

#include <chrono>

const int render_w = 800;
const int render_h = 600;

const int nSamples = 16;
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
    // Timer
    auto            begin = std::chrono::steady_clock::now();

    // Camera
    float           aspectRatio = (float)render_w / render_h;
    cr::CCamera     camera = cr::CCamera(45.f, aspectRatio);
    camera.SetPos(glm::vec3(0, 0.65, -1));
    camera.LookAt(glm::vec3(0, 0, 0));

    // Scene
    std::shared_ptr<cr::CMaterial>  mat_labmbertGreen = std::make_shared<cr::CMaterialLambertian>(glm::vec3(0.15, 0.6, 0.09));
    std::shared_ptr<cr::CMaterial>  mat_lambertWhite = std::make_shared<cr::CMaterialLambertian>(glm::vec3(1.0f));
    std::shared_ptr<cr::CMaterial>  mat_lambertBrown = std::make_shared<cr::CMaterialLambertian>(glm::vec3(0.92f, 0.59f, 0.17f));
    std::shared_ptr<cr::CMaterial>  mat_lambertWhiteGray = std::make_shared<cr::CMaterialLambertian>(glm::vec3(0.8f));
    std::shared_ptr<cr::CMaterial>  mat_lambertBlue = std::make_shared<cr::CMaterialLambertian>(glm::vec3(0.2, 0.18, 0.87));
    std::shared_ptr<cr::CMaterial>  mat_metalWhite = std::make_shared<cr::CMaterialMetal>(glm::vec3(1.0, 1.0, 1.0), 0);
    std::shared_ptr<cr::CMaterial>  mat_metalGold = std::make_shared<cr::CMaterialMetal>(glm::vec3(0.8, 0.6, 0.2), 0);
    std::shared_ptr<cr::CMaterial>  mat_metalBlue = std::make_shared<cr::CMaterialMetal>(glm::vec3(0.2, 0.3, 0.8), 0);
    std::shared_ptr<cr::CMaterial>  mat_metalRose = std::make_shared<cr::CMaterialMetal>(glm::vec3(0.8, 0.3, 0.2), 0.2);
    std::shared_ptr<cr::CMaterial>  mat_glass = std::make_shared<cr::CMaterialGlass>(1.9, 0);

    cr::CHittableList   world;

#if 1   // Use Obj
    auto croissant = std::make_shared<cr::CHittableMesh>(glm::vec3(0, 0, 0), mat_lambertBrown);
    croissant->Load("Model/Croissants_obj/Croissant.obj");
    world.Add(croissant);
#else
    world.Add(std::make_shared<cr::CHittableSphere>(cr::CHittableSphere(glm::vec3(0, 0, 0), 0.1, mat_lambertWhite)));
#endif
    world.Add(std::make_shared<cr::CHittableSphere>(cr::CHittableSphere(glm::vec3(0.1, 0.097, 0.3), 0.15, mat_lambertWhite)));
    world.Add(std::make_shared<cr::CHittableSphere>(cr::CHittableSphere(glm::vec3(0.35, 0.07, 0.18), 0.12, mat_metalRose)));
    world.Add(std::make_shared<cr::CHittableSphere>(cr::CHittableSphere(glm::vec3(-0.3, 0.05, 0), 0.1, mat_metalWhite)));
    world.Add(std::make_shared<cr::CHittableSphere>(cr::CHittableSphere(glm::vec3(0.18, 0.025, -0.15), 0.05, mat_glass)));
    world.Add(std::make_shared<cr::CHittableSphere>(cr::CHittableSphere(glm::vec3(-0.155, 0.06, 0.23), 0.11, mat_metalBlue)));
    world.Add(std::make_shared<cr::CHittableSphere>(cr::CHittableSphere(glm::vec3(0, -10.05, 0), 10, mat_lambertWhite)));

    world.BuildBVHTree();

    // Render loop
    printf("[Render] Start rendering...\n");
    fflush(stdout);

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
    printf("[Render] Done.\n");
    printf("[Render] Elpased: %.3fs.\n", elapsed / 1000.f);
}

//----------------------------------------------------

void saveJpeg()
{
    const char* filename = "MyRender.jpg";

    // remap pixmap to 8 bits
    unsigned char pixmap8bits[render_w * render_h * 3];
    for (size_t i = 0; i < render_w * render_h * 3; ++i)
        pixmap8bits[i] = pixmap[i] * 255.f;

    stbi_flip_vertically_on_write(true);
    int success = stbi_write_jpg(filename, render_w, render_h, 3, pixmap8bits, 100);

    if (success)
        std::cout << "Image saved: " << filename << std::endl;
    else
        std::cout << "Error while saving image: " << filename << std::endl;
}

//----------------------------------------------------

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        saveJpeg();
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

    glfwSetKeyCallback(window, keyCallback);
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
