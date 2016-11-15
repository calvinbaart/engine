#ifndef _EVENT_H_
#define _EVENT_H_

#include "Config.h"

namespace Event
{
	const string MOUSEMOVE = "mouse_move";
	const string MOUSESCROLL = "mouse_scroll";
	const string KEYPRESS = "key_press";
	const string KEYRELEASE = "key_release";
}

class IEvent
{
public:
	virtual string type() = 0;
};

class MouseMoveEvent : public IEvent
{
private:
	float m_x;
	float m_y;
	float m_dx;
	float m_dy;
public:
	MouseMoveEvent(float x, float y, float dx, float dy) :
		m_x(x), m_y(y), m_dx(dx), m_dy(dy)
	{
	}

	string type() override { return Event::MOUSEMOVE; }

	inline float x()
	{
		return m_x;
	}

	inline float y()
	{
		return m_y;
	}

	inline float dx()
	{
		return m_dx;
	}

	inline float dy()
	{
		return m_dy;
	}
};

class MouseScrollEvent : public IEvent
{
private:
	float m_z;
public:
	MouseScrollEvent(float z) :
		m_z(z)
	{
	}

	string type() override { return Event::MOUSESCROLL; }

	inline float z()
	{
		return m_z;
	}
};

class KeyPressEvent : public IEvent
{
private:
	Key m_key;
public:
	KeyPressEvent(Key key) :
		m_key(key)
	{
	}

	string type() override { return Event::KEYPRESS; }

	inline Key key()
	{
		return m_key;
	}
};

class KeyReleaseEvent : public IEvent
{
private:
	Key m_key;
public:
	KeyReleaseEvent(Key key) :
		m_key(key)
	{
	}

	string type() override { return Event::KEYRELEASE; }

	inline Key key()
	{
		return m_key;
	}
};
#endif