#include "RenderCone.h"

RenderCone::RenderCone() {
	m_baseradius = 1.0f;
	m_apexradius = 1.0f;
	m_basepoint = glm::vec3(0);
	m_apexpoint = glm::vec3(0.f, 1.f, 0.f);
	m_resolution = 20;
	create();
}

RenderCone::RenderCone(glm::vec3 basepoint, float baseradius, float apexradius, float height, int resolution)
{
	m_baseradius = baseradius;
	m_apexradius = apexradius;
	m_height = height;
	m_basepoint = basepoint;
	m_apexpoint = basepoint + glm::vec3(0.f, height, 0.f);
	m_resolution = resolution;
	create();
}

RenderCone::~RenderCone()
{
	glDeleteBuffers(1, &m_vertexbuffer);
	glDeleteBuffers(1, &m_normalbuffer);
	glDeleteBuffers(1, &m_uvbuffer);
	glDeleteBuffers(1, &m_indexlist);

	glDeleteVertexArrays(1, &m_vao);
}

void RenderCone::createBuffers()
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

void RenderCone::render()
{

	glBindVertexArray(m_vao);
	glPointSize(5.0f);
	glDrawElements(GL_TRIANGLES, m_indices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void RenderCone::render(int n)
{
	int last_vao = 0;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vao);
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_indices, GL_UNSIGNED_INT, 0, n);
	glBindVertexArray(last_vao);
}

const glm::vec3& RenderCone::getBasepoint() const 
{ 
	return m_basepoint; 
}

const glm::vec3& RenderCone::getApexpoint() const 
{ 
	return m_apexpoint; 
}

const glm::vec3& RenderCone::get_u() const 
{ 
	return m_u; 
}

const glm::vec3& RenderCone::get_v() const 
{ 
	return m_v; 
}

const glm::vec3& RenderCone::get_w() const 
{ 
	return m_w; 
}

float RenderCone::getBaseradius() const 
{ 
	return m_baseradius; 
}

float RenderCone::getApexradius() const 
{ 
	return m_apexradius; 
}

float RenderCone::getSlope() const 
{ 
	return m_slope; 
}

std::vector<glm::vec4> RenderCone::getVertices()
{
	return m_vertices;
}

std::vector<glm::vec3> RenderCone::getNormals()
{
	return m_normals;
}

std::vector<glm::vec2>& RenderCone::getUVs()
{
	return m_uvs;
}

std::vector<unsigned int>& RenderCone::getIndex()
{
	return m_index;
}

GLuint RenderCone::getVAO()
{
	return m_vao;
}

int RenderCone::getNumIndices() const
{
	return m_indices;
}

void RenderCone::create()
{
	// iniatialize the variable we are going to use
	float u, v;
	float radius, phi;
	glm::vec3 q;
	int offset = 0, i, j;

	m_v = m_apexpoint - m_basepoint;
	m_height = glm::length(m_v);
	m_v = glm::normalize(m_v);

	/* find two axes which are at right angles to cone_v */
	glm::vec3 tmp(0.f, 1.f, 0.f);
	if (1.f - fabs(glm::dot(tmp, m_v)) < 1e-4f)
		tmp = glm::vec3(0.f, 0.f, 1.f);

	m_u = glm::normalize(glm::cross(m_v, tmp));
	m_w = glm::normalize(glm::cross(m_u, m_v));

	m_slope = (m_baseradius - m_apexradius) / m_height;

	// Envelope
	for (j = 0; j <= m_resolution; j++)     // radius
		for (i = 0; i <= m_resolution; i++) // phi
		{
			u = i / (float)m_resolution;
			phi = 2 * glm::pi<float>() * u;
			v = j / (float)m_resolution;
			v = m_height * v;

			radius = m_baseradius - m_slope * v;
			q = m_basepoint + radius * sinf(phi) * m_u + v * m_v +
				radius * cosf(phi) * m_w;

			float t = glm::dot(q, m_v) - glm::dot(m_basepoint, m_v);
			glm::vec3 q_1 = q - t * m_v;
			glm::vec3 n = glm::normalize(q_1 - m_basepoint);
			n = glm::normalize(n + m_slope * m_v);

			m_vertices.push_back(glm::vec4(q, 1.0f));
			m_normals.push_back(n);
			m_uvs.push_back(glm::vec2(u, v / m_height));
		}

	m_points = m_vertices.size();

	// create index list
	for (j = 0; j < m_resolution; j++) {
		for (i = 0; i < m_resolution; i++) {
			// 1. Triangle
			m_index.push_back(offset + i + m_resolution + 1);
			m_index.push_back(offset + i);
			m_index.push_back(offset + i + 1);

			// 2. Triangle
			m_index.push_back(offset + i + 1);
			m_index.push_back(offset + i + m_resolution + 1 + 1);
			m_index.push_back(offset + i + m_resolution + 1);
		}
		offset += m_resolution + 1;
	}
	m_indices = m_index.size();

	createBuffers();
}
