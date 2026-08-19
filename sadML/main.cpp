#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#pragma comment(lib, "SDL3.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

#include "vertex_buffer.h"
#include "defines.h"
#include "shader.h"





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
        Vertex{-0.5f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f, 1.0f},
        Vertex{0.0f, 1.0f, 0.0f,
                0.5f, 0.0f, 0.5f, 1.0f},
        Vertex{0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    }
    };
    int numVertecies = 3;
    VertexBuffer vBuffer(verticies, numVertecies);

    

    Shader shader("basic.vertex", "basic.frag");
    shader.bind();

    bool isActive = true;
    while (isActive) {

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        vBuffer.bind();
        glDrawArrays(GL_TRIANGLES, 0, numVertecies);
        vBuffer.unBind();

        SDL_GL_SwapWindow(window);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) isActive = false;
        }
    }

    return 0;
}