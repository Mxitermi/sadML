#pragma once

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include "defines.h"
#include "camera.h";


class InputManager {
	
public:
	void handleInput(SDL_Event event) {
		if (event.type == SDL_EVENT_QUIT)
		{
			shouldQuit = true;
		} 
		else if (event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.key)
            {
            case SDLK_F3:
                enableMovement = !enableMovement;
                break;

            case SDLK_ESCAPE:
                shouldQuit = true;
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
        

	}

	bool getShouldQuit() {
		return shouldQuit;
	}

    bool getEnableMovement() {
        return enableMovement;
    }

    void move(Camera& camera, float delta, float cameraSpeed) {

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
    }


private:
	bool shouldQuit = false;
    bool enableMovement = false;
    bool moveW = false;
    bool moveA = false;
    bool moveS = false;
    bool moveD = false;
    bool moveUp = false;
    bool moveDown = false;
};