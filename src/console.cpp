#include <iostream>
#include "console.h"
#include <imgui.h>
#include <cstdarg>
#include <chrono>
#include <iomanip>
#include <ctime>

namespace Sable {
	static Console* s_ConsoleInstance = nullptr;

	void Console::Init() {
		s_ConsoleInstance = new Console();
	}

	void Console::Shutdown() {
		delete s_ConsoleInstance;
		s_ConsoleInstance = nullptr;
	}

    void Console::Log(const char* format, ...) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm = *std::localtime(&now_time_t);

        // Get milliseconds
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        // Format time
        std::ostringstream oss;
        oss << std::setfill('0')
            << std::setw(2) << now_tm.tm_hour << ':'
            << std::setw(2) << now_tm.tm_min << ':'
            << std::setw(2) << now_tm.tm_sec << '.'
            << std::setw(3) << milliseconds.count();

        // Format the message
        char buffer[256]; // Adjust size as needed
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        std::string finalMessage = "[" + oss.str() + "] - " + buffer;

        std::cout << finalMessage << std::endl;
        m_Logs.push_back(finalMessage);
    }

	void Console::DisplayConsoleImGui() {
        ImGui::BeginChild("ConsoleChild", ImVec2(0, 0), true); // 0,0 for auto size, true for border

        // Loop through logs and display them
        for (int i = 0, size = m_Logs.size(); i < size; i++) {
            ImGui::Text(m_Logs[i].c_str());
        }

        // Snap to the bottom of the console
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f); // Scroll to the bottom
        }

        ImGui::EndChild();
	}

	Console::Console() {}
	Console::~Console() {
		m_Logs.clear();
	}
}