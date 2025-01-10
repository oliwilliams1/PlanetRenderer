#pragma once
#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

// Using chernos' alternative to a usual singleton
namespace AssetManager
{
	struct ObjectData {
		std::string texturePath;
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<unsigned int> indices;
	};

	class System
	{
	public:
		System(const System&) = delete;
		System& operator=(const System&) = delete;

		static void Init();
		static void Shutdown();

		static System& GetInstance();


	private:
		System();
		~System();

		std::vector<AssetManager::ObjectData> m_Objects;
	};
}