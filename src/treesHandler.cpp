#include "treesHandler.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

TreesHandler::TreesHandler(Planet* planet) {
	this->planet = planet;
    this->shader = new Shader("shaders/tree.vert", "shaders/tree.frag", "Trees Shader");

	int numPoints = 5000;

	posx = new unsigned char[512 * 512 * 4];
	negx = new unsigned char[512 * 512 * 4];
	posy = new unsigned char[512 * 512 * 4];
	negy = new unsigned char[512 * 512 * 4];
	posz = new unsigned char[512 * 512 * 4];
	negz = new unsigned char[512 * 512 * 4];

	glBindTexture(GL_TEXTURE_CUBE_MAP, planet->noiseCubemapTexture);

	glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, GL_UNSIGNED_BYTE, posx);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, GL_UNSIGNED_BYTE, negx);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, posy);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, negy);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, GL_UNSIGNED_BYTE, posz);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, GL_UNSIGNED_BYTE, negz);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	FibonacciSphere(numPoints);
    SetupBuffers();

    delete[] posx;
	delete[] negx;
	delete[] posy;
	delete[] negy;
	delete[] posz;
	delete[] negz;
}

void TreesHandler::SetupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void TreesHandler::Draw() {
    shader->use();
    glBindVertexArray(VAO);
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, vertices.size());
    glBindVertexArray(0);
}

unsigned char* TreesHandler::SampleCubemap(const glm::vec3& direction) {
    glm::vec3 dir = glm::normalize(direction);

    unsigned char* selectedFace = nullptr;
    float u, v;

    if (std::abs(dir.x) >= std::abs(dir.y) && std::abs(dir.x) >= std::abs(dir.z)) {
        // Positive X face
        if (dir.x > 0) {
            selectedFace = posx;
            u = -dir.z / dir.x;
            v = -dir.y / dir.x;
        }
        else {
            selectedFace = negx;
            u = dir.z / -dir.x;
            v = -dir.y / -dir.x;
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
            v = dir.z / -dir.y;
        }
    }
    else {
        // Positive Z face
        if (dir.z > 0) {
            selectedFace = posz;
            u = dir.x / dir.z;
            v = -dir.y / dir.z;
        }
        else {
            selectedFace = negz;
            u = dir.x / -dir.z;
            v = -dir.y / -dir.z;
        }
    }

    u = 0.5f * (u + 1.0f) * (512 - 1); // [-1, 1] to [0, 511]
    v = 0.5f * (v + 1.0f) * (512 - 1); // [-1, 1] to [0, 511]

    int x = std::clamp(static_cast<int>(u), 0, 511);
    int y = std::clamp(static_cast<int>(v), 0, 511);

    unsigned char* texel = selectedFace + (y * 512 + x) * 4;

    unsigned char* color = new unsigned char[4];
    color[0] = texel[0];
    color[1] = texel[1];
    color[2] = texel[2];
    color[3] = 255;

    return color; // Must be deleted
}

void TreesHandler::FibonacciSphere(int numPoints) {
	float goldenAngle = 3.14159265358 * (3.0 - sqrt(5.0));

	vertices.clear();

	for (int i = 0; i < numPoints; i++) {
		float theta = goldenAngle * i;
		float phi = acos(1.0 - 2.0 * ((float)i + 0.5) / numPoints);

		glm::vec3 dir(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));

        unsigned char* color = SampleCubemap(dir);

        if (color[1] > 128) {
			vertices.emplace_back(glm::normalize(dir) * 900.0f);
        }

        delete[] color;
	}

    std::cout << vertices.size() << std::endl;
}