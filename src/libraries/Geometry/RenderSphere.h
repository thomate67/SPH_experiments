#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>

class RenderSphere
{
public:
	RenderSphere();
	RenderSphere(glm::vec4 center, float radius, int resolution);
	virtual ~RenderSphere();

	void createBuffers();
	void render();
	void render(int n);
	
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
	void create(glm::vec4 center, float radius, int resolution);
	glm::vec4 m_Center;
	float m_radius;
	int m_resolution;
};