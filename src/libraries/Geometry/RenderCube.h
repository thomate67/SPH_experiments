#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include <vector>

class RenderCube
{
public:
	RenderCube();
	RenderCube(float size);
	RenderCube(float width, float height, float depth);
	virtual ~RenderCube();

	void createBuffers();
	void render();
	void render(int n);
	void renderShell();

	void setSize(float width, float height, float depth);

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
	void create(float width, float height, float depth);
	float m_size;
	float m_width;
	float m_height;
	float m_depth;
};