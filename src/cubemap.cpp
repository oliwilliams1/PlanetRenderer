#include "cubemap.h"

Cubemap::Cubemap(GLuint texture, int resolution) {
	this->texture = texture;
	this->resolution = resolution;

    FetchCubemapData();
}

void Cubemap::FetchCubemapData() {
	int resolutionPerFace = resolution * resolution * 4;

	posx = new unsigned char[resolutionPerFace];
	negx = new unsigned char[resolutionPerFace];
	posy = new unsigned char[resolutionPerFace];
	negy = new unsigned char[resolutionPerFace];
	posz = new unsigned char[resolutionPerFace];
	negz = new unsigned char[resolutionPerFace];

	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, GL_UNSIGNED_BYTE, posx);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, GL_UNSIGNED_BYTE, negx);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, posy);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, negy);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, GL_UNSIGNED_BYTE, posz);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, GL_UNSIGNED_BYTE, negz);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

glm::vec4 Cubemap::Sample(const glm::vec3& direction) {
    glm::vec3 dir = glm::normalize(direction);

    unsigned char* selectedFace = nullptr;
    float u, v;

    if (std::abs(dir.x) >= std::abs(dir.y) && std::abs(dir.x) >= std::abs(dir.z)) {
        // Positive X face
        if (dir.x > 0) {
            selectedFace = posx;
            u = dir.z / -dir.x;
            v = dir.y / -dir.x;
        }
        else {
            selectedFace = negx;
            u = dir.z / -dir.x;
            v = dir.y / dir.x;
        }
    }
    else if (std::abs(dir.y) >= std::abs(dir.x) && std::abs(dir.y) >= std::abs(dir.z)) {
        // Positive Y face
        if (dir.y > 0) {
            selectedFace = posy;
            u = dir.x / dir.y;
            v = dir.z / dir.y;
        }
        else {
            selectedFace = negy;
            u = dir.x / -dir.y;
            v = dir.z / dir.y;
        }
    }
    else {
        // Positive Z face
        if (dir.z > 0) {
            selectedFace = posz;
            u = dir.x / dir.z;
            v = dir.y / -dir.z;
        }
        else {
            selectedFace = negz;
            u = dir.x / dir.z;
            v = dir.y / dir.z;
        }
    }

    u = 0.5f * (u + 1.0f) * (resolution - 1); // [-1, 1] to [0, resolution]
    v = 0.5f * (v + 1.0f) * (resolution - 1); // [-1, 1] to [0, resolution]

    int x = std::clamp(static_cast<int>(u), 0, resolution - 1);
    int y = std::clamp(static_cast<int>(v), 0, resolution - 1);

    unsigned char* texel = selectedFace + (y * resolution + x) * 4;

    glm::vec4 colour = glm::vec4(texel[0], texel[1], texel[2], texel[3]);
    colour /= 255.0f;

    return colour;
}

Cubemap::~Cubemap() {
	delete[] posx;
	delete[] negx;
	delete[] posy;
	delete[] negy;
	delete[] posz;
	delete[] negz;
}