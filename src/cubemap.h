#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <algorithm>

class Cubemap {
public:
	Cubemap(GLuint texture, int resolution);
	~Cubemap();

	glm::vec4 Sample(const glm::vec3& dir);

private:
	void FetchCubemapData();

	GLuint texture;
	int resolution;
	unsigned char* posx;
	unsigned char* negx;
	unsigned char* posy;
	unsigned char* negy;
	unsigned char* posz;
	unsigned char* negz;
};