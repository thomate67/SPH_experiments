#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include <vector>

class RenderCone
{
public:
	RenderCone();
	RenderCone(glm::vec3 basepoint, float baseradius, float apexradius, float height, int resolution);
	virtual ~RenderCone();

	void createBuffers();
	void render();
	void render(int n);

	const glm::vec3& getBasepoint() const;
	const glm::vec3& getApexpoint() const;
	const glm::vec3& get_u() const;
	const glm::vec3& get_v() const;
	const glm::vec3& get_w() const;
	float getBaseradius() const;
	float getApexradius() const;
	float getSlope() const;

	std::vector<glm::vec4> getVertices();
	std::vector<glm::vec3> getNormals();
	std::vector<glm::vec2>& getUVs();
	std::vector<unsigned int>& getIndex();
	GLuint getVAO();
	int getNumIndices() const;

protected:
	GLuint m_vao;
	GLuint m_vertexbuffer;
	GLuint m_normalbuffer;
	GLuint m_uvbuffer;
	GLuint m_indexlist;

	int m_points;
	int m_indices;

	std::vector<glm::vec4> m_vertices;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec2> m_uvs;
	std::vector<unsigned int> m_index;

private:
	void create();
	float m_baseradius;
	float m_apexradius;
	float m_height;
	float m_slope;
	glm::vec3 m_v;
	glm::vec3 m_u;
	glm::vec3 m_w;
	glm::vec3 m_basepoint;
	glm::vec3 m_apexpoint;
	int m_resolution;
};
