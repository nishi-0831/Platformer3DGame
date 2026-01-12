#pragma once
#include <mtgb.h>

class FadeOutScreen : public mtgb::GameObject
{
  public:
	FadeOutScreen();
	FadeOutScreen(float _duration);
	~FadeOutScreen();

	void Update() override;
	void Draw() const override;
	void Start() override;
	void StartFadeOut();
	bool IsFinished();

  private:
	ImageRenderer* pImageRenderer_;
	float duration_;
	float elapsed_;
	bool isStarted_;
	bool isFinished_;
};