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

void Planet::ObjectDebugImGUI() {
    ImGui::Begin("Object data");
    if (ImGui::SliderFloat3("Position", &position.x, -10.0f, 10.0f)) UpdateModelMatrix();
    if (ImGui::SliderFloat3("Rotation", &rotation.x, -180.0f, 180.0f)) UpdateModelMatrix();
    if (ImGui::SliderFloat("Scale", &planetScale, 1.0f, 100.0f)) glUniform1f(planetScaleLocation, planetScale);
    ImGui::End();
}