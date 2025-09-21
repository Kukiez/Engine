#pragma once
#include <ECS/Level/Views/LevelUpdateView.h>

#include "InputEvents.h"
#include <Core/App.h>
#include <Renderer/Common.h>

#include "Renderer/GeometrySystem.h"
#include "SDL2/SDL_events.h"

struct InputOut {
    ecs::frame_vector<KeyPressEvent> keyPressEvents;
    ecs::frame_vector<MouseMotionEvent> mouseMotionEvents;
};

struct InputSystem {
    struct InputReceiver : Stages<LevelInStage> {
        static InputOut onLevelIn(LevelInView<InputReceiver> view) {
            assert(_CrtCheckMemory());
            auto retEvent = [&]<typename Ev>() {
                assert(_CrtCheckMemory());
                auto events = view.read<Ev>();
                auto ret = view.allocate<std::vector, Ev>(events.length());
                assert(_CrtCheckMemory());
                ret.insert(ret.end(), events);
                return ret;
            };

            return {
                retEvent.operator()<KeyPressEvent>(),
                retEvent.operator()<MouseMotionEvent>()
            };
        }
    };

    struct InputPoller {
        std::unordered_set<SDL_Keycode> keys;
        MouseMotionEvent mouseMotionEvent;
        mem::vector<MouseMotionEvent> mouseWheelEvents;
        mem::vector<KeyPressEvent> keyPressEvents;

        void readInput() {
            SDL_Event event;

            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        std::exit(1);
                    case SDL_KEYDOWN:
                        keys.emplace(event.key.keysym.sym);
                        break;
                    case SDL_KEYUP:
                        keys.erase(event.key.keysym.sym);
                        break;
                    case SDL_TEXTINPUT:
                        break;
                    case SDL_MOUSEMOTION:
                        mouseMotionEvent.x = event.motion.x;
                        mouseMotionEvent.y = event.motion.y;
                        mouseMotionEvent.xrel += event.motion.xrel;
                        mouseMotionEvent.yrel += event.motion.yrel;
                        break;
                    case SDL_MOUSEWHEEL:
                        mouseWheelEvents.emplace_back(
                            event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel
                        );
                        break;
                    default: break;
                }
            }
        }

        void onUpdate(LevelUpdateView<InputPoller> level) {
            readInput();
            for (auto key : keys) {
                keyPressEvents.emplace_back(key);
            }
        }

        void onLevelOut(LevelOutView<InputPoller> view) {
            view.send(keyPressEvents);

            if (mouseMotionEvent.xrel != 0 || mouseMotionEvent.yrel != 0)
                view.send<MouseMotionEvent>(mouseMotionEvent);

            view.send(mouseWheelEvents);

            keyPressEvents.clear();
            mouseMotionEvent.xrel = 0;
            mouseMotionEvent.yrel = 0;
            mouseWheelEvents.clear();
        }
    };
};

using InputIn = In<LevelInStage, InputSystem::InputReceiver>;