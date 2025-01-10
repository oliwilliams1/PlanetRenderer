#include "assetManager.h"
#include <iostream>

namespace AssetManager {
	static System* s_Instance = nullptr;

	System::System() {
		std::cout << "asset manager constuctor" << std::endl;
	}

	System::~System() {
		std::cout << "asset manager destructor" << std::endl;
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
}