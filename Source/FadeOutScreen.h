#pragma once
#include <mtgb.h>

/// <summary>
/// 画面のフェードアウトを行うゲームオブジェクト
/// </summary>
class FadeOutScreen : public mtgb::GameObject
{
  public:
	FadeOutScreen();
	FadeOutScreen(float _duration);
	~FadeOutScreen();

	void Update() override;
	void Draw() const override;
	void Start() override;
	/// <summary>
	/// フェードアウトを開始する
	/// </summary>
	void StartFadeOut();
	/// <summary>
	/// フェードアウトが終了しているか否かを返す
	/// </summary>
	/// <returns></returns>
	bool IsFinished();

  private:
	ImageRenderer* pImageRenderer_;
	/// <summary>
	/// 何秒間かけてフェードアウトを行うかの秒数
	/// </summary>
	float duration_;
	/// <summary>
	/// 経過時間
	/// </summary>
	float elapsed_;
	/// <summary>
	/// フェードアウトが開始しているか否か
	/// </summary>
	bool isStarted_;
	/// <summary>
	/// フェードアウトが終了しているか否か
	/// </summary>
	bool isFinished_;
};