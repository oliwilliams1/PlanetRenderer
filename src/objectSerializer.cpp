#include "objectSerializer.h"

void SerializeObjects(const std::vector<ObjectData>& objects, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    size_t objectCount = objects.size();
    file.write(reinterpret_cast<const char*>(&objectCount), sizeof(objectCount));

    for (const auto& object : objects) {
        size_t texturePathLength = object.texturePath.size();
        file.write(reinterpret_cast<const char*>(&texturePathLength), sizeof(texturePathLength));
        file.write(object.texturePath.c_str(), texturePathLength);

        size_t vertexCount = object.vertices.size();
        file.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));
        file.write(reinterpret_cast<const char*>(object.vertices.data()), vertexCount * sizeof(glm::vec3));

        size_t normalCount = object.normals.size();
        file.write(reinterpret_cast<const char*>(&normalCount), sizeof(normalCount));
        file.write(reinterpret_cast<const char*>(object.normals.data()), normalCount * sizeof(glm::vec3));

        size_t uvCount = object.uvs.size();
        file.write(reinterpret_cast<const char*>(&uvCount), sizeof(uvCount));
        file.write(reinterpret_cast<const char*>(object.uvs.data()), uvCount * sizeof(glm::vec2));

        size_t indexCount = object.indices.size();
        file.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));
        file.write(reinterpret_cast<const char*>(object.indices.data()), indexCount * sizeof(unsigned int));
    }

    file.close();
}

std::vector<ObjectData> DeserializeObjects(const std::string& filename) {
    std::vector<ObjectData> objects;
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for reading: " << filename << std::endl;
        return objects;
    }

    size_t objectCount = 0;
    file.read(reinterpret_cast<char*>(&objectCount), sizeof(objectCount));
    objects.resize(objectCount);

    for (auto& object : objects) {
        size_t texturePathLength = 0;
        file.read(reinterpret_cast<char*>(&texturePathLength), sizeof(texturePathLength));
        object.texturePath.resize(texturePathLength);
        file.read(&object.texturePath[0], texturePathLength);

        size_t vertexCount = 0;
        file.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
        object.vertices.resize(vertexCount);
        file.read(reinterpret_cast<char*>(object.vertices.data()), vertexCount * sizeof(glm::vec3));

        size_t normalCount = 0;
        file.read(reinterpret_cast<char*>(&normalCount), sizeof(normalCount));
        object.normals.resize(normalCount);
        file.read(reinterpret_cast<char*>(object.normals.data()), normalCount * sizeof(glm::vec3));

        size_t uvCount = 0;
        file.read(reinterpret_cast<char*>(&uvCount), sizeof(uvCount));
        object.uvs.resize(uvCount);
        file.read(reinterpret_cast<char*>(object.uvs.data()), uvCount * sizeof(glm::vec2));

        size_t indexCount = 0;
        file.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
        object.indices.resize(indexCount);
        file.read(reinterpret_cast<char*>(object.indices.data()), indexCount * sizeof(unsigned int));
    }

    file.close();
    return objects;
}