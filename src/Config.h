#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include <unordered_map>
#include <chrono>
#include <string>
#include <stdarg.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#if __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#include <SDL_syswm.h>
#endif
#include <stdint.h>
using namespace std;

#ifdef _DEBUG
#define CHECK_GL_ERROR {auto error = glGetError();if(error != GL_NO_ERROR){LogSystem::get()->err("GL error: %i in %s@%s:%i", error, __FUNCTION__, __FILE__, __LINE__);}}
#else
#define CHECK_GL_ERROR {}
#undef glGetError
#define glGetError() GL_NO_ERROR
#endif

#define NEW_0(type) make_shared<type>()
#define NEW_1(type, A) make_shared<type>(A)
#define NEW_2(type, A, B) make_shared<type>(A, B)
#define NEW_3(type, A, B, C) make_shared<type>(A, B, C)
#define NEW_4(type, A, B, C, D) make_shared<type>(A, B, C, D)
#define UNEW_0(type) make_unique<type>()
#define UNEW_1(type, A) make_unique<type>(A)
#define UNEW_2(type, A, B) make_unique<type>(A, B)
#define UNEW_3(type, A, B, C) make_unique<type>(A, B, C)
#define UNEW_4(type, A, B, C, D) make_unique<type>(A, B, C, D)
#define PTR(type) shared_ptr<type>
#define UPTR(type) unique_ptr<type>

using Clock = chrono::high_resolution_clock;
using TimePoint = chrono::time_point<chrono::steady_clock>;
using TimeDelta = chrono::nanoseconds;

class Canvas;
class GUI;
class Input;
class LogSystem;
class Shader;
class Texture;
class Window;
class IEvent;
class EventHandler;

using CanvasPtr = PTR(Canvas);
using GUIPtr = PTR(GUI);
using InputPtr = PTR(Input);
using LogSystemPtr = PTR(LogSystem);
using ShaderPtr = PTR(Shader);
using TexturePtr = PTR(Texture);
using WindowPtr = PTR(Window);
using EventPtr = PTR(IEvent);
using EventHandlerPtr = PTR(EventHandler);

#if defined(_WIN64) || defined(__x86_64__)
using TextureID = uint64_t;
#else
using TextureID = uint32_t;
#endif

using dmatrix4 = glm::dmat4x4;
using fmatrix4 = glm::mat4x4;
using dvec2 = glm::dvec2;
using dvec3 = glm::dvec3;
using dvec4 = glm::dvec4;
using fvec2 = glm::vec2;
using fvec3 = glm::vec3;
using fvec4 = glm::vec4;
using ivec2 = glm::i32vec2;
using ivec3 = glm::i32vec3;
using ivec4 = glm::i32vec4;

#undef min
#undef max

#endif
