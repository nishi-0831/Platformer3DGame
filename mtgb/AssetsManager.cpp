#include "AssetsManager.h"
#include "Windows.h"
#include <filesystem>
namespace fs = std::filesystem;
namespace
{
	static const char* ASSETS_DIRECTORY_PATH { "Assets" };
}

mtgb::AssetsManager::AssetsManager() {}

mtgb::AssetsManager::~AssetsManager() {}

void mtgb::AssetsManager::Initialize()
{
	SetCurrentDirectory(ASSETS_DIRECTORY_PATH);
}

void mtgb::AssetsManager::Update() {}
