#include "Texture.h"

Texture::Texture(TextureID id, float width, float height) :
    m_id(id), m_width(width), m_height(height)
{
}

Texture::~Texture()
{
}

TextureID Texture::get_id()
{
    return m_id;
}

float Texture::get_width()
{
    return m_width;
}

float Texture::get_height()
{
    return m_height;
}
