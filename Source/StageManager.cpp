#include "stdafx.h"
#include "StageManager.h"
#include <fstream>

void StageManger::Initialize()
{
	std::string fileName = "data3.json";
	std::ifstream inputFile(fileName);
	nlohmann::json j;
	try
	{
		inputFile >> j;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		assert(false);
	}
	stageJsons_[StageID::STAGE_ONE] = j;
}

void StageManger::Update()
{
}

std::optional<nlohmann::json> StageManger::GetStageJson(StageID _stageID)
{
	if (stageJsons_.contains(_stageID))
	{
		return stageJsons_[_stageID];
	}
	return std::nullopt;
}
