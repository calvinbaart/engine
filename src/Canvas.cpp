#include "Canvas.h"
#include "LogSystem.h"
#include "RenderLayer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <png.h>

using GeometryData = pair<vector<VertexData>, vector<uint16_t>>;
static int32_t intersect(fvec2 P1, fvec2 P2, fvec2 P3, fvec2 P4, fvec2& Pout)
{
    float mua = 0.0f;
    float mub = 0.0f;
    float denom = 0.0f;
    float numera = 0.0f;
    float numerb = 0.0f;
    float eps = 0.000000000001f;

    denom = (P4.y - P3.y) * (P2.x - P1.x) - (P4.x - P3.x) * (P2.y - P1.y);
    numera = (P4.x - P3.x) * (P1.y - P3.y) - (P4.y - P3.y) * (P1.x - P3.x);
    numerb = (P2.x - P1.x) * (P1.y - P3.y) - (P2.y - P1.y) * (P1.x - P3.x);
    if ((-eps < numera && numera < eps) && (-eps < numerb && numerb < eps) && (-eps < denom  && denom  < eps))
    {
        Pout.x = (P1.x + P2.x) * 0.5f;
        Pout.y = (P1.y + P2.y) * 0.5f;
        return 2; //meaning the lines coincide
    }
    if (-eps < denom  && denom  < eps)
    {
        Pout.x = 0;
        Pout.y = 0;
        return 0; //meaning lines are parallel
    }
    mua = numera / denom;
    mub = numerb / denom;
    Pout.x = P1.x + mua * (P2.x - P1.x);
    Pout.y = P1.y + mua * (P2.y - P1.y);
    bool out1 = mua < 0 || mua > 1;
    bool out2 = mub < 0 || mub > 1;

    if (out1 && out2)
    {
        return 5; //the intersection lies outside both segments
    }
    else if (out1)
    {
        return 3; //the intersection lies outside segment 1
    }
    else if (out2)
    {
        return 4; //the intersection lies outside segment 2
    }
    else
    {
        return 1; //the intersection lies inside both segments
    }
}

static GeometryData polyline(vector<fvec2> points, bool closed = false, float strength = 0.6f)
{
    if (points.size() <= 1)
        return { vector<VertexData>(), vector<uint16_t>() };

    if (points.size() == 2)
    {
        vector<fvec2> midPoints = {
            fvec2(points[0].x, points[0].y),
            fvec2(points[1].x, points[1].y)
        };

        float nx = -(points[1].y - points[0].y);
        float ny = points[1].x - points[0].x;
        float length = sqrtf((nx * nx) + (ny * ny));
        nx /= length;
        ny /= length;

		fvec2 t0(points[0].x + nx * strength, points[0].y + ny * strength);
		fvec2 mint0(points[0].x - nx * strength, points[0].y - ny * strength);
		fvec2 t2(points[1].x + nx * strength, points[1].y + ny * strength);
		fvec2 mint2(points[1].x - nx * strength, points[1].y - ny * strength);

        vector<VertexData> vertices;
        vertices.push_back(VertexData(t0));		//0
        vertices.push_back(VertexData(mint0));	//1
        vertices.push_back(VertexData(t2));		//2
        vertices.push_back(VertexData(mint2));	//3

        vector<unsigned short> indices;
        indices.push_back(0); indices.push_back(2); indices.push_back(3);
        indices.push_back(3); indices.push_back(1); indices.push_back(0);

        return { vertices, indices };
    }

    vector<fvec2> midPoints;
    vector<vector<fvec2>> anchors;

    if (closed)
    {
        for (size_t i = 0; i < points.size();i++)
        {
            fvec2 curr = points[i];
            fvec2 next = points[i == points.size() - 1 ? 0 : i + 1];
            midPoints.emplace_back(curr.x + ((next.x - curr.x) / 2.0), curr.y + ((next.y - curr.y) / 2.0));
        }

        for (size_t i = 0; i < points.size(); i++)
        {
            fvec2 previous = midPoints[i == 0 ? midPoints.size() - 1 : i - 1];
            fvec2 current = points[i];
            fvec2 next = midPoints[i];

            anchors.push_back({ previous, current, next });
        }
    }
    else
    {
        for (size_t i = 0; i < points.size() - 1; i++)
        {
            fvec2 curr = points[i];
            fvec2 next = points[i + 1];
            midPoints.emplace_back(curr.x + ((next.x - curr.x) / 2.0), curr.y + ((next.y - curr.y) / 2.0));
        }

        midPoints[0] = points[0];
        midPoints[points.size() - 2] = points[points.size() - 1];

        for (size_t i = 1; i < points.size() - 2; i++)
        {
            fvec2 previous = midPoints[i - 1];
            fvec2 current = points[i];
            fvec2 next = midPoints[i];

            anchors.push_back({ previous, current, next });
        }
    }

    vector<VertexData> vertices;
    vector<uint16_t> indices;

    for (auto& anchor : anchors)
    {
        float n0x = -(anchor[1].y - anchor[0].y);
        float n0y = anchor[1].x - anchor[0].x;
        float n2x = -(anchor[2].y - anchor[1].y);
        float n2y = anchor[2].x - anchor[1].x;
        float length0 = sqrtf((n0x * n0x) + (n0y * n0y));
        float length2 = sqrtf((n2x * n2x) + (n2y * n2y));

        n0x /= length0;
        n0y /= length0;
        n2x /= length2;
        n2y /= length2;

		fvec2 t0(anchor[0].x - n0x * strength, anchor[0].y - n0y * strength);						//0
		fvec2 mint0(anchor[0].x + n0x * strength, anchor[0].y + n0y * strength);					//1
		fvec2 t2(anchor[2].x - n2x * strength, anchor[2].y - n2y * strength);						//2
		fvec2 mint2(anchor[2].x + n2x * strength, anchor[2].y + n2y * strength);					//3
		fvec2 at(anchor[1].x - n0x * strength, anchor[1].y - n0y * strength);						//4
		fvec2 bt(anchor[1].x - n2x * strength, anchor[1].y - n2y * strength);						//5

		fvec2 vp;																					//6
        intersect(t0, at, t2, bt, vp);
		fvec2 minvp(anchor[1].x - (vp.x - anchor[1].x), anchor[1].y - (vp.y - anchor[1].y));		//7

		uint16_t currentIndex = (uint16_t)vertices.size();
        vertices.emplace_back(t0);
        vertices.emplace_back(mint0);
        vertices.emplace_back(t2);
        vertices.emplace_back(mint2);
        vertices.emplace_back(at);
        vertices.emplace_back(bt);
        vertices.emplace_back(vp);
        vertices.emplace_back(minvp);

        indices.push_back(currentIndex + 0); indices.push_back(currentIndex + 4); indices.push_back(currentIndex + 7);
        indices.push_back(currentIndex + 0); indices.push_back(currentIndex + 7); indices.push_back(currentIndex + 1);

        indices.push_back(currentIndex + 4); indices.push_back(currentIndex + 6); indices.push_back(currentIndex + 5);
        indices.push_back(currentIndex + 4); indices.push_back(currentIndex + 5); indices.push_back(currentIndex + 7);

        indices.push_back(currentIndex + 5); indices.push_back(currentIndex + 2); indices.push_back(currentIndex + 7);
        indices.push_back(currentIndex + 2); indices.push_back(currentIndex + 3); indices.push_back(currentIndex + 7);
    }

    return { vertices, indices };
}

Canvas::Canvas() : 
    m_layers(), m_state(move(UNEW_0(RenderState))), m_setup(false), m_clear_color(0.0f, 0.0f, 0.0f, 1.0f),
    m_viewport_x(0.0f), m_viewport_y(0.0f), m_viewport_width(1.0f), m_viewport_height(1.0f),
    m_textures(), m_viewport_scale_x(1.0f), m_viewport_scale_y(1.0f)
{
}

Canvas::~Canvas()
{
}

void Canvas::setup()
{
    if (m_setup) return;

    glGenVertexArrays(1, &m_vao);
    CHECK_GL_ERROR;
    glBindVertexArray(m_vao);
    CHECK_GL_ERROR;

    CHECK_GL_ERROR;
    glEnable(GL_BLEND);
    CHECK_GL_ERROR;
    glBlendEquation(GL_FUNC_ADD);
    CHECK_GL_ERROR;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CHECK_GL_ERROR;
    glEnable(GL_DEPTH_TEST);
    CHECK_GL_ERROR;
    glDepthFunc(GL_LEQUAL);
    CHECK_GL_ERROR;

	string vertexSource =
		"#version 330                                               \r\n"
		"in vec4 position;                                          \r\n"
		"in vec4 color;                                             \r\n"
		"                                                           \r\n"
		"out vec2 vTexCoord;                                        \r\n"
		"out vec4 vColor;                                           \r\n"
		"                                                           \r\n"
		"uniform mat4 projection;                                   \r\n"
        "                                                           \r\n"
        "void main(void)                                            \r\n"
        "{                                                          \r\n"
        "    vColor = color;                                        \r\n"
        "    vTexCoord = position.zw;                                \r\n"
        "                                                           \r\n"
        "    gl_Position = projection * vec4(position.xy, 0, 1);    \r\n"
        "}                                                          \r\n";

    //"precision mediump float;" +
    string fragmentSource =
        "#version 330                                               \r\n"
        "in vec2 vTexCoord;                                         \r\n"
        "in vec4 vColor;                                            \r\n"
        "out vec4 oColor;                                           \r\n"
        "                                                           \r\n"
        "uniform sampler2D tex;                                     \r\n"
        "                                                           \r\n"
        "void main(void)                                            \r\n"
        "{                                                          \r\n"
        "    vec4 color = texture(tex, vTexCoord) * vColor;         \r\n"
		"    oColor = color;                                        \r\n"
        "}                                                          \r\n";

    string geomVertexSource =
        "#version 330                                               \r\n"
        "in vec4 position;                                          \r\n"
        "in vec4 color;                                             \r\n"
        "                                                           \r\n"
        "out vec4 vColor;                                           \r\n"
        "                                                           \r\n"
        "uniform mat4 projection;                                   \r\n"
        "                                                           \r\n"
        "void main(void)                                            \r\n"
        "{                                                          \r\n"
        "    vColor = color;										\r\n"
        "                                                           \r\n"
        "    gl_Position = projection * vec4(position.xy, 0, 1);    \r\n"
        "}                                                          \r\n";

    //"precision mediump float;" +
    string geomFragmentSource =
        "#version 330                                               \r\n"
        "in vec4 vColor;                                            \r\n"
        "out vec4 oColor;                                           \r\n"
        "                                                           \r\n"
        "void main(void)                                            \r\n"
        "{                                                          \r\n"
        "    oColor = vColor;                                       \r\n"
        "}                                                          \r\n";

    m_default_shader = create_shader(vertexSource, fragmentSource);
    m_default_geom_shader = create_shader(geomVertexSource, geomFragmentSource);

    m_vertex_attribute = glGetAttribLocation(m_default_shader->get_program(), "position");
    CHECK_GL_ERROR;
    m_color_attribute = glGetAttribLocation(m_default_shader->get_program(), "color");
    CHECK_GL_ERROR;

    glEnableVertexAttribArray(m_vertex_attribute);
    CHECK_GL_ERROR;
    glEnableVertexAttribArray(m_color_attribute);
    CHECK_GL_ERROR;

    m_setup = true;
}

void Canvas::begin()
{
    setup();

    for (auto& layer : m_layers)
        m_buffers.emplace_back(move(layer));

    m_scissor = false;
	m_layers.clear();
	m_state->reset();

	if (m_viewport_scale_x != 1.0f || m_viewport_scale_y != 1.0f)
	{
		fmatrix4 mat;
		mat = glm::scale(mat, fvec3(m_viewport_scale_x, m_viewport_scale_y, 1.0f));

		m_state->push_matrix(mat);
	}
}

void Canvas::draw_line(float x1, float y1, float x2, float y2, float strength)
{
}

void Canvas::draw_polyline(const vector<fvec2>& points, bool closed, float strength)
{
    GeometryData data = polyline(points, closed, strength);
    draw(data.first, data.second);
}

void Canvas::draw(const vector<VertexData>& vertices, const vector<unsigned short>& indices, bool flipped_y)
{
    draw(nullptr, vertices, indices, flipped_y);
}

void Canvas::draw(float x, float y, float w, float h, bool flipped_y)
{
    draw(nullptr, x, y, w, h, flipped_y);
}

void Canvas::draw(float sx, float sy, float sw, float sh, float dx, float dy, bool flipped_y)
{
    draw(nullptr, sx, sy, sw, sh, dx, dy, flipped_y);
}

void Canvas::draw(float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool flipped_y)
{
    draw(nullptr, sx, sy, sw, sh, dx, dy, dw, dh, flipped_y);
}

void Canvas::draw(TexturePtr texture, const vector<VertexData>& vertices, const vector<unsigned short>& indices, bool flipped_y)
{
    get_layer(texture)->draw(vertices, indices, flipped_y);
}

void Canvas::draw(TexturePtr texture, float x, float y, float w, float h, bool flipped_y)
{
    if (texture == nullptr)
        draw(texture, 0.0f, 0.0f, w, h, x, y, w, h, flipped_y);
    else
        draw(texture, 0.0f, 0.0f, (float)texture->get_width(), (float)texture->get_height(), x, y, w, h, flipped_y);
}

void Canvas::draw(TexturePtr texture, float sx, float sy, float sw, float sh, float dx, float dy, bool flipped_y)
{
    draw(texture, sx, sy, sw, sh, dx, dy, sw, sh, flipped_y);
}

void Canvas::draw(TexturePtr texture, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool flipped_y)
{
    static vector<VertexData> vertices(4, VertexData());
    static vector<unsigned short> indices(6, 0);

    vertices[0].v.x = dx;      vertices[0].v.y = dy;
    vertices[1].v.x = dx + dw; vertices[1].v.y = dy;
    vertices[2].v.x = dx + dw; vertices[2].v.y = dy + dh;
    vertices[3].v.x = dx;      vertices[3].v.y = dy + dh;

    vertices[0].color = 0xffffffff;
    vertices[1].color = 0xffffffff;
    vertices[2].color = 0xffffffff;
    vertices[3].color = 0xffffffff;

    vertices[0].uv.x = 0.0f; vertices[0].uv.y = 0.0f;
    vertices[1].uv.x = 1.0f; vertices[1].uv.y = 0.0f;
    vertices[2].uv.x = 1.0f; vertices[2].uv.y = 1.0f;
    vertices[3].uv.x = 0.0f; vertices[3].uv.y = 1.0f;

    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 2; indices[4] = 3; indices[5] = 0;

    draw(texture, vertices, indices, flipped_y);
}

void Canvas::end()
{
    glDisable(GL_SCISSOR_TEST);

    glViewport((uint32_t)m_viewport_x, (uint32_t)m_viewport_y, (uint32_t)m_viewport_width, (uint32_t)m_viewport_height);
    CHECK_GL_ERROR;
    glClearColor(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
    CHECK_GL_ERROR;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CHECK_GL_ERROR;

	fmatrix4 projection = glm::ortho(m_viewport_x, m_viewport_width, m_viewport_y, m_viewport_height, -100.0f, 100.0f);
    for (auto& batch : m_layers)
    {
		if(batch->get_shader() != m_last_shader)
		{
			m_last_shader = batch->get_shader();
			m_last_shader->set_uniform("projection", projection);
			m_last_shader->apply();
		}

        batch->upload(m_vertex_attribute, m_color_attribute);
    }
}

void Canvas::set_clear_color(const Color& color)
{
    m_clear_color = color;
}

void Canvas::set_viewport(float x, float y, float w, float h)
{
    m_viewport_x = x;
    m_viewport_y = y;
    m_viewport_width = w;
    m_viewport_height = h;
}

void Canvas::set_viewport_scaling(float x, float y)
{
	m_viewport_scale_x = x;
	m_viewport_scale_y = y;
}

void Canvas::set_scissor(bool enabled, float x, float y, float w, float h)
{
    m_scissor = enabled;
    m_scissor_x = x;
    m_scissor_y = y;
    m_scissor_width = w;
    m_scissor_height = h;
}

void Canvas::set_shader(ShaderPtr shader)
{
    m_shader = shader;
}

bool Canvas::scissor_test()
{
    return m_scissor;
}

float Canvas::get_scissor_x()
{
    return m_scissor_x;
}

float Canvas::get_scissor_y()
{
    return m_scissor_y;
}

float Canvas::get_scissor_width()
{
    return m_scissor_width;
}

float Canvas::get_scissor_height()
{
    return m_scissor_height;
}

float Canvas::get_viewport_x()
{
    return m_viewport_x;
}

float Canvas::get_viewport_y()
{
    return m_viewport_y;
}

float Canvas::get_viewport_width()
{
    return m_viewport_width;
}

float Canvas::get_viewport_height()
{
    return m_viewport_height;
}

TexturePtr Canvas::create_texture(unsigned char* pixels, int32_t width, int32_t height, ColorFormat format)
{
    uint32_t texture;
    glGenTextures(1, &texture);
    CHECK_GL_ERROR;
    glBindTexture(GL_TEXTURE_2D, texture);
    CHECK_GL_ERROR;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    CHECK_GL_ERROR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    CHECK_GL_ERROR;

    //todo: actually use format

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    CHECK_GL_ERROR;

    TexturePtr p = NEW_3(Texture, texture, (float)width, (float)height);
    m_textures[texture] = p;

    return p;
}

TexturePtr Canvas::create_texture(string file)
{
    FILE *fp = nullptr;
    
#ifdef _WIN32
    fopen_s(&fp, file.c_str(), "rb");
#else
    fp = fopen(file.c_str(), "rb");
#endif
    
    if (fp == nullptr)
        return nullptr;

    char header[8];
    fread(header, 1, 8, fp);

    if (png_sig_cmp((png_const_bytep)header, 0, 8))
        return nullptr;

    auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr)
        return nullptr;

    auto info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr)
    {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return nullptr;
    }

    //todo: add error handlers

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    auto width = png_get_image_width(png_ptr, info_ptr);
    auto height = png_get_image_height(png_ptr, info_ptr);
    auto channels = png_get_channels(png_ptr, info_ptr);

    png_read_update_info(png_ptr, info_ptr);

    auto tmp = png_get_rowbytes(png_ptr, info_ptr);
    unsigned char* pixeldata = new unsigned char[height * tmp];
    png_bytep* row_pointers = new png_bytep[height];

    for (size_t y = 0; y < height; y++)
        row_pointers[y] = pixeldata + ((height - y - 1) * tmp);

    png_read_image(png_ptr, row_pointers);
    fclose(fp);

    if (channels == 3)
    {
        unsigned char* pixeldata2 = new unsigned char[width * height * 4];
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                pixeldata2[((y * width) + x) * 4 + 0] = pixeldata[((y * width) + x) * 3 + 0];
                pixeldata2[((y * width) + x) * 4 + 1] = pixeldata[((y * width) + x) * 3 + 1];
                pixeldata2[((y * width) + x) * 4 + 2] = pixeldata[((y * width) + x) * 3 + 2];
                pixeldata2[((y * width) + x) * 4 + 3] = 255;
            }
        }

        delete[] pixeldata;
        pixeldata = pixeldata2;
    }

    TexturePtr tex = create_texture(pixeldata, width, height);

    delete[] pixeldata;
    delete[] row_pointers;

    png_destroy_info_struct((png_structp)info_ptr, nullptr);
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);

    return tex;
}

TexturePtr Canvas::create_texture(TextureID id)
{
    if (m_textures.find(id) == m_textures.end())
        return nullptr;

    return m_textures[id];
}

ShaderPtr Canvas::create_shader(const string& vertex, const string& fragment)
{
    auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    CHECK_GL_ERROR;

    auto vsource = vertex.c_str();
    int32_t vlength = (int32_t)vertex.length();

    glShaderSource(vertex_shader, 1, &vsource, &vlength);
    CHECK_GL_ERROR;
    glCompileShader(vertex_shader);
    CHECK_GL_ERROR;

	int32_t success = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    CHECK_GL_ERROR;

    if (success == GL_FALSE)
    {
		int32_t logSize = 0;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logSize);
        CHECK_GL_ERROR;

        static char log[2048];
        memset(log, 0, 2048);

        glGetShaderInfoLog(vertex_shader, 2048, &logSize, log);
        CHECK_GL_ERROR;
        glDeleteShader(vertex_shader);
        CHECK_GL_ERROR;

        LogSystem::get()->err("Failed to vertex shader: %s", log);
        return nullptr;
    }

    auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    CHECK_GL_ERROR;

    auto fsource = fragment.c_str();
	int32_t flength = (int32_t)fragment.length();

    glShaderSource(fragment_shader, 1, &fsource, &flength);
    CHECK_GL_ERROR;
    glCompileShader(fragment_shader);
    CHECK_GL_ERROR;

    success = 0;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    CHECK_GL_ERROR;

    if (success == GL_FALSE)
    {
		int32_t logSize = 0;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logSize);
        CHECK_GL_ERROR;

        static char log[2048];
        memset(log, 0, 2048);

        glGetShaderInfoLog(fragment_shader, 2048, &logSize, log);
        CHECK_GL_ERROR;
        glDeleteShader(vertex_shader);
        CHECK_GL_ERROR;
        glDeleteShader(fragment_shader);
        CHECK_GL_ERROR;

        LogSystem::get()->err("Failed to fragment shader: %s", log);
        return nullptr;
    }

    uint32_t program = glCreateProgram();
    CHECK_GL_ERROR;
    glAttachShader(program, vertex_shader);
    CHECK_GL_ERROR;
    glAttachShader(program, fragment_shader);
    CHECK_GL_ERROR;

    glBindAttribLocation(program, 0, "position");
    CHECK_GL_ERROR;
    glBindAttribLocation(program, 1, "color");
    CHECK_GL_ERROR;
    glBindAttribLocation(program, 2, "texcoord");
    CHECK_GL_ERROR;

    glLinkProgram(program);
    CHECK_GL_ERROR;

    success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    CHECK_GL_ERROR;

    if (success == GL_FALSE)
    {
		int32_t maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        CHECK_GL_ERROR;

        static char log[2048];
        memset(log, 0, 2048);

        glGetProgramInfoLog(program, 2048, &maxLength, log);
        CHECK_GL_ERROR;
        glDeleteProgram(program);
        CHECK_GL_ERROR;
        glDeleteShader(vertex_shader);
        CHECK_GL_ERROR;
        glDeleteShader(fragment_shader);
        CHECK_GL_ERROR;

        LogSystem::get()->err("Failed to link shader: %s", log);
        return nullptr;
    }
    
    glDetachShader(program, vertex_shader);
    CHECK_GL_ERROR;
    glDetachShader(program, fragment_shader);
    CHECK_GL_ERROR;

    return NEW_1(Shader, program);
}

RenderState* Canvas::get_state()
{
    return m_state.get();
}

RenderLayer* Canvas::get_layer(TexturePtr texture, bool force)
{
    ShaderPtr shader = m_shader;
    if (shader == nullptr)
    {
        if (texture == nullptr)
            shader = m_default_geom_shader;
        else
            shader = m_default_shader;
    }

    if (m_layers.empty() || !m_layers.back()->validate(texture, shader) || force)
    {
        if (!m_buffers.empty())
        {
            auto& layer = m_buffers[0];
			layer->reset(texture, shader);

			m_layers.push_back(move(layer));
            m_buffers.erase(m_buffers.begin());

            return m_layers.back().get();
        }
        else
        {
            uint32_t buffers[2];
            glGenBuffers(2, buffers);
            CHECK_GL_ERROR;

			m_layers.push_back(UNEW_3(RenderLayer, this, buffers[0], buffers[1]));

            auto& layer = m_layers.back();
			layer->reset(texture, shader);

            return layer.get();
        }
    }

    return m_layers.back().get();
}

RenderState::RenderState() : m_opacity(), m_colors(), m_matrices()
{
}

RenderState::~RenderState()
{
}

void RenderState::reset()
{
    m_opacity.clear();
    m_colors.clear();
    m_matrices.clear();
}

void RenderState::push_opacity(float opacity)
{
    float last = m_opacity.size() == 0 ? 1.0f : m_opacity[m_opacity.size() - 1];
    m_opacity.push_back(last * opacity);
}

void RenderState::push_matrix(const fmatrix4& matrix)
{
	fmatrix4 last = m_matrices.size() == 0 ? fmatrix4() : m_matrices[m_matrices.size() - 1];
    m_matrices.push_back(last * matrix);
}

void RenderState::push_color(const Color& color)
{
    Color last = m_colors.size() == 0 ? Color::White() : m_colors[m_colors.size() - 1];
    m_colors.emplace_back(last.r * color.r, last.g * color.g, last.b * color.b, last.a * color.a);
}

const fmatrix4& RenderState::matrix()
{
    static fmatrix4 identity;
    if (m_matrices.size() == 0) return identity;

    return m_matrices[m_matrices.size() - 1];
}

const Color& RenderState::color()
{
    static Color base = Color::White();
    if (m_colors.size() == 0) return base;

    return m_colors[m_colors.size() - 1];
}

float RenderState::opacity()
{
    if (m_opacity.size() == 0) return 1.0f;
    return m_opacity[m_opacity.size() - 1];
}

void RenderState::pop_opacity()
{
    if (m_opacity.size() == 0) return;
    m_opacity.pop_back();
}

void RenderState::pop_matrix()
{
    if (m_matrices.size() == 0) return;
    m_matrices.pop_back();
}

void RenderState::pop_color()
{
    if (m_colors.size() == 0) return;
    m_colors.pop_back();
}
