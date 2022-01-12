#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>

class RenderPlane
{
public:
	RenderPlane();
	RenderPlane(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d);
	RenderPlane(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec2 tca, glm::vec2 tcb, glm::vec2 tcc, glm::vec2 tcd);
	virtual ~RenderPlane();

	void createBuffers();
	void render();
	void render(int n);

	void set_Points(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d);
	void set_Tcoords(glm::vec2 tca, glm::vec2 tcb, glm::vec2 tcc, glm::vec2 tcd);

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
	void create(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec2 tca, glm::vec2 tcb, glm::vec2 tcc, glm::vec2 tcd);
};