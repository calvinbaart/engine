#ifndef _COLOR_H_
#define _COLOR_H_

#include "Config.h"

#define COL32_R_SHIFT    0
#define COL32_G_SHIFT    8
#define COL32_B_SHIFT    16
#define COL32_A_SHIFT    24
#define F32_TO_INT8_SAT(_VAL) ((int32_t)(_VAL * 255.0f + 0.5f))

struct Color
{
    float r, g, b, a;
	uint32_t uint;

    Color(float _r, float _g, float _b, float _a = 1.0f) : 
        r(_r), g(_g), b(_b), a(_a)
    {
        build_uint();
    }

    Color(uint32_t col) :
        uint(col)
    {
        extract_uint();
    }

	bool operator==(const Color& rhs)
	{
		return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
	}

	bool operator!=(const Color& rhs)
	{
		return r != rhs.r || g != rhs.g || b != rhs.b || a != rhs.a;
	}

    inline void extract_uint()
    {
        float s = 1.0f / 255.0f;
        r = ((uint >> COL32_R_SHIFT) & 0xFF) * s;
        g = ((uint >> COL32_G_SHIFT) & 0xFF) * s;
        b = ((uint >> COL32_B_SHIFT) & 0xFF) * s;
        a = ((uint >> COL32_A_SHIFT) & 0xFF) * s;
    }

    inline void build_uint()
    {
		uint32_t _a = ((uint32_t)F32_TO_INT8_SAT(a)) << COL32_A_SHIFT;
		uint32_t _b = ((uint32_t)F32_TO_INT8_SAT(b)) << COL32_B_SHIFT;
		uint32_t _g = ((uint32_t)F32_TO_INT8_SAT(g)) << COL32_G_SHIFT;
		uint32_t _r = ((uint32_t)F32_TO_INT8_SAT(r)) << COL32_R_SHIFT;

        uint = _r | _g | _b | _a;
    }

    inline Color operator*(const Color& rhs)
    {
        return Color(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a);
    }

	inline Color operator*(const float& rhs)
	{
		return{ r * rhs, g * rhs, b * rhs, a * rhs };
	}

	inline Color operator*(const uint32_t& rhs)
	{
		float s = 1.0f / 255.0f;
		float r = ((rhs >> IM_COL32_R_SHIFT) & 0xFF) * s;
		float g = ((rhs >> IM_COL32_G_SHIFT) & 0xFF) * s;
		float b = ((rhs >> IM_COL32_B_SHIFT) & 0xFF) * s;
		float a = ((rhs >> IM_COL32_A_SHIFT) & 0xFF) * s;

		return{ this->r * r, this->g * g, this->b * b, this->a * a };
	}

    inline Color& operator*=(const float& rhs)
    {
        a *= rhs;
        r *= rhs;
        g *= rhs;
        b *= rhs;

        build_uint();
        return *this;
    }

    static inline Color White()
    {
        return Color(1.0f, 1.0f, 1.0f, 1.0f);
    }

    static inline Color Red()
    {
        return Color(1.0f, 0.0f, 0.0f, 1.0f);
    }

    static inline Color Green()
    {
        return Color(0.0f, 1.0f, 0.0f, 1.0f);
    }

    static inline Color Blue()
    {
        return Color(0.0f, 0.0f, 1.0f, 1.0f);
    }
};

#endif
