#include "planet.h"

PlanetShader::PlanetShader(const char* vsSource, const char* fsSource, const char* shaderName)
    : Shader(vsSource, fsSource, shaderName) {
    std::string tesc, tese;
    if (!ReadFile("shaders/planet.tesc", tesc)) exit(1);
    AddShader(shaderProgram, tesc.c_str(), GL_TESS_CONTROL_SHADER);

    if (!ReadFile("shaders/planet.tese", tese)) exit(1);
    AddShader(shaderProgram, tese.c_str(), GL_TESS_EVALUATION_SHADER);
    
    GLint success = 0;
    GLchar errorLog[1024] = { 0 };

    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(shaderProgram, sizeof(errorLog), nullptr, errorLog);
        std::cout << "Error linking shader program: " << errorLog << std::endl;
        exit(1);
    }

    glUseProgram(shaderProgram);

    SetupUniforms();
}

void PlanetShader::SetupUniforms() {
    planetColourLocation = glGetUniformLocation(shaderProgram, "planetColour");
    if (planetColourLocation == -1) {
        std::cerr << "Warning: planetColour uniform not found!" << std::endl;
    }

    glUniform3f(planetColourLocation, 0.5f, 0.25f, 0.0f);
}

Planet::Planet(PlanetShader* shader) : Object(shader) {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    GeneratePlanet(vertices, indices);

    for (int i = 0; i < 2; i++) {
        SubdividePlanet(vertices, indices);
    }

    SetData(vertices, std::vector<glm::vec3>(0), indices);

    planetScaleLocation = glGetUniformLocation(shader->shaderProgram, "planetScale");
    if (planetScaleLocation == -1) {
        std::cerr << "Warning: planetScale uniform not found!" << std::endl;
    }

    glUniform1f(planetScaleLocation, planetScale);

    GenerateTexture();

	planetTextureLocation = glGetUniformLocation(shader->shaderProgram, "planetTexture");
	if (planetTextureLocation == -1) {
		std::cerr << "Warning: planetTexture uniform not found!" << std::endl;
	}

	glUniform1i(planetTextureLocation, 0);
}

void Planet::Draw()
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glDrawElements(GL_PATCHES, indicesCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Planet::GeneratePlanet(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices) {
    vertices = {
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f,  1.0f, -1.0f),
        glm::vec3(-1.0f,  1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f,  1.0f),
        glm::vec3(1.0f, -1.0f,  1.0f),
        glm::vec3(1.0f,  1.0f,  1.0f),
        glm::vec3(-1.0f,  1.0f,  1.0)
    };

    indices = {
        0, 3, 2, 1,
        4, 5, 6, 7,
        0, 4, 7, 3,
        1, 2, 6, 5,
        3, 7, 6, 2,
        4, 0, 1, 5 
    };
}

// Thank you chatgpt
void Planet::SubdividePlanet(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices) {
    std::vector<glm::vec3> newVertices = vertices;
    std::vector<unsigned int> newIndices;
    std::vector<unsigned int> tempIndices;

    for (size_t j = 0; j < indices.size(); j += 4) {
        unsigned int a = indices[j];
        unsigned int b = indices[j + 1];
        unsigned int c = indices[j + 2];
        unsigned int d = indices[j + 3];

        glm::vec3 ab = 0.5f * (newVertices[a] + newVertices[b]);
        glm::vec3 bc = 0.5f * (newVertices[b] + newVertices[c]);
        glm::vec3 cd = 0.5f * (newVertices[c] + newVertices[d]);
        glm::vec3 da = 0.5f * (newVertices[d] + newVertices[a]);
        glm::vec3 m = 0.25f * (newVertices[a] + newVertices[b] + newVertices[c] + newVertices[d]);

        unsigned int ab_index = newVertices.size();
        unsigned int bc_index = ab_index + 1;
        unsigned int cd_index = bc_index + 1;
        unsigned int da_index = cd_index + 1;
        unsigned int m_index = da_index + 1;

        newVertices.push_back(ab);
        newVertices.push_back(bc);
        newVertices.push_back(cd);
        newVertices.push_back(da);
        newVertices.push_back(m);

        tempIndices.insert(tempIndices.end(), { a, ab_index, m_index, da_index });
        tempIndices.insert(tempIndices.end(), { ab_index, b, bc_index, m_index });
        tempIndices.insert(tempIndices.end(), { m_index, bc_index, c, cd_index });
        tempIndices.insert(tempIndices.end(), { da_index, m_index, cd_index, d });
    }

    newIndices = std::move(tempIndices);

    vertices = std::move(newVertices);
    indices = std::move(newIndices);
}

void Planet::GenerateTexture() {
    int width = 2048;
    int height = 1024;

    const siv::PerlinNoise::seed_type seed = 1;
    const siv::PerlinNoise perlin(seed);

    uint8_t* data = new uint8_t[width * height * 3];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float lon = (x / (float)width) * 360 - 180;
            float lat = (y / (float)height) * 180 - 90;

            float theta = glm::radians(lon);
            float phi = glm::radians(lat);

            glm::vec3 dir = glm::vec3(glm::cos(phi) * glm::cos(theta),
                glm::sinh(phi),
                glm::cos(phi) * glm::sin(theta));

			dir = glm::normalize(dir);

            float contrib = perlin.octave3D_01(dir.x, dir.y, dir.z, 8, 0.5f);

            data[(y * width + x) * 3 + 0] = contrib * 255;
            data[(y * width + x) * 3 + 1] = contrib * 255;
            data[(y * width + x) * 3 + 2] = contrib * 255;
        }
    }

    // Upload texture
    glGenTextures(1, &planetTextureID);
    glBindTexture(GL_TEXTURE_2D, planetTextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Planet::ObjectDebugImGUI() {
    ImGui::Begin("Object data");
    if (ImGui::SliderFloat3("Position", &position.x, -10.0f, 10.0f)) UpdateModelMatrix();
    if (ImGui::SliderFloat3("Rotation", &rotation.x, -180.0f, 180.0f)) UpdateModelMatrix();
    if (ImGui::SliderFloat("Scale", &planetScale, 1.0f, 1000.0f)) glUniform1f(planetScaleLocation, planetScale);
    ImGui::End();

    ImGui::Begin("Planet texture viewer");
	ImGui::Image((void*)(intptr_t)planetTextureID, ImVec2(256, 128));
	ImGui::End();
}