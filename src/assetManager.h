#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "utils.h"

struct ObjectData {
	std::string objName;
	std::string texturePath;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;
};

// Using chernos' alternative to a usual singleton
namespace AssetManager {
	class System {
	public:
		System(const System&) = delete;
		System& operator=(const System&) = delete;

		static void Init();
		static void Shutdown();

		static System& GetInstance();

		void DisplayGui();
		bool GetObject(const std::string& objName, ObjectData& object);
		std::vector<std::string> GetObjectNames();

	private:
		System();
		~System();

		char addObjectNameBuffer[256];
		bool addObjectWindow;
		void DisplayAddObjectWindow();

		void LoadObjects();
		void SaveObjects();

		bool LoadModelFromFile(const std::string& filePath);

		void ModifyBrokenOBJFile(const std::string& filePath);

		static inline std::map<std::string, ObjectData> m_Objects;
	};
}