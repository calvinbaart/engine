#include "Shader.h"
#include "LogSystem.h"
#include "Texture.h"
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(unsigned int program) : m_program(program)
{
    set_uniform("tex", (TexturePtr)nullptr);
    set_uniform("projection", fmatrix4());
}

Shader::~Shader()
{
}

void Shader::set_uniform(string uniform, const fmatrix4& matrix)
{
    m_matrices[uniform] = matrix;
}

void Shader::set_uniform(string uniform, TexturePtr texture)
{
    m_textures[uniform] = (texture == nullptr) ? 0 : texture->get_id();
}

void Shader::set_uniform(string uniform, float val)
{
    m_float1[uniform] = val;
}

void Shader::set_uniform(string uniform, const fvec2& vec)
{
    m_float2[uniform] = vec;
}

void Shader::set_uniform(string uniform, const fvec3& vec)
{
    m_float3[uniform] = vec;
}

void Shader::set_uniform(string uniform, const fvec4& vec)
{
    m_float4[uniform] = vec;
}

void Shader::set_uniform(string uniform, const Color& col)
{
    set_uniform(uniform, fvec4(col.r, col.g, col.b, col.a));
}

void Shader::apply()
{
    glUseProgram(m_program);
    CHECK_GL_ERROR;

    for (auto it = m_matrices.begin(); it != m_matrices.end(); ++it)
    {
        glUniformMatrix4fv(glGetUniformLocation(m_program, it->first.c_str()), 1, false, glm::value_ptr(it->second));
        CHECK_GL_ERROR;
    }

    for (auto it = m_textures.begin(); it != m_textures.end(); ++it)
    {
        glUniform1i(glGetUniformLocation(m_program, it->first.c_str()), (GLuint)it->second);
        CHECK_GL_ERROR;
    }

    for (auto it = m_float1.begin(); it != m_float1.end(); ++it)
    {
        glUniform1f(glGetUniformLocation(m_program, it->first.c_str()), it->second);
        CHECK_GL_ERROR;
    }

    for (auto it = m_float2.begin(); it != m_float2.end(); ++it)
    {
        glUniform2f(glGetUniformLocation(m_program, it->first.c_str()), it->second.x, it->second.y);
        CHECK_GL_ERROR;
    }

    for (auto it = m_float3.begin(); it != m_float3.end(); ++it)
    {
        glUniform3f(glGetUniformLocation(m_program, it->first.c_str()), it->second.x, it->second.y, it->second.z);
        CHECK_GL_ERROR;
    }

    for (auto it = m_float4.begin(); it != m_float4.end(); ++it)
    {
        glUniform4f(glGetUniformLocation(m_program, it->first.c_str()), it->second.x, it->second.y, it->second.z, it->second.w);
        CHECK_GL_ERROR;
    }
}

unsigned int Shader::get_program()
{
    return m_program;
}
