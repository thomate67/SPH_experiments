#include "RenderPlane.h"

RenderPlane::RenderPlane() 
{
	create(glm::vec3(-1, 0, -1), glm::vec3(-1, 0, 1), glm::vec3(1, 0, 1),
		glm::vec3(1, 0, -1), glm::vec2(0, 1), glm::vec2(0, 0),
		glm::vec2(1, 0), glm::vec2(1, 1));
}

RenderPlane::RenderPlane(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d)
{
	create(a, b, c, d, glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1, 0),
		glm::vec2(1, 1));
}

RenderPlane::RenderPlane(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec2 tca, glm::vec2 tcb, glm::vec2 tcc, glm::vec2 tcd)
{
	create(a, b, c, d, tca, tcb, tcc, tcd);
}

RenderPlane::~RenderPlane()
{
	glDeleteBuffers(1, &m_vertexbuffer);
	glDeleteBuffers(1, &m_normalbuffer);
	glDeleteBuffers(1, &m_uvbuffer);
	glDeleteBuffers(1, &m_indexlist);

	glDeleteVertexArrays(1, &m_vao);
}

void RenderPlane::createBuffers()
{
	m_points = m_vertices.size();
	m_indices = m_index.size();

	glGenBuffers(1, &m_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_points * sizeof(glm::vec4), &m_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_points * sizeof(glm::vec3), &m_normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_points * sizeof(glm::vec2), &m_uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_indexlist);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexlist);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices * sizeof(unsigned int), &m_index[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_normalbuffer);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexlist);

	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void RenderPlane::render()
{
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void RenderPlane::render(int n)
{
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_indices, GL_UNSIGNED_INT, 0, n);
	glBindVertexArray(0);
}

void RenderPlane::set_Points(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d)
{
	m_vertices.clear();
	m_vertices.emplace_back(a, 1.0f);
	m_vertices.emplace_back(b, 1.0f);
	m_vertices.emplace_back(c, 1.0f);
	m_vertices.emplace_back(d, 1.0f);

	glm::vec3 n = glm::normalize(glm::cross(b - a, c - a));

	m_normals.clear();
	m_normals.push_back(n);
	m_normals.push_back(n);
	m_normals.push_back(n);
	m_normals.push_back(n);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_points * sizeof(glm::vec4), &m_vertices[0],
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_points * sizeof(glm::vec3), &m_normals[0],
		GL_STATIC_DRAW);
}
void RenderPlane::set_Tcoords(glm::vec2 tca, glm::vec2 tcb, glm::vec2 tcc, glm::vec2 tcd)
{
	m_uvs.clear();
	m_uvs.push_back(tca);
	m_uvs.push_back(tcb);
	m_uvs.push_back(tcc);
	m_uvs.push_back(tcd);

	glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, m_points * sizeof(glm::vec2), &m_uvs[0],
		GL_STATIC_DRAW);
}

std::vector<glm::vec4> RenderPlane::getVertices()
{
	return m_vertices;
}

std::vector<glm::vec3> RenderPlane::getNormals()
{
	return m_normals;
}

std::vector<glm::vec2>& RenderPlane::getUVs()
{
	return m_uvs;
}

std::vector<unsigned int>& RenderPlane::getIndex()
{
	return m_index;
}

GLuint RenderPlane::getVAO()
{
	return m_vao;
}

int RenderPlane::getNumIndices() const
{
	return m_indices;
}

void RenderPlane::create(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
	glm::vec2 tca, glm::vec2 tcb, glm::vec2 tcc, glm::vec2 tcd) {
	m_vertices.emplace_back(a, 1.0f);
	m_vertices.emplace_back(b, 1.0f);
	m_vertices.emplace_back(c, 1.0f);
	m_vertices.emplace_back(d, 1.0f);

	glm::vec3 n = glm::normalize(glm::cross(b - a, c - a));

	m_normals.push_back(n);
	m_normals.push_back(n);
	m_normals.push_back(n);
	m_normals.push_back(n);

	m_uvs.push_back(tca);
	m_uvs.push_back(tcb);
	m_uvs.push_back(tcc);
	m_uvs.push_back(tcd);

	m_points = 4;

	m_index.push_back(0);
	m_index.push_back(1);
	m_index.push_back(2);

	m_index.push_back(2);
	m_index.push_back(3);
	m_index.push_back(0);

	m_indices = 6;

	createBuffers();
}