#pragma once

#include "Config.h"

enum
{
    SDLK_mouse_left = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APP2 + 1),
    SDLK_mouse_right = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APP2 + 2),
    SDLK_mouse_middle = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APP2 + 3),
    SDLK_mouse_4 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APP2 + 4),
    SDLK_mouse_5 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APP2 + 5)
};

using Key = int;
class Input
{
private:
    unordered_map<Key, bool> m_keys;
    unordered_map<Key, bool> m_keys_released;
    vector<EventHandlerPtr> m_on_key_press;
    vector<EventHandlerPtr> m_on_key_release;
    vector<EventHandlerPtr> m_on_mouse_move;
    vector<EventHandlerPtr> m_on_mouse_scroll;

    float m_mouse_x;
    float m_mouse_y;
    float m_mouse_z;
public:
    Input();
    ~Input();

    bool is_key_pressed(Key key);
    bool is_key_released(Key key);

    float get_mouse_x();
    float get_mouse_y();
    float get_mouse_z();

    void add_on_key_press(EventHandlerPtr handler);
    void add_on_key_release(EventHandlerPtr handler);
    void add_on_mouse_move(EventHandlerPtr handler);
    void add_on_mouse_scroll(EventHandlerPtr handler);

    void remove_on_key_press(EventHandlerPtr handler);
    void remove_on_key_release(EventHandlerPtr handler);
    void remove_on_mouse_move(EventHandlerPtr handler);
    void remove_on_mouse_scroll(EventHandlerPtr handler);
private:
    friend class Window;

    void _notify_key_press(Key key);
    void _notify_key_release(Key key);
    void _notify_mouse_move(float x, float y);
    void _notify_mouse_scroll(float z);

    void fire_on_key_press(Key key);
    void fire_on_key_release(Key key);
    void fire_on_mouse_move(float x, float y, float dx, float dy);
    void fire_on_mouse_scroll(float z);
};

