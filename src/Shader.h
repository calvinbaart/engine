#ifndef _SHADER_H_
#define _SHADER_H_

#include "Config.h"
#include "Color.h"

class Shader
{
private:
	uint32_t m_program;

    unordered_map<string, fmatrix4> m_matrices;
    unordered_map<string, TextureID> m_textures;
    unordered_map<string, float> m_float1;
    unordered_map<string, fvec2> m_float2;
    unordered_map<string, fvec3> m_float3;
    unordered_map<string, fvec4> m_float4;
public:
    Shader(uint32_t program);
    ~Shader();

    void set_uniform(string uniform, const fmatrix4& matrix);
    void set_uniform(string uniform, TexturePtr texture);
    void set_uniform(string uniform, float val);
    void set_uniform(string uniform, const fvec2& vec);
    void set_uniform(string uniform, const fvec3& vec);
    void set_uniform(string uniform, const fvec4& vec);
	void set_uniform(string uniform, const Color& col);

    void apply();

    uint32_t get_program();
};

#endif
