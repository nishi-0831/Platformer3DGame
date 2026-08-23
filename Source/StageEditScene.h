#pragma once
#include "GameScene.h"
/// <summary>
/// ステージを編集する用のシーン。編集モードがオンの状態で作成される
/// </summary>
class StageEditScene : public mtgb::GameScene
{
  public:
	StageEditScene();
	/// <summary>
	/// ステージのデフォルトデータとして使われるJSON
	/// </summary>
	/// <param name="_stageData"></param>
	StageEditScene(const nlohmann::json& _stageData);
	~StageEditScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;

  private:
	nlohmann::json stageData_;
};