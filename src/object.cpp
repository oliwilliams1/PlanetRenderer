#include "object.h"
#include "assetManager.h"
#include "utils.h"

Object::Object(Shader* shader) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);
    glGenBuffers(1, &NBO);
    this->shader   = shader;
    this->position = glm::vec3(0.0f, 0.0f, 0.0f);
	this->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	this->scale    = glm::vec3(1.0f, 1.0f, 1.0f);

    GetModelMatrixLocation();
    UpdateModelMatrix();
}

void Object::SetMesh(const std::string& objName, bool foliage) {
    glBindVertexArray(VAO);

	ObjectData objData;

    AssetManager::System::GetInstance().GetObject(objName, objData);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, objData.vertices.size() * sizeof(glm::vec3), objData.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    if (objData.normals.size() > 0) {
        // Normal buffer
        glBindBuffer(GL_ARRAY_BUFFER, NBO);
        glBufferData(GL_ARRAY_BUFFER, objData.normals.size() * sizeof(glm::vec3), objData.normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
    }

    if (objData.uvs.size() > 0) {
        glGenBuffers(1, &UVBO);
        glBindBuffer(GL_ARRAY_BUFFER, UVBO);
        glBufferData(GL_ARRAY_BUFFER, objData.uvs.size() * sizeof(glm::vec2), objData.uvs.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);
    }

    if (objData.indices.size() > 0) {
        // Element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, objData.indices.size() * sizeof(unsigned int), objData.indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        indicesCount = objData.indices.size();
    }

    if (!objData.texturePath.empty()) {
        albedoLocation = GetUniformLocation(shader->shaderProgram, "u_Albedo");
        LoadTexture(&albedo, ("resources/" + objData.texturePath).c_str(), foliage);
    }
}

void Object::Draw() {
    UpdateModelMatrix();

    if (albedoLocation != -1) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedo);
        glUniform1i(albedoLocation, 0);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Object::GetModelMatrixLocation() {
    modelMatrixLocation = GetUniformLocation(shader->shaderProgram, "m_Model");
}

void Object::UpdateModelMatrix() const {
    if (modelMatrixLocation == -1) return;

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, position);

    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, scale);

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(model));
}

void Object::DebugDraw() {
    if (ImGui::DragFloat3("Position", &position.x, 1.0f, 0.0f, 0.0f, "%.1f")) UpdateModelMatrix();
    if (ImGui::DragFloat3("Rotation", &rotation.x, 1.0f, -180.0f, 180.0f, "%.1f")) UpdateModelMatrix();
    if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.0f, 10.0f, "%.1f")) UpdateModelMatrix();
}