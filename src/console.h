#pragma once
#include <string>
#include <vector>

namespace Sable {
	class Console {
	public:
		Console(const Console&) = delete;
		Console& operator=(const Console&) = delete;

		static void Init();
		static void Shutdown();

		static void Log(const char* format, ...);
		static void DisplayConsoleImGui();

	private:
		Console();
		~Console();

		static inline std::vector<std::string> m_Logs;
	};
}