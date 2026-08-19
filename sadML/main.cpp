#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include "../dependencies/glm-1.0.3/glm/glm/glm.hpp"

#pragma comment(lib, "SDL3.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

#include "vertex_buffer.h"
#include "defines.h"
#include "shader.h"
#include "Index_buffer.h"
#define _USE_MATH_DEFINES 
#include <cmath>

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
    
    window = SDL_CreateWindow("SAD", 800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "Error with GLew: " << glewGetErrorString(err) << std::endl;
        std::cin.get();
        return -1;
    }
    
    std::cout << "Version of local OpenGL: " << glGetString(GL_VERSION) << std::endl;

    Vertex verticies[] = {
        Vertex{-0.5f, -0.5f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
        Vertex{-0.4f, 0.5f, 0.0f,
                0.5f, 0.0f, 0.5f, 1.0f},
        Vertex{0.5f, -0.5f, 0.0f,
        0.5f, 0.0f, 0.5f, 1.0f},
        Vertex{0.5f, 0.5f, 0.0f,
        0.0f, 0.0f, 1.0f, 1.0f}
    };
    int numVertecies = 3;

    uint32_t indices[] = {
        0, 1, 2
    };
    uint32_t numIndices = 3;

    IndexBuffer indexBuffer(indices, numIndices, sizeof(indices[0]));
    VertexBuffer vBuffer(verticies, numVertecies);
    vBuffer.unBind();

    Shader shader("basic.vertex", "basic.frag");
    shader.bind();

    uint64_t perfCounterFrequency = SDL_GetPerformanceFrequency();
    uint64_t lastCounter = SDL_GetPerformanceCounter();
    float delta = 0.0f;

    int cUniformLocation1 = glGetUniformLocation(shader.getShaderId(), "u_color1");
    glUniform4f(cUniformLocation1, 1.0f, 0.0f, 0.0f, 1.0f);
    int cUniformLocation2 = glGetUniformLocation(shader.getShaderId(), "u_color2");
    glUniform4f(cUniformLocation2, 0.0f, 1.0f, 0.0f, 1.0f);


    float time = 0.0f;

    bool isActive = true;
    while (isActive) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        time += delta;
        glUniform4f(cUniformLocation1, sinf(time * 0.5f) * sinf(time * 0.5f), sinf(time * 0.5f + PI) * sinf(time * 0.5f + PI), sinf(time * 0.1 + PI) * sinf(time * 0.1 + PI), 1.0f);
        glUniform4f(cUniformLocation2, sinf(time * 0.5f + PI) * sinf(time * 0.5f + PI), sinf(time * 0.5f) * sinf(time * 0.5f), sinf(time * 0.1 + PI) * sinf(time * 0.1 + PI), 1.0f);

        vBuffer.bind();
        indexBuffer.bind();
        
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
        
        indexBuffer.unBind();
        vBuffer.unBind();

        SDL_GL_SwapWindow(window);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) isActive = false;
        }

        uint64_t endCounter = SDL_GetPerformanceCounter();
        uint64_t counterElapsed = endCounter - lastCounter;
        delta = ((float)counterElapsed) / (float)perfCounterFrequency;
        lastCounter = endCounter;
    }

    return 0;
}