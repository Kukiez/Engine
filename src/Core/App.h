#pragma once
#include <App.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <gl/glew.h>

#include "ECS/ECS.h"


class AppSystem : Stages<LevelLoadStage, DefaultStage, LevelUnloadStage> {
    FRIEND_DESCRIPTOR

    bool initSDL() {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            SDL_Delay(6000);
            return false;
        }

        window = SDL_CreateWindow("SDL2 OpenGL Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 2560, 1440, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if (!window) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            SDL_Delay(6000);
            return false;
        }

        glContext = SDL_GL_CreateContext(window);
        SDL_GL_MakeCurrent(window, glContext);
        if (!glContext) {
            std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        glewExperimental = GL_TRUE;
        GLenum glewError = glewInit();
        if (glewError != GLEW_OK) {
            std::cerr << "Error initializing GLEW! " << glewGetErrorString(glewError) << std::endl;
            SDL_GL_DeleteContext(glContext);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            SDL_GL_DeleteContext(glContext);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

        //  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);

        SDL_GL_SetSwapInterval(0);

        int width  = 0;
        int height = 0;

        SDL_GetWindowSize(window, &width, &height);
        SDL_SetRelativeMouseMode(static_cast<SDL_bool>(1));
        glViewport(0, 0, width, height);
        return true;
    }

    void destroySDL() const {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_GLContext glContext;
public:
    AppSystem() = default;

    void onLevelLoad() {
        initSDL();
    }

    void onUpdate() {
        SDL_GL_SwapWindow(window);
    }

    void onLevelUnload() const {
        destroySDL();
    }

    auto getWindow() const {
        return window;
    }
};
