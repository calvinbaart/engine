#ifndef _RENDER_LAYER_H_
#define _RENDER_LAYER_H_

#include "Config.h"
#include "Canvas.h"

class RenderLayer
{
private:
    static const int32_t MAX_NUM_PRIMITIVES = 8192;

    using VertexArray = array<VertexData, MAX_NUM_PRIMITIVES * 3>;
    using IndexArray = array<uint16_t, MAX_NUM_PRIMITIVES * 3>;
    using VertexArrayPtr = UPTR(VertexArray);
    using IndexArrayPtr = UPTR(IndexArray);

    Canvas* m_canvas;

    uint32_t m_vertex_buffer;
    uint32_t m_index_buffer;

    VertexArrayPtr m_vertex_data;
    IndexArrayPtr m_index_data;
    ShaderPtr m_shader;

    int32_t m_current_index;
    int32_t m_current_vertex;

    //Can't change for batching
    TexturePtr m_texture;
    bool m_scissor;
    float m_scissor_x;
    float m_scissor_y;
    float m_scissor_width;
    float m_scissor_height;
public:
    RenderLayer(Canvas* canvas, uint32_t vertex_buffer, uint32_t index_buffer);
    ~RenderLayer();

    bool draw(const vector<VertexData>& vertices, const vector<uint16_t>& indices, bool flipped_y = false);
    bool validate(TexturePtr texture, ShaderPtr shader);
    bool texture();
    bool scissor_test();

    float get_scissor_x();
    float get_scissor_y();
    float get_scissor_width();
    float get_scissor_height();

    TexturePtr get_texture();
    ShaderPtr get_shader();

    void reset(TexturePtr texture, ShaderPtr shader);
    void upload(int32_t va, int32_t ca);
};

#endif