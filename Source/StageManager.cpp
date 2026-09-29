#include "stdafx.h"
#include "StageManager.h"

#include <fstream>

nlohmann::json GetJson(const char* _path)
{
	// ファイルを読み込む
	std::ifstream inputFile(_path);
	massert(inputFile.is_open() && "failed to open JSON");

	// JSON形式に変換
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
	// 各ステージに対応したデータを読み込み
	stageJsons_[StageID::STAGE_ONE]				= GetJson("Stage/data13.json");
	stageJsons_[StageID::STAGE_CLEAR_SCENE]		= GetJson("Stage/result_scene.json");
	stageJsons_[StageID::STAGE_GAME_OVER_SCENE] = GetJson("Stage/game_over_scene2.json");
	stageJsons_[StageID::STAGE_TITLE_SCENE]		= GetJson("Stage/title_scene3.json");
}

void StageManager::Update() {}

// StageIDに対応するJSONを返す
std::optional<nlohmann::json> StageManager::GetStageJson(StageID _stageID)
{
	// StageIDに対応するJSONがあるならそれを返す
	if (stageJsons_.contains(_stageID))
	{
		return stageJsons_[_stageID];
	}

	// 対応するJSONがなかった
	return std::nullopt;
}

void StageManager::InitializeStage(StageID _stageID)
{
	stageCleared_[_stageID] = false;
}

void StageManager::StartStage(StageID _stageID)
{
	// ステージを初期化
	InitializeStage(_stageID);
	// 現在のステージに記録
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