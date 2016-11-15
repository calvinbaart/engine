#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "Config.h"

struct SDL_Window;
class Window
{
private:
    SDL_Window* m_window;
	InputPtr m_input;
    void* m_context;

    float m_width;
    float m_height;
    float m_viewport_width;
    float m_viewport_height;

    float m_scale_width;
    float m_scale_height;
public:
    Window();
    ~Window();

    bool open(const string& title, uint32_t w, uint32_t h, bool fullscreen = false);
    void make_current();
    void swap();
    void close();

    float get_width();
    float get_height();
    float get_viewport_width();
    float get_viewport_height();
	InputPtr get_input();
    SDL_Window* get_handle();

    bool event_tick();
private:
    void initialise_sdl();
    void initialise_glad();
    void deinitialise_sdl();
};

#endif
