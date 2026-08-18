#include "stdafx.h"
#include "StageManager.h"

#include <fstream>

nlohmann::json GetJson(const char* _path)
{
	std::ifstream inputFile(_path);
	massert(inputFile.is_open() && "failed to open JSON");

	nlohmann::json ret;
	try
	{
		inputFile >> ret;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		massert(false && e.what());
	}
	return ret;
}

void StageManager::Initialize()
{
	stageJsons_[StageID::STAGE_ONE]				= GetJson("Stage/data12.json");
	stageJsons_[StageID::STAGE_CLEAR_SCENE]		= GetJson("Stage/result_scene.json");
	stageJsons_[StageID::STAGE_GAME_OVER_SCENE] = GetJson("Stage/game_over_scene2.json");
	stageJsons_[StageID::STAGE_TITLE_SCENE]		= GetJson("Stage/title_scene_2.json");
}

void StageManager::Update() {}

std::optional<nlohmann::json> StageManager::GetStageJson(StageID _stageID)
{
	if (stageJsons_.contains(_stageID))
	{
		return stageJsons_[_stageID];
	}
	return std::nullopt;
}

void StageManager::InitializeStage(StageID _stageID)
{
	stageCleared_[_stageID] = false;
}

void StageManager::StartStage(StageID _stageID)
{
	InitializeStage(_stageID);
	currStage_ = _stageID;
}

bool StageManager::IsCleared(StageID _stageID)
{
	return stageCleared_[_stageID];
}

bool StageManager::IsClearedCurrentStage()
{
	return stageCleared_[currStage_];
}

void StageManager::ClearStage(StageID _stageID)
{
	stageCleared_[_stageID] = true;
}

void StageManager::ClearCurrentStage()
{
	stageCleared_[currStage_] = true;
}

StageID StageManager::GetCurrentStage()
{
	return currStage_;
}
