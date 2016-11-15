#include "LogSystem.h"
#include "Window.h"
#include "Input.h"

static bool sdl_setup = false;
static bool glad_setup = false;
static int window_ref_count = 0;

Window::Window() : m_window(nullptr), m_context(nullptr), m_input(NEW_0(Input))
{
    window_ref_count++;
}

Window::~Window()
{
    close();

    window_ref_count--;
    if (window_ref_count == 0)
        deinitialise_sdl();
}

bool Window::open(const string& title, uint32_t w, uint32_t h, bool fullscreen)
{
    if (m_window != nullptr)
        return true;

    initialise_sdl();

	auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
    if (fullscreen)
    {
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (int32_t)w, (int32_t)h, flags);
    if (m_window == nullptr)
    {
        LogSystem::get()->err("Failed to open window");
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_context = SDL_GL_CreateContext(m_window);
    if (m_context == nullptr)
    {
        LogSystem::get()->err("Failed to create GL context");

        SDL_DestroyWindow(m_window);
        return false;
    }

    initialise_glad();
    glGetError(); //clear the error glad generated
    SDL_GL_SetSwapInterval(1);

    int32_t ww, wh;
	int32_t vw, vh;
    SDL_GetWindowSize(m_window, &ww, &wh);
    SDL_GL_GetDrawableSize(m_window, &vw, &vh);

    m_width = (float)ww;
    m_height = (float)wh;
    m_viewport_width = (float)vw;
    m_viewport_height = (float)vh;

    m_scale_width = m_viewport_width / m_width;
    m_scale_height = m_viewport_height / m_height;
    
    return true;
}

void Window::make_current()
{
    SDL_GL_MakeCurrent(m_window, m_context);
}

void Window::swap()
{
    SDL_GL_SwapWindow(m_window);
}

void Window::close()
{
    if(m_context)
        SDL_GL_DeleteContext(m_context);

    if(m_window)
        SDL_DestroyWindow(m_window);

    m_context = nullptr;
    m_window = nullptr;
}

float Window::get_width()
{
    return m_width;
}

float Window::get_height()
{
    return m_height;
}

float Window::get_viewport_width()
{
    return m_viewport_width;
}

float Window::get_viewport_height()
{
    return m_viewport_height;
}

InputPtr Window::get_input()
{
	return m_input;
}

SDL_Window* Window::get_handle()
{
    return m_window;
}

bool Window::event_tick()
{
    static SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            return false;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            m_input->_notify_key_press(event.key.keysym.sym);
        }
        else if (event.type == SDL_KEYUP)
        {
			m_input->_notify_key_release(event.key.keysym.sym);
        }
        else if (event.type == SDL_MOUSEMOTION)
        {
			m_input->_notify_mouse_move((float)event.motion.x, (float)event.motion.y);
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            switch (event.button.button)
            {
                case 0:
					m_input->_notify_key_press(SDLK_mouse_4);
                    break;

                case 1:
					m_input->_notify_key_press(SDLK_mouse_left);
                    break;

                case 2:
					m_input->_notify_key_press(SDLK_mouse_middle);
                    break;

                case 3:
					m_input->_notify_key_press(SDLK_mouse_right);
                    break;

                case 4:
					m_input->_notify_key_press(SDLK_mouse_5);
                    break;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONUP)
        {
            switch (event.button.button)
            {
                case 0:
					m_input->_notify_key_release(SDLK_mouse_4);
                    break;

                case 1:
					m_input->_notify_key_release(SDLK_mouse_left);
                    break;

                case 2:
					m_input->_notify_key_release(SDLK_mouse_middle);
                    break;

                case 3:
					m_input->_notify_key_release(SDLK_mouse_right);
                    break;

                case 4:
					m_input->_notify_key_release(SDLK_mouse_5);
                    break;
            }
        }
        else if (event.type == SDL_MOUSEWHEEL)
        {
			m_input->_notify_mouse_scroll((float)event.wheel.y);
        }
    }

    return true;
}

void Window::initialise_sdl()
{
    if (sdl_setup)
        return;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        LogSystem::get()->err("Failed to initialise SDL, exiting...");
        exit(0);
    }

    sdl_setup = true;
}

void Window::initialise_glad()
{
    if (glad_setup)
        return;

    gladLoadGLES2Loader(SDL_GL_GetProcAddress);
    glad_setup = true;
}

void Window::deinitialise_sdl()
{
    if (!sdl_setup)
        return;

    SDL_Quit();
    sdl_setup = false;
}
