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

void StageManger::InitializeStage(StageID _stageID)
{
	stageCleared_[_stageID] = false;
}

void StageManger::StartStage(StageID _stageID)
{
	InitializeStage(_stageID);
	currStage_ = _stageID;
}

bool StageManger::IsCleared(StageID _stageID)
{
	return stageCleared_[_stageID];
}

bool StageManger::IsClearedCurrentStage()
{
	return false;
}

void StageManger::ClearStage(StageID _stageID)
{
	stageCleared_[_stageID] = true;
}

void StageManger::ClearCurrentStage()
{
	stageCleared_[currStage_] = true;
}

StageID StageManger::GetCurrentStage()
{
	return currStage_;
}
