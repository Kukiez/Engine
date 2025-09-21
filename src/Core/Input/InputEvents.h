#pragma once
#include "ECS/Component/Component.h"
#include <SDL2/SDL.h>

struct KeyPressEvent : EventComponent {
    SDL_Keycode key;

    KeyPressEvent(const SDL_Keycode key) : key(key) {}

    bool operator == (const SDL_Keycode key) const {
        return this->key == key;
    }
};

struct MouseMotionEvent : EventComponent {
    int x = 0;
    int y = 0;
    int xrel = 0;
    int yrel = 0;

    MouseMotionEvent() = default;
    MouseMotionEvent(const int x, const int y, const int xrel, const int yrel)
        : x(x), y(y), xrel(xrel), yrel(yrel) {}
};

struct MouseButtonEvent : EventComponent {

};