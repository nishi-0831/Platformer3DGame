#pragma once
#include <mtgb.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <optional>
#include "StageID.h"
#include <nlohmann/json.hpp>

/// <summary>
/// ステージを管理する
/// JSONやクリア状況を管理
/// </summary>
class StageManager : public ISystem
{
  public:
	void Initialize() override;
	void Update() override;
	/// <summary>
	/// ステージのIDに対応するJSONを返す
	/// </summary>
	/// <param name="_stageID">ステージのID</param>
	/// <returns>存在しないならnullopt</returns>
	std::optional<nlohmann::json> GetStageJson(StageID _stageID);
	/// <summary>
	/// ステージのクリア状況を初期化
	/// </summary>
	/// <param name="_stageID"></param>
	void InitializeStage(StageID _stageID);

	/// <summary>
	/// ステージのクリア状況を初期化
	/// 現在プレイしているステージとする
	/// </summary>
	/// <param name="_stageID"></param>
	void StartStage(StageID _stageID);
	/// <summary>
	/// ステージをクリアされたか返す
	/// </summary>
	/// <param name="_stageID"></param>
	/// <returns></returns>
	bool IsCleared(StageID _stageID);
	/// <summary>
	/// 現在のステージをクリアされたか返す
	/// </summary>
	/// <returns></returns>
	bool IsClearedCurrentStage();
	/// <summary>
	/// ステージをクリアした状態にする
	/// </summary>
	/// <param name="_stageID"></param>
	void ClearStage(StageID _stageID);
	/// <summary>
	/// 現在のステージをクリアした状態にする
	/// </summary>
	void ClearCurrentStage();
	/// <summary>
	/// 現在のステージのIDを返す
	/// </summary>
	/// <returns></returns>
	StageID GetCurrentStage();

  private:
	// 現在のステージのID
	StageID currStage_;
	// ステージのIDに対応したJSON
	std::unordered_map<StageID, nlohmann::json> stageJsons_;
	std::unordered_map<StageID, bool> stageCleared_; // IDに対応するステージをクリアしたか
};
