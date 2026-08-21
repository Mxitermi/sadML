#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>

#include "../dependencies/glm-1.0.3/glm/glm/glm.hpp"
#include "../dependencies/glm-1.0.3/glm/glm/ext/matrix_transform.hpp"
#include "../dependencies/glm-1.0.3/glm/glm/gtc/matrix_transform.hpp"

#pragma comment(lib, "SDL3.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "freetype.lib")

#include "vertex_buffer.h"
#include "defines.h"
#include "shader.h"
#include "Index_buffer.h"
#define _USE_MATH_DEFINES 
#include <cmath>
#include "camera.h";
#include "InputManager.h";
#include "model_loader.h"

constexpr float PI = 3.14159265358979323846f / 2;


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
    
    window = SDL_CreateWindow("SAD", 800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "Error with GLew: " << glewGetErrorString(err) << std::endl;
        std::cin.get();
        return -1;
    }
    
    std::cout << "Version of local OpenGL: " << glGetString(GL_VERSION) << std::endl;

    Vertex fich[] = {
        Vertex{-1.0f, -0.5f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
        Vertex{-0.75f, -0.75f, 0.0f,
                0.5f, 0.0f, 0.5f, 1.0f},
        Vertex{-0.75f, 0.5f, 0.0f,
                0.5f, 0.0f, 0.5f, 1.0f},
        Vertex{-1.0f, 0.5f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
        Vertex{-0.75f, 0.75f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
                Vertex{0.0f, 0.75f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
                Vertex{0.25f, 0.5f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
                Vertex{0.25f, -0.5f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
            Vertex{0.0f, -0.75f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
            Vertex{0.4f, 0.5f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
            Vertex{0.4f, -0.5f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
            Vertex{1.0f, 1.0f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
            Vertex{1.0f, 0.5f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
            Vertex{1.0f, -1.0f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
    };
    

    ModelLoader mLoader;

    VertexCombo test = mLoader.loadObjFromFile("models/monkey.obj");

    Vertex* verticies = test.vertices.data();
    int numVertecies = test.numVertices;

    uint32_t* indices = test.indices.data();
    uint32_t numIndices = test.numIndices;

    IndexBuffer indexBuffer(indices, numIndices, sizeof(indices[0]));
    VertexBuffer vBuffer(verticies, numVertecies);
    vBuffer.unBind();

    Shader shader("basic.vertex", "basic.frag");
    shader.bind();

    uint64_t perfCounterFrequency = SDL_GetPerformanceFrequency();
    uint64_t lastCounter = SDL_GetPerformanceCounter();
    float delta = 0.0f;

    int cUniformLocation1 = glGetUniformLocation(shader.getShaderId(), "u_color1");
    int cUniformLocation2 = glGetUniformLocation(shader.getShaderId(), "u_color2");
    int modelViewProjLocation = glGetUniformLocation(shader.getShaderId(), "u_modelViewProj");

    glm::mat4 model = glm::mat4(1.0f);
    
    Camera camera(90.0f, 800.0f, 600.0f);
    camera.translate(glm::vec3(0.0f, 0.0f, 5.0f));
    camera.update();

    glm::mat4 modelViewProj = camera.getViewProj() * model;

    InputManager inputManager;

    bool enableMovement = false;
    float cameraSpeed = 7.0f;
    
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

    glEnable(GL_CULL_FACE); //Aufpassen wenn selber dreiecke erstellen wegen windingorder
    glEnable(GL_DEPTH_TEST);

    float time = 0.0f;
    bool isActive = true;
    while (isActive) {

        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            inputManager.handleInput(event);
            if (event.type == SDL_EVENT_MOUSE_MOTION && enableMovement) {
                camera.onMouseMoved(event.motion.xrel, event.motion.yrel);
            }
            
        }
        time += delta;
        
        inputManager.move(camera, delta, cameraSpeed);

        isActive = !inputManager.getShouldQuit();
        enableMovement = inputManager.getEnableMovement();
        
        if (enableMovement) {
            glClearColor(0.05f, 0.07f, 0.05f, 1.0f);
            SDL_SetWindowRelativeMouseMode(window, true);
        }
        else {
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            SDL_SetWindowRelativeMouseMode(window, false);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        model = glm::rotate(model, 2.5f * delta, glm::vec3(0, 1, 0));
        modelViewProj = camera.getViewProj() * model;

        //glUniform4f(cUniformLocation1, sinf(time * 0.5f) * sinf(time * 0.5f), sinf(time * 0.5f + PI) * sinf(time * 0.5f + PI), sinf(time * 0.1 + PI) * sinf(time * 0.1 + PI), 1.0f);
        //glUniform4f(cUniformLocation2, sinf(time * 0.5f + PI) * sinf(time * 0.5f + PI), sinf(time * 0.5f) * sinf(time * 0.5f), sinf(time * 0.1 + PI) * sinf(time * 0.1 + PI), 1.0f);
        glUniformMatrix4fv(modelViewProjLocation, 1, GL_FALSE, &modelViewProj[0][0]);

        vBuffer.bind();
        indexBuffer.bind();
        
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
        
        indexBuffer.unBind();
        vBuffer.unBind();


        SDL_GL_SwapWindow(window);

        uint64_t endCounter = SDL_GetPerformanceCounter();
        uint64_t counterElapsed = endCounter - lastCounter;
        delta = ((float)counterElapsed) / (float)perfCounterFrequency;
        lastCounter = endCounter;
    }

    return 0;
}

