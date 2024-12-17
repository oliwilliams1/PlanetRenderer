#include "treesHandler.h"

TreesHandler::TreesHandler(Planet* planet) {
	this->planet = planet;
	cubemapResolution = planet->cubemapResolution;

	RetrieveCubemapData();

    glm::vec4 sample = SampleCubemap(glm::vec3(0.0f, 1.0f, 0.0f));
    std::cout << sample.x << " " << sample.y << " " << sample.z << " " << sample.w << std::endl;
}

void TreesHandler::RetrieveCubemapData() {
	cubemapData = std::vector<float>(cubemapResolution * cubemapResolution * 6 * 4);

	glBindTexture(GL_TEXTURE_CUBE_MAP, planet->noiseCubemapTexture);

	for (int i = 0; i < 6; i++) {
		glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, GL_FLOAT, &cubemapData[static_cast<std::vector<float, std::allocator<float>>::size_type>((GLuint)i) * cubemapResolution * cubemapResolution * 4]);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

glm::vec4 TreesHandler::SampleCubemap(const glm::vec3& direction) {
    glm::vec3 dir = glm::normalize(direction);

    int face;
    float s, t;

    if (abs(dir.x) >= abs(dir.y) && abs(dir.x) >= abs(dir.z)) {
        face = (dir.x > 0) ? 0 : 1;
        s = -dir.z;
        t = dir.y;
    }
    else if (abs(dir.y) >= abs(dir.x) && abs(dir.y) >= abs(dir.z)) {
        face = (dir.y > 0) ? 2 : 3;
        s = dir.x;
        t = -dir.z;
    }
    else {
        face = (dir.z > 0) ? 4 : 5;
        s = dir.x;
        t = dir.y;
    }

    s = (s + 1.0f) * 0.5f * (cubemapResolution - 1);
    t = (t + 1.0f) * 0.5f * (cubemapResolution - 1);

    int pixelIndex = (face * cubemapResolution * cubemapResolution + int(t) * cubemapResolution + int(s)) * 4;

    return glm::vec4(
        cubemapData[pixelIndex],
        cubemapData[pixelIndex + 1],
        cubemapData[pixelIndex + 2],
        cubemapData[pixelIndex + 3]
    );
}