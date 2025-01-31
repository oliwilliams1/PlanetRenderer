#include "assetManager.h"
#include <filesystem>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "console.h"

namespace AssetManager {
	static System* s_Instance = nullptr;

	System::System() {
		// Set input box to empty instead of null
		strncpy(this->addObjectNameBuffer, "", sizeof(this->addObjectNameBuffer) - 1);
		this->addObjectNameBuffer[sizeof(this->addObjectNameBuffer) - 1] = '\0';

		this->addObjectWindow = false;
		s_Instance->LoadObjects();
	}

	System::~System() {
		m_Objects.clear();
	}

	void System::Init() {
		s_Instance = new System();
	}

	void System::Shutdown() {
		delete s_Instance;
		s_Instance = nullptr;
	}

	System& System::GetInstance() {
		return *s_Instance;
	}

	void System::DisplayGui() {
		ImGui::Text("Asset Manager");
		ImGui::SameLine();

		if (ImGui::Button("Add Object")) addObjectWindow = true;

		// List all loaded object names
		for (const auto& [key, object] : m_Objects) {
			ImGui::Selectable(key.c_str());
		}

		if (addObjectWindow) DisplayAddObjectWindow();
	}

	void System::DisplayAddObjectWindow() {
		ImGui::Begin("Add Object", &addObjectWindow);
		ImGui::SetWindowSize(ImVec2(350, 450));

		ImGui::Text("format = name.filetype");
		ImGui::InputText("File name", addObjectNameBuffer, sizeof(addObjectNameBuffer));
		
		if (ImGui::Button("Load object")) {
			bool success = LoadModelFromFile(addObjectNameBuffer);

			if (success) {
				addObjectWindow = false;
				Sable::Console::Log("Loaded object: %s", addObjectNameBuffer);

				SaveObjects();
				// Clear input box to empty instead of null
				strncpy(this->addObjectNameBuffer, "", sizeof(this->addObjectNameBuffer) - 1);
				this->addObjectNameBuffer[sizeof(this->addObjectNameBuffer) - 1] = '\0';
			}
			else {
				Sable::Console::Log("Failed to load object: %s", addObjectNameBuffer);
			}
		};
		ImGui::End();
	}

	bool System::GetObject(const std::string& objName, ObjectData& object) {
		auto it = m_Objects.find(objName);
		if (it != m_Objects.end()) {
			object = it->second;
			return true;
		}

		// If not found, attempt to load it from file
		std::vector<std::string> extentions = { ".obj", ".fbx", ".blend", ".glTF" };
		for (const auto& ext : extentions) {
			std::string fullFileName = "resources/" + objName + ext;
			if (std::filesystem::exists(fullFileName)) {
				Sable::Console::Log("Loading object: %s", fullFileName.c_str());
				bool success = LoadModelFromFile(objName + ext);

				if (success) {
					Sable::Console::Log("Loaded object: %s", fullFileName.c_str());
					object = m_Objects[objName];
					SaveObjects();
					return true;
				}
				else {
					Sable::Console::Log("Failed to load object: %s", fullFileName.c_str());
					return false;
				}
			}
		}

		Sable::Console::Log("Failed to find object: %s", objName.c_str());

		return false;
	}

	std::vector<std::string> System::GetObjectNames() {
		std::vector<std::string> objectNames;
		for (const auto& [key, object] : m_Objects) {
			objectNames.push_back(key);
		}
		return objectNames;
	}

	bool System::LoadModelFromFile(const std::string& i_filePath) {
		std::string filePath = "resources/" + i_filePath;

		// Check if obj file (correct format for assimp becaus of treeit)
		if (filePath.find(".obj") != std::string::npos) {
			ModifyBrokenOBJFile(filePath);
		}

		std::ifstream file(filePath);

		if (!file.good()) {
			Sable::Console::Log("Failed to load object: %s", filePath.c_str());
			return false;
		}

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filePath, aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

		// Check if the import was successful
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			Sable::Console::Log("Failed to load object: %s", filePath.c_str());
			return false;
		}

		std::string objName = i_filePath.substr(0, i_filePath.find_last_of('.'));
		
		// Process all meshes
		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[i];

			ObjectData objectData;

			// Process vertices, normals, and UVs
			for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
				aiVector3D vertex = mesh->mVertices[j];
				objectData.vertices.emplace_back(vertex.x, vertex.y, vertex.z);

				aiVector3D normal = mesh->mNormals[j];
				objectData.normals.emplace_back(normal.x, normal.y, normal.z);

				// Process UVs if available
				if (mesh->mTextureCoords[0]) {
					aiVector3D uv = mesh->mTextureCoords[0][j];
					objectData.uvs.emplace_back(uv.x, uv.y);
				}
				else {
					objectData.uvs.emplace_back(0.0f, 0.0f);
				}
			}

			objectData.indices.reserve(mesh->mNumFaces * 3);

			// Process indices
			for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
				aiFace face = mesh->mFaces[j];

				// Reverse winding order
				for (unsigned int k = face.mNumIndices; k > 0; --k) {
					objectData.indices.emplace_back(face.mIndices[k - 1]);
				}
			}

			// Retrieve material index for the current mesh
			if (mesh->mMaterialIndex >= 0) {
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

				// Check for a texture path
				aiString texturePath;
				if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
					objectData.texturePath = texturePath.C_Str();
				}
			}

			std::string saveName;
			if (i == 0) {
				saveName = objName;
			}
			else {
				saveName = objName + "_" + std::to_string(i);
			}

			objectData.objName = saveName;

			// Add the populated ObjectData to the objects vector
			m_Objects[saveName] = objectData;
		}
		return true;
	}

	void System::ModifyBrokenOBJFile(const std::string& filePath) {
		std::string inputData;
		std::string outputData;

		if (!ReadFile(filePath.c_str(), inputData)) {
			Sable::Console::Log("Failed to read file to fix OBJ issue: %s", filePath.c_str());
			return;
		}

		std::istringstream stream(inputData);
		std::string line;

		while (std::getline(stream, line)) {
			if (line.substr(0, 2) == "v ") {
				std::istringstream vertexStream(line);
				std::string vertexPrefix;
				double x, y, z;

				vertexStream >> vertexPrefix >> x >> y >> z;
				outputData += "v " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + "\n";
			}
			else {
				outputData += line + "\n";
			}
		}

		if (!WriteFile(filePath.c_str(), outputData)) {
			Sable::Console::Log("Failed to write file to fix OBJ issue: %s", filePath.c_str());
		}
	}

	void System::LoadObjects() {
		std::ifstream file("resources/assets.assets", std::ios::binary);
		if (!file.is_open()) {
			Sable::Console::Log("Failed to open file for reading: resources/assets.assets");
			return;
		}

		size_t objectCount = 0;
		file.read(reinterpret_cast<char*>(&objectCount), sizeof(objectCount));

		for (size_t i = 0; i < objectCount; ++i) {
			ObjectData object;

			// Read the length of the name and the name itself
			size_t nameLength = 0;
			file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
			std::string objName(nameLength, '\0');
			file.read(&objName[0], nameLength);
			object.objName = objName;

			// Read texture path
			size_t texturePathLength = 0;
			file.read(reinterpret_cast<char*>(&texturePathLength), sizeof(texturePathLength));
			std::string texturePath(texturePathLength, '\0');
			file.read(&texturePath[0], texturePathLength);
			object.texturePath = texturePath;

			// Read vertices
			size_t vertexCount = 0;
			file.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
			object.vertices.resize(vertexCount);
			file.read(reinterpret_cast<char*>(object.vertices.data()), vertexCount * sizeof(glm::vec3));

			// Read normals
			size_t normalCount = 0;
			file.read(reinterpret_cast<char*>(&normalCount), sizeof(normalCount));
			object.normals.resize(normalCount);
			file.read(reinterpret_cast<char*>(object.normals.data()), normalCount * sizeof(glm::vec3));

			// Read UVs
			size_t uvCount = 0;
			file.read(reinterpret_cast<char*>(&uvCount), sizeof(uvCount));
			object.uvs.resize(uvCount);
			file.read(reinterpret_cast<char*>(object.uvs.data()), uvCount * sizeof(glm::vec2));

			// Read indices
			size_t indexCount = 0;
			file.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
			object.indices.resize(indexCount);
			file.read(reinterpret_cast<char*>(object.indices.data()), indexCount * sizeof(unsigned int));

			// Insert into the map
			m_Objects[objName] = object;
		}

		file.close();
	}
	
	void System::SaveObjects() {
		std::ofstream file("resources/assets.assets", std::ios::binary);
		if (!file.is_open()) {
			Sable::Console::Log("Failed to open file for writing: resources/assets.assets");
			return;
		}

		// Write the number of objects
		size_t objectCount = m_Objects.size();
		file.write(reinterpret_cast<const char*>(&objectCount), sizeof(objectCount));

		for (const auto& [key, object] : m_Objects) {
			// Write the length of the name and the name itself
			size_t nameLength = key.size();
			file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
			file.write(key.c_str(), nameLength);

			// Write object data
			size_t texturePathLength = object.texturePath.size();
			file.write(reinterpret_cast<const char*>(&texturePathLength), sizeof(texturePathLength));
			file.write(object.texturePath.c_str(), texturePathLength);

			// Write vertices
			size_t vertexCount = object.vertices.size();
			file.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));
			file.write(reinterpret_cast<const char*>(object.vertices.data()), vertexCount * sizeof(glm::vec3));

			// Write normals
			size_t normalCount = object.normals.size();
			file.write(reinterpret_cast<const char*>(&normalCount), sizeof(normalCount));
			file.write(reinterpret_cast<const char*>(object.normals.data()), normalCount * sizeof(glm::vec3));

			// Write UVs
			size_t uvCount = object.uvs.size();
			file.write(reinterpret_cast<const char*>(&uvCount), sizeof(uvCount));
			file.write(reinterpret_cast<const char*>(object.uvs.data()), uvCount * sizeof(glm::vec2));

			// Write indices
			size_t indexCount = object.indices.size();
			file.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));
			file.write(reinterpret_cast<const char*>(object.indices.data()), indexCount * sizeof(unsigned int));
		}
	}
}