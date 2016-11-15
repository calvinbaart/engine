#include "RenderLayer.h"
#include "LogSystem.h"

RenderLayer::RenderLayer(Canvas* canvas, unsigned int vertex_buffer, unsigned int index_buffer) :
	m_canvas(canvas), m_texture(), m_vertex_buffer(vertex_buffer), m_index_buffer(index_buffer), m_current_index(), m_current_vertex(),
	m_scissor(), m_scissor_x(), m_scissor_y(), m_scissor_width(), m_scissor_height()
{
	m_vertex_data = UNEW_0(VertexArray);
	m_index_data = UNEW_0(IndexArray);
}

RenderLayer::~RenderLayer()
{
}

bool RenderLayer::draw(const vector<VertexData>& vertices, const vector<unsigned short>& indices, bool flipped_y)
{
	auto vsz = vertices.size();
	auto isz = indices.size();

	if (vsz == 0 || isz == 0)
		return true;

	auto color = m_canvas->get_state()->color();
	auto transform = m_canvas->get_state()->matrix();
	auto opacity = m_canvas->get_state()->opacity();

	if (m_current_vertex + vsz >= MAX_NUM_PRIMITIVES * 3 || m_current_index + floor(isz / 3.0f) >= MAX_NUM_PRIMITIVES)
		return false;

	int baseIndex = m_current_vertex;
	int offset = m_current_vertex;

	float _00 = transform[0][0];
	float _01 = transform[0][1];
	float _10 = transform[1][0];
	float _11 = transform[1][1];
	float _30 = transform[3][0];
	float _31 = transform[3][1];

	auto* data = &(*m_vertex_data)[0];
	auto* verts = vertices.data();
	for (int i = 0; i < vsz; i++)
	{
		auto& vert = verts[i];
		auto& target = data[offset++];

		target.v.x = _00 * vert.v.x + _10 * vert.v.y + _30;
		target.v.y = _01 * vert.v.x + _11 * vert.v.y + _31;
		target.uv.x = vert.uv.x;
		target.uv.y = vert.uv.y;
		target.color = (color * vert.color * opacity).uint;

		if (flipped_y)
			target.v.y = m_canvas->get_viewport_height() - target.v.y;
	}

	offset = m_current_index * 3;
	auto* index_data = &(*m_index_data)[0];
	auto* idxs = indices.data();

	for (int i = 0; i<isz; i++)
	{
		index_data[offset++] = baseIndex + idxs[i];
	}

	m_current_vertex += (int)vsz;
	m_current_index += (int)(isz / 3.0f);

	return true;
}

bool RenderLayer::validate(TexturePtr texture, ShaderPtr shader)
{
	if (m_texture != texture)
		return false;

	if (m_shader != shader)
		return false;

	if (m_scissor != m_canvas->scissor_test())
		return false;

	if (m_scissor)
	{
		if (m_canvas->get_scissor_x() != m_scissor_x)
			return false;

		if (m_canvas->get_scissor_y() != m_scissor_y)
			return false;

		if (m_canvas->get_scissor_width() != m_scissor_width)
			return false;

		if (m_canvas->get_scissor_height() != m_scissor_height)
			return false;
	}

	return true;
}

bool RenderLayer::texture()
{
	return m_texture.get() != nullptr;
}

bool RenderLayer::scissor_test()
{
	return m_scissor;
}

float RenderLayer::get_scissor_x()
{
	return m_scissor_x;
}

float RenderLayer::get_scissor_y()
{
	return m_scissor_y;
}

float RenderLayer::get_scissor_width()
{
	return m_scissor_width;
}

float RenderLayer::get_scissor_height()
{
	return m_scissor_height;
}

TexturePtr RenderLayer::get_texture()
{
	return m_texture;
}

ShaderPtr RenderLayer::get_shader()
{
	return m_shader;
}

void RenderLayer::reset(TexturePtr texture, ShaderPtr shader)
{
	m_texture = texture;
	m_shader = shader;

	m_current_index = 0;
	m_current_vertex = 0;

	memset(&(*m_vertex_data)[0], 0, m_vertex_data->size() * sizeof(VertexData));
	memset(&(*m_index_data)[0], 0, m_index_data->size() * sizeof(unsigned short));

	m_scissor = m_canvas->scissor_test();
	m_scissor_x = m_canvas->get_scissor_x();
	m_scissor_y = m_canvas->get_scissor_y();
	m_scissor_width = m_canvas->get_scissor_width();
	m_scissor_height = m_canvas->get_scissor_height();
}

void RenderLayer::upload(int va, int ca)
{
	if (m_current_vertex == 0 || m_current_index == 0)
		return;

	glActiveTexture(GL_TEXTURE0);
	CHECK_GL_ERROR;
	glBindTexture(GL_TEXTURE_2D, m_texture == nullptr ? 0 : (GLuint)m_texture->get_id());
	CHECK_GL_ERROR;

	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	CHECK_GL_ERROR;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
	CHECK_GL_ERROR;

	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * m_current_vertex, &(*m_vertex_data)[0], GL_STREAM_DRAW);
	CHECK_GL_ERROR;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * m_current_index * 3, &(*m_index_data)[0], GL_STREAM_DRAW);
	CHECK_GL_ERROR;

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
	glVertexAttribPointer(va, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (GLvoid*)OFFSETOF(VertexData, v));
	CHECK_GL_ERROR;
	glVertexAttribPointer(ca, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexData), (GLvoid*)OFFSETOF(VertexData, color));
	CHECK_GL_ERROR;
#undef OFFSETOF

	if (m_scissor)
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor((GLint)m_scissor_x, (GLint)m_scissor_y, (GLsizei)m_scissor_width, (GLsizei)m_scissor_height);
	}
	else
	{
		glDisable(GL_SCISSOR_TEST);
	}

	glDrawElements(GL_TRIANGLES, m_current_index * 3, GL_UNSIGNED_SHORT, 0);
	CHECK_GL_ERROR;

	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERROR;
}