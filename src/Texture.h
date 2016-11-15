#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Config.h"

class Texture
{
private:
    TextureID m_id;
    float m_width;
    float m_height;
public:
    Texture(TextureID id, float width, float height);
    ~Texture();

    TextureID get_id();
    float get_width();
    float get_height();
};

#endif