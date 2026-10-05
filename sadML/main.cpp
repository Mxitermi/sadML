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
#include "wind.h"
#include <algorithm>

constexpr float PI = 3.14159265358979323846f / 2;

void resetSimulation(std::vector<GameObject>& objects, std::vector<WindPoint>& wind) {
    for (GameObject& obj : objects) {
        obj.reset();
    }
    for (WindPoint& w : wind) {
        w.reset();
    }
}

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
    
    window = SDL_CreateWindow("SAD", 1100, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);


    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "Error with GLew: " << glewGetErrorString(err) << std::endl;
        std::cin.get();
        return -1;
    }
    
    std::cout << "Version of local OpenGL: " << glGetString(GL_VERSION) << std::endl;

    glViewport(0, 0, 800, 600);

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

  

    ImGui_ImplSDL3_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 330");

    ModelLoader mLoader;
    
    mLoader.addObjectToLoad("models/cube.obj", "Cube", glm::vec3(0.0f, 0.0f, 4.0f), false);


    std::vector<GameObject> objects = mLoader.initializeGameobjects();

    std::cout << "Objects: " << objects.size() << "\n";

    Shader shader("basic.vertex", "basic.frag");
    Shader shader_lines("basic_lines.vertex", "basic.frag");
    Shader shader_points("basic_points.vertex", "basic.frag");

    uint64_t perfCounterFrequency = SDL_GetPerformanceFrequency();
    uint64_t lastCounter = SDL_GetPerformanceCounter();
    float delta = 0.0f;

    int modelViewProjLocationLines = glGetUniformLocation(shader_lines.getShaderId(), "u_modelViewProj");
    int modelViewProjLocation = glGetUniformLocation(shader.getShaderId(), "u_modelViewProj");
    int modelViewProjLocationPoints = glGetUniformLocation(shader_points.getShaderId(), "u_modelViewProj");
    
    Camera camera(90.0f, 800.0f, 600.0f);
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
    
    //Code
    float airResolution = 0.1f;

    AirHandler airhandler(glm::vec3(-5.0f), glm::vec3(5.0f), airResolution); //fehlender support für rechtecke
    //glm::vec3 influence = airhandler.getInfluece(glm::vec3(2.6f, 2.2f, 2.4f));

    //std::cout << "Ist: " << influence.x << " " << influence.y << " " << influence.z;
    //glm::vec3 influenc = airhandler.getInfluece(glm::vec3(-5.0f));

    std::vector<WindPoint> wind = airhandler.createWindLayer(glm::vec3(-5, -2, 5), glm::vec3(5, 2, 5), 0.4f);


    std::vector<Vertex> windBoundingBox = airhandler.getWindBoundingBoxVertecies();
    VertexBuffer vWindBindingBoxBuffer(
        windBoundingBox.data(),
        static_cast<uint32_t>(windBoundingBox.size())
    );
    std::vector<uint32_t> windIndices = airhandler.getWindBoundingBoxIndices();
    IndexBuffer iWindBuffer(windIndices.data(), static_cast<uint32_t>(windIndices.size()), sizeof(uint32_t));
    //Code end

    std::vector<int> indiceForClear;

    bool enableMovement = false;
    bool moveW = false;
    bool moveA = false;
    bool moveS = false;
    bool moveD = false;
    bool moveUp = false;
    bool moveDown = false;

    //Constants
    const float airPressaure = 101.325f; //kPa
    const float airTemperatur = 59.0f; // Fahrenheit
    const float airDensity = 1.2250f; // kg/m³
    const float maxWindSpeed = 56; // m/s

    float windSpeed = 15.5556f; // m/s
    float simulationSpeed = 0.2f; 

    bool spawnWindParticles = true;
    float customWindDirectionX = 0.0f;
    float customWindDirectionY = 0.0f;
    float customWindDirectionZ = -1.0f;
    bool displayWindPoints = false;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_CULL_FACE); //Aufpassen wenn selber dreiecke erstellen wegen windingorder
    glEnable(GL_DEPTH_TEST);

    resetSimulation(objects, wind);
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
                case SDLK_RETURN:
                    resetSimulation(objects, wind);
                    time = 0.0f;
                    break;
                case SDLK_L:
                    objects.push_back(mLoader.addGameObject("Cube", glm::vec3(0.0f)));
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

        int index = -1;
        shader.bind();
        for (GameObject& object : objects)
        {
            index++;

            /*wobject.velocity = airhandler.calculateDragAcceleration(
                w.velocity,
                newVelcoity * windSpeed,
                airDensity,  // airDensity
                0.01f,  // area
                0.45f,  // dragCoef
                0.001f    // mass
            ) * delta;*/

            glm::vec3 newVelocity = airhandler.getInfluece(object.position) * windSpeed;
            object.velocity = airhandler.calculateDragAcceleration(object.velocity, newVelocity, airDensity, 1, 1.05f, 1) * delta;
            //object.momentum *= (1-0.1 * delta);
            

            object.position += object.velocity * delta * simulationSpeed;

            glm::mat4 model = glm::translate(
                glm::mat4(1.0f),
                object.position
            );

            model = glm::rotate(
                model,
                object.rotation.x,
                glm::vec3(1.0f, 0.0f, 0.0f)
            );

            model = glm::rotate(
                model,
                object.rotation.y,
                glm::vec3(0.0f, 1.0f, 0.0f)
            );

            model = glm::rotate(
                model,
                object.rotation.z,
                glm::vec3(0.0f, 0.0f, 1.0f)
            );

            model = glm::scale(model, object.scale);

            glm::mat4 modelViewProj = mvp * model;
            glUniformMatrix4fv(modelViewProjLocation, 1, GL_FALSE, &modelViewProj[0][0]);
            object.model->vBuffer.bind();
            object.model->iBuffer.bind();

            glDrawElements(GL_TRIANGLES, object.model->numIndices, GL_UNSIGNED_INT, 0);

            if (!airhandler.isInfluenced(object.position)) {
                indiceForClear.push_back(index);
            }
        }
        

        std::sort(indiceForClear.begin(), indiceForClear.end(), std::greater<int>());
        for (auto i : indiceForClear)
            objects.erase(objects.begin() + i);
        indiceForClear.clear();

        shader_lines.bind();
        
        glUniformMatrix4fv(modelViewProjLocationLines, 1, GL_FALSE, &mvp[0][0]);
        vWindBindingBoxBuffer.bind();
        iWindBuffer.bind();

        glDrawElements(GL_LINES, static_cast<uint32_t>(windIndices.size()), GL_UNSIGNED_INT, 0);
        shader.bind();

        //glUniform4f(cUniformLocation1, sinf(time * 0.5f) * sinf(time * 0.5f), sinf(time * 0.5f + PI) * sinf(time * 0.5f + PI), sinf(time * 0.1 + PI) * sinf(time * 0.1 + PI), 1.0f);
        //glUniform4f(cUniformLocation2, sinf(time * 0.5f + PI) * sinf(time * 0.5f + PI), sinf(time * 0.5f) * sinf(time * 0.5f), sinf(time * 0.1 + PI) * sinf(time * 0.1 + PI), 1.0f);
        
        static int lastSecond = -1;

        int currentSecond = static_cast<int>(time);

        timer += delta;
        if (timer >= 0.1f && spawnWindParticles)
        {
            timer = 0.0f;
            std::vector<WindPoint> newWindLayer = airhandler.createWindLayer(glm::vec3(-5.0f, -2.0f, 5.0f), glm::vec3(5.0f, 2.0f, 5.0f), 0.4f);
            wind.insert(wind.end(), newWindLayer.begin(), newWindLayer.end());

        }
        index = -1;
        std::vector<Vertex> windVertex;
        for (WindPoint &w : wind) {
            index++;

            w.velocity = airhandler.getInfluece(w.position) * windSpeed;

            /*
            Erst für Objekte nutzen und nicht für Tracelines
            w.velocity = airhandler.calculateDragAcceleration(
                w.velocity,
                    newVelcoity * windSpeed,
                    airDensity,  // airDensity
                    0.01f,  // area
                    0.45f,  // dragCoef
                    0.001f    // mass
                ) * delta;*/
            
            w.position += w.velocity * delta * simulationSpeed;
            if (!airhandler.isInfluenced(w.position)) {
                indiceForClear.push_back(index);
            }
            else {
                Vertex v;
                v.position = w.position;
                float t = glm::clamp(glm::length(w.velocity) / maxWindSpeed, 0.0f, 1.0f);
                v.color = glm::vec4(t, 1.0f - t, 0.0f, 1.0f);
                windVertex.push_back(v);
            }
        }
        std::sort(indiceForClear.begin(), indiceForClear.end(), std::greater<int>());
        for (auto i : indiceForClear)
            wind.erase(wind.begin() + i);
        indiceForClear.clear();

        if (displayWindPoints) {
            for (AirVertex d : airhandler.windDirection) {
                Vertex v;
                v.position = d.position;
                v.color = glm::vec4(0.5f, 0.0f, 1.0f, 1.0f);
                windVertex.push_back(v);
            }
        }

        shader_points.bind();
        glUniformMatrix4fv(modelViewProjLocationPoints, 1, GL_FALSE, &mvp[0][0]);
        VertexBuffer windBuffer(windVertex.data(), static_cast<uint32_t>(windVertex.size()));
        windBuffer.bind();
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(windVertex.size()));

        //IMGUI Zeug
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        float panelWidth = 300.0f;

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

        float tempor;

        if (ImGui::BeginTabBar("SettingsTabs"))
        {
            if (ImGui::BeginTabItem("Wind"))
            {
                ImGui::Text("Wind settings");

                const char* windTypes[] =
                {
                    "Default",
                    "Nothing",
                    "Custom",
                    "zVortex"
                };

                static int currentWind = 0;

                if (ImGui::Combo("Wind Type", &currentWind, windTypes, IM_ARRAYSIZE(windTypes)))
                {

                    if (currentWind != 2) {
                        
                        airhandler.plannedWinds(
                            static_cast<WindType>(currentWind)
                        );
                        
                    }
                }

                if (currentWind == 2) {
                    if (ImGui::CollapsingHeader("Custom Wind normal"))
                    {
                        /*
                        ImGui::SliderFloat(
                            "X",
                            &customWindDirectionX,
                            -1.0f,
                            1.0f
                        );

                        ImGui::SliderFloat(
                            "Y",
                            &customWindDirectionY,
                            -1.0f,
                            1.0f
                        );
                        ImGui::SliderFloat(
                            "Z",
                            &customWindDirectionZ,
                            -1.0f,
                            1.0f
                        );
                    }

                    airhandler.setNumbersWind(glm::vec3(customWindDirectionX, customWindDirectionY, customWindDirectionZ));
                    */

                        static char x[128] = "";
                        static char y[128] = "";
                        static char z[128] = "";

                        ImGui::InputText("X", x, IM_ARRAYSIZE(x));
                        ImGui::InputText("Y", y, IM_ARRAYSIZE(y));
                        ImGui::InputText("Z", z, IM_ARRAYSIZE(z));

                        if(ImGui::Button("refresh")) airhandler.setVariablesWind(x, y, z);

                        ImGui::Separator();
                    }
                }

                ImGui::SliderFloat(
                    "Windspeed",
                    &windSpeed,
                    0.0f,
                    maxWindSpeed
                );
               
                if (windSpeed <= 0.0f || currentWind == 1) spawnWindParticles = false;

                ImGui::Checkbox("Spawn Windparticles", &spawnWindParticles);

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Gameobjects"))
            {
                ImGui::Text("Gameobject settings");

                if (ImGui::Button("Spawn Cube")) {
                    objects.push_back(mLoader.addGameObject("Cube", glm::vec3(2.0f)));
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Simulation"))
            {
                ImGui::Text("Simulation settings");
                
                if (ImGui::Button("Reset Simulation")) {
                    resetSimulation(objects, wind);
                    time = 0;
                }

                

                if (ImGui::Button("Display Windpoints")) {
                    displayWindPoints = !displayWindPoints;
                }


                ImGui::SliderFloat(
                    "Simulation speed",
                    &simulationSpeed,
                    0.0f,
                    2.0f,
                    "%.2f",
                    ImGuiSliderFlags_Logarithmic
                );
               
                ImGui::Text("Simulation constants");
                ImGui::Separator();
                ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);
                ImGui::Text("Current Air Resolution %.001f", airResolution);
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

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

