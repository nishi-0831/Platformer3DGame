#include "stdafx.h"
#include "FadeOutScreen.h"

FadeOutScreen::FadeOutScreen()
	: GameObject{}
	, pImageRenderer_{Component<ImageRenderer>()}
	, duration_{3.0f}
	, elapsed_{0.0f}
	, isStarted_{false}
	, isFinished_{false}
{
	pImageRenderer_->imageFileName_		 = "Image/Black.png";
	pImageRenderer_->handle_			 = Image::Load(pImageRenderer_->imageFileName_);
	pImageRenderer_->layer_				 = AllLayer();
	pImageRenderer_->uiParams_.depth	 = 1;
	pImageRenderer_->uiParams_.layerFlag = AllLayer();
	Vector2F screenSize					 = Game::System<Screen>().GetSizeF();

	pImageRenderer_->drawRect_ = RectF{Vector2F::Zero(), screenSize};

	pImageRenderer_->color_.component[static_cast<int32_t>(Color::Component::Alpha)] = 0;
}

FadeOutScreen::FadeOutScreen(float _duration)
	: FadeOutScreen{}
{
	duration_ = _duration;
}

FadeOutScreen::~FadeOutScreen()
{
}

void FadeOutScreen::Update()
{
	if (isStarted_ == false || isFinished_ == true)
		return;

	elapsed_ += Time::DeltaTimeF();
	float progress = elapsed_ / duration_;
	float alpha	   = (progress * static_cast<float>(UINT8_MAX));
	pImageRenderer_->color_.component[static_cast<int32_t>(Color::Component::Alpha)] = static_cast<int32_t>(alpha);

	if (progress >= 1.0f)
	{
		isFinished_ = true;
	}
}

void FadeOutScreen::Draw() const
{
}

void FadeOutScreen::Start()
{
}

void FadeOutScreen::StartFadeOut()
{
	isStarted_ = true;
}

bool FadeOutScreen::IsFinished()
{
	return isFinished_;
}
