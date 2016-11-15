#include "Input.h"
#include "EventHandler.h"
#include "Event.h"

Input::Input() : 
    m_keys(), m_mouse_x(), m_mouse_y(), m_mouse_z(),
	m_on_key_press(), m_on_key_release(), m_on_mouse_move(), m_on_mouse_scroll()
{
}

Input::~Input()
{
}

bool Input::is_key_pressed(Key key)
{
    if (m_keys.find(key) == m_keys.end())
        return false;

    return m_keys[key];
}

bool Input::is_key_released(Key key)
{
    if (m_keys.find(key) == m_keys.end())
        return true;

    return !m_keys[key];
}

float Input::get_mouse_x()
{
    return m_mouse_x;
}

float Input::get_mouse_y()
{
    return m_mouse_y;
}

float Input::get_mouse_z()
{
    float _z = m_mouse_z;
    m_mouse_z = 0.0f;

    return _z;
}

void Input::add_on_key_press(EventHandlerPtr handler)
{
	assert(m_on_key_press.size() != 256);
	m_on_key_press.push_back(handler);
}

void Input::add_on_key_release(EventHandlerPtr handler)
{
	assert(m_on_key_release.size() != 256);
	m_on_key_release.push_back(handler);
}

void Input::add_on_mouse_move(EventHandlerPtr handler)
{
	assert(m_on_mouse_move.size() != 256);
	m_on_mouse_move.push_back(handler);
}

void Input::add_on_mouse_scroll(EventHandlerPtr handler)
{
	assert(m_on_mouse_scroll.size() != 256);
	m_on_mouse_scroll.push_back(handler);
}

void Input::remove_on_key_press(EventHandlerPtr handler)
{
	assert(m_on_key_press.size() != 256);
	
	auto it = find(m_on_key_press.begin(), m_on_key_press.end(), handler);
	assert(it != m_on_key_press.end());

	m_on_key_press.erase(it);
}

void Input::remove_on_key_release(EventHandlerPtr handler)
{
	assert(m_on_key_release.size() != 256);

	auto it = find(m_on_key_release.begin(), m_on_key_release.end(), handler);
	assert(it != m_on_key_release.end());

	m_on_key_release.erase(it);
}

void Input::remove_on_mouse_move(EventHandlerPtr handler)
{
	assert(m_on_mouse_move.size() != 256);

	auto it = find(m_on_mouse_move.begin(), m_on_mouse_move.end(), handler);
	assert(it != m_on_mouse_move.end());

	m_on_mouse_move.erase(it);
}

void Input::remove_on_mouse_scroll(EventHandlerPtr handler)
{
	assert(m_on_mouse_scroll.size() != 256);

	auto it = find(m_on_mouse_scroll.begin(), m_on_mouse_scroll.end(), handler);
	assert(it != m_on_mouse_scroll.end());

	m_on_mouse_scroll.erase(it);
}

void Input::_notify_key_press(Key key)
{
    m_keys[key] = true;
	fire_on_key_press(key);
}

void Input::_notify_key_release(Key key)
{
    m_keys[key] = false;
	fire_on_key_release(key);
}

void Input::_notify_mouse_move(float x, float y)
{
	float dx = x - m_mouse_x;
	float dy = y - m_mouse_y;

    m_mouse_x = x;
    m_mouse_y = y;

	fire_on_mouse_move(x, y, dx, dy);
}

void Input::_notify_mouse_scroll(float z)
{
    m_mouse_z = z;
}

void Input::fire_on_key_press(Key key)
{
	if (m_on_key_press.empty()) return;

	EventPtr evt = NEW_1(KeyPressEvent, key);
	for (auto& listener : m_on_key_press)
	{
		listener->fire_event(evt);
	}
}

void Input::fire_on_key_release(Key key)
{
	if (m_on_key_release.empty()) return;

	EventPtr evt = NEW_1(KeyReleaseEvent, key);
	for (auto& listener : m_on_key_release)
	{
		listener->fire_event(evt);
	}
}

void Input::fire_on_mouse_move(float x, float y, float dx, float dy)
{
	if (m_on_mouse_move.empty()) return;

	EventPtr evt = NEW_4(MouseMoveEvent, x, y, dx, dy);
	for (auto& listener : m_on_mouse_move)
	{
		listener->fire_event(evt);
	}
}

void Input::fire_on_mouse_scroll(float z)
{
	if (m_on_mouse_scroll.empty()) return;

	EventPtr evt = NEW_1(MouseScrollEvent, z);
	for (auto& listener : m_on_mouse_scroll)
	{
		listener->fire_event(evt);
	}
}
