#ifndef _CANVAS_H_
#define _CANVAS_H_

#include "Config.h"
#include "Texture.h"
#include "Color.h"
#include "Shader.h"

enum class ColorFormat
{
    RGBA,
    BGRA,
    ARGB,
    ABGR
};

struct VertexData
{
	fvec2 v;
	fvec2 uv;
    uint32_t color;

    VertexData() :
		v(), uv(), color(0xFFFFFFFF)
    {
    }

    VertexData(const fvec2& _v, const fvec2& _uv, uint32_t _c) :
        v(_v), uv(_uv), color(_c)
    {
    }

    VertexData(const fvec2& p) :
		v(p), uv(), color(0xFFFFFFFF)
    {
    }

    VertexData(const fvec2& p, const fvec2& t) :
		v(p), uv(t), color(0xFFFFFFFF)
    {
    }

    VertexData(const fvec2& p, const fvec2& t, const Color& col) :
		v(p), uv(t), color(col.uint)
    {
    }
};

class RenderState
{
private:
    vector<fmatrix4> m_matrices;
    vector<Color> m_colors;
    vector<float> m_opacity;
public:
    RenderState();
    ~RenderState();

    void reset();
    void push_opacity(float opacity);
    void push_matrix(const fmatrix4& matrix);
    void push_color(const Color& color);

    const fmatrix4& matrix();
    const Color& color();
    float opacity();

    void pop_opacity();
    void pop_matrix();
    void pop_color();
};

class RenderLayer;
using RenderLayerPtr = UPTR(RenderLayer);
using RenderStatePtr = UPTR(RenderState);

class Canvas
{
private:
    vector<RenderLayerPtr> m_layers;
    vector<RenderLayerPtr> m_buffers;
    unordered_map<TextureID, TexturePtr> m_textures;

	RenderStatePtr m_state;

    ShaderPtr m_default_shader;
    ShaderPtr m_default_geom_shader;
    int32_t m_vertex_attribute;
	int32_t m_color_attribute;

    Color m_clear_color;
	float m_viewport_scale_x;
	float m_viewport_scale_y;
    float m_viewport_x;
    float m_viewport_y;
    float m_viewport_width;
    float m_viewport_height;

    float m_scissor_x;
    float m_scissor_y;
    float m_scissor_width;
    float m_scissor_height;
    bool m_scissor;

    ShaderPtr m_shader;
    ShaderPtr m_last_shader;

    uint32_t m_vao;
    bool m_setup;
public:
    Canvas();
    ~Canvas();

    void setup();

    void begin();

    void draw_line(float x1, float y1, float x2, float y2, float strength = 0.6f);
    void draw_polyline(const vector<fvec2>& points, bool closed = false, float strength = 0.6f);

    void draw(const vector<VertexData>& vertices, const vector<uint16_t>& indices, bool flipped_y = false);
    void draw(float x, float y, float w, float h, bool flipped_y = false);
    void draw(float sx, float sy, float sw, float sh, float dx, float dy, bool flipped_y = false);
    void draw(float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool flipped_y = false);
    void draw(TexturePtr texture, const vector<VertexData>& vertices, const vector<unsigned short>& indices, bool flipped_y = false);
    void draw(TexturePtr texture, float x, float y, float w, float h, bool flipped_y = false);
    void draw(TexturePtr texture, float sx, float sy, float sw, float sh, float dx, float dy, bool flipped_y = false);
    void draw(TexturePtr texture, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool flipped_y = false);
    void end();

    void set_clear_color(const Color& color);
    void set_viewport(float x, float y, float w, float h);
	void set_viewport_scaling(float x, float y);
    void set_scissor(bool enabled, float x = 0.0f, float y = 0.0f, float w = 0.0f, float h = 0.0f);
    void set_shader(ShaderPtr shader);
    bool scissor_test();

    float get_scissor_x();
    float get_scissor_y();
    float get_scissor_width();
    float get_scissor_height();

    float get_viewport_x();
    float get_viewport_y();
    float get_viewport_width();
    float get_viewport_height();
    
    TexturePtr create_texture(unsigned char* pixels, int32_t width, int32_t height, ColorFormat format = ColorFormat::RGBA);
    TexturePtr create_texture(string file);
    TexturePtr create_texture(TextureID id);
    ShaderPtr create_shader(const string& vertex, const string& fragment);

    RenderState* get_state();
private:
	RenderLayer* get_layer(TexturePtr texture, bool force = false);
};

#endif
