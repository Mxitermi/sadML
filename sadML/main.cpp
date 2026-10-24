#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>


#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>

#include "../dependencies/glm-1.0.3/glm/glm/glm.hpp"
#include "../dependencies/glm-1.0.3/glm/glm/ext/matrix_transform.hpp"
#include "../dependencies/glm-1.0.3/glm/glm/gtc/matrix_transform.hpp"

#pragma comment(lib, "SDL3.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

#include "vertex_buffer.h"
#include "defines.h"
#include "shader.h"
#include "Index_buffer.h"
#define _USE_MATH_DEFINES 
#include <cmath>
#include "camera.h";
#include "model_loader.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
int main(int argc, char** argv)
{

    SDL_Window* window;
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetSwapInterval(1);
    
    window = SDL_CreateWindow("SAD", 1600, 900, SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);


    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "Error with GLew: " << glewGetErrorString(err) << std::endl;
        std::cin.get();
        return -1;
    }
    
    std::cout << "Version of local OpenGL: " << glGetString(GL_VERSION) << std::endl;

    glViewport(0, 0, 1200, 900);

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplSDL3_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 330");

    ModelLoader mLoader;
    
    mLoader.addObjectToLoad("models/cube.obj", "Cube", glm::vec3(0.0f, 0.0f, 0.0f), true);


    std::vector<GameObject> objects = mLoader.initializeGameobjects();
    
    objects[0].influencedByWind = false;

    std::cout << "Objects: " << objects.size() << "\n";

    Shader shader("basic.vertex", "basic_object.frag");
    Shader shader_lines("basic_lines.vertex", "basic.frag");
    Shader shader_points("basic_points.vertex", "basic.frag");

    uint64_t perfCounterFrequency = SDL_GetPerformanceFrequency();
    uint64_t lastCounter = SDL_GetPerformanceCounter();
    float delta = 0.0f;

    int modelViewProjLocationLines = glGetUniformLocation(shader_lines.getShaderId(), "u_modelViewProj");
    int modelViewProjLocation = glGetUniformLocation(shader.getShaderId(), "u_modelViewProj");
    int modelViewProjLocationPoints = glGetUniformLocation(shader_points.getShaderId(), "u_modelViewProj");
    
    Camera camera(90.0f, 1200.0f, 900.0f);
    camera.translate(glm::vec3(0.0f, 0.0f, 5.0f));
    camera.update();
    

    float cameraSpeed = 7.0f;
    std::ifstream infile("save.sad");
    std::string line;
    std::string type;
    while (std::getline(infile, line)) {
        std::stringstream ss(line);
        ss >> type;
        if (type == "C") {
            glm::vec3 pos;
            float p, y;
            ss >> pos.x >> pos.y >> pos.z >> p >> y;
            camera.setPosition(pos);
            camera.setPitch(p);
            camera.setYaw(y);
            camera.onMouseMoved(0.0f, 0.0f);
            camera.update();
        }
    }
   
    bool enableMovement = false;
    bool moveW = false;
    bool moveA = false;
    bool moveS = false;
    bool moveD = false;
    bool moveUp = false;
    bool moveDown = false;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_CULL_FACE); //Aufpassen wenn selber dreiecke erstellen wegen windingorder
    glEnable(GL_DEPTH_TEST);

    static float timer = 0.0f;
    float time = 0.0f;
    bool isActive = true;
    while (isActive) {
        time += delta;
        SDL_Event event;

        

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
            {
                isActive = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key)
                {
                case SDLK_F3:
                    enableMovement = !enableMovement;
                    if (!enableMovement) {
                        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                        SDL_SetWindowRelativeMouseMode(window, false);

                        std::ofstream myfile;
                        myfile.open("save.sad");
                        glm::vec3 pos = camera.getPosition();
                        myfile << "C" << " " << pos.x << " " << pos.y << " " << pos.z << " " << camera.getPitch() << " " << camera.getYaw();
                        myfile.close();
                    }
                    else {
                        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
                        SDL_SetWindowRelativeMouseMode(window, true);
                    }
                    
                    break;

                case SDLK_ESCAPE:
                    isActive = false;
                    break;

                case SDLK_W:
                    moveW = true;
                    break;

                case SDLK_A:
                    moveA = true;
                    break;

                case SDLK_S:
                    moveS = true;
                    break;

                case SDLK_D:
                    moveD = true;
                    break;
                case SDLK_SPACE:
                    moveUp = true;
                    break;
                case SDLK_LSHIFT:
                    moveDown = true;
                    break;
                case SDLK_0:
                    camera.setPosition(glm::vec3(0.0f));
                    camera.setPitch(0.0f);
                    camera.setYaw(-90.0f);
                    camera.update();
                    break;
                case SDLK_P:
                    timer = 0.1f;
                }
            }
            else if (event.type == SDL_EVENT_KEY_UP)
            {
                switch (event.key.key)
                {
                case SDLK_W:
                    moveW = false;
                    break;
  
                case SDLK_A:
                    moveA = false;
                    break;

                case SDLK_S:
                    moveS = false;
                    break;

                case SDLK_D:
                    moveD = false;
                    break;
                case SDLK_SPACE:
                    moveUp = false;
                    break;
                case SDLK_LSHIFT:
                    moveDown = false;
                    break;
                }

            }

            if (event.type == SDL_EVENT_MOUSE_MOTION && enableMovement) {
                camera.onMouseMoved(event.motion.xrel, event.motion.yrel);
            }
            
        }
        if (enableMovement) {
            if (moveW) {
                camera.moveFront(delta * cameraSpeed);
            }
            if (moveA) {
                camera.moveSideways(delta * cameraSpeed);
            }
            if (moveS) {
                camera.moveFront(-delta * cameraSpeed);
            }
            if (moveD) {
                camera.moveSideways(-delta * cameraSpeed);
            }
            if (moveUp) {
                camera.moveUp(delta * cameraSpeed);
            }
            if (moveDown) {
                camera.moveUp(-delta * cameraSpeed);
            }
            camera.update();
        }
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 mvp = camera.getViewProj();
        
        

        //IMGUI Zeug
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        float panelWidth = 400.0f;

        ImGui::SetNextWindowPos(
            ImVec2(
                ImGui::GetIO().DisplaySize.x - panelWidth,
                0
            )
        );

        ImGui::SetNextWindowSize(
            ImVec2(
                panelWidth,
                ImGui::GetIO().DisplaySize.y
            )
        );

        ImGui::Begin(
            "Wind Settings",
            nullptr,
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoTitleBar
        );

        

        ImGui::End();
        

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(
            ImGui::GetDrawData()
        );

        SDL_GL_SwapWindow(window);

        uint64_t endCounter = SDL_GetPerformanceCounter();
        uint64_t counterElapsed = endCounter - lastCounter;
        delta = ((float)counterElapsed) / (float)perfCounterFrequency;
        lastCounter = endCounter;
    }

    return 0;
}

