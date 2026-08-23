#include "stdafx.h"
#include "ImageRenderer.h"
#include "Draw.h"
#include "Image/Image.h"
#include "DirectX11Draw.h"
#include "Image/Sprite.h"

mtgb::ImageRenderer::ImageRenderer()
	: color_ { Color::WHITE }
{
}
mtgb::ImageRenderer::ImageRenderer(EntityId _entityId)
	: IComponent { _entityId }
	, handle_ { INVALID_HANDLE }
	, imageFileName_ { "" }
	, drawRect_ {}
	, cutRect_ {}
	, rotationZ_ { 0.0f }
	, layer_ { AllLayer() }
	, color_ { Color::WHITE }
{
}

void mtgb::ImageRenderer::Render() const
{
	Sprite* pSprite = { Game::System<Image>().GetSprite(handle_) };
	pSprite->Draw(drawRect_, rotationZ_, { Vector2F::Zero(), Image::GetSizeF(handle_) }, color_);
}

bool mtgb::ImageRenderer::CanRender() const
{
	return (handle_ != INVALID_ENTITY) && enabled_;
}

void mtgb::ImageRenderer::Initialize()
{
	Game::System<mtgb::Draw>().RegisterUIElement(this);
}

void mtgb::ImageRenderer::Reset()
{
	Game::System<mtgb::Draw>().UnregisterUIElement(this);
}

GameObjectLayerFlag mtgb::ImageRenderer::GetLayer() const
{
	return layer_;
}

UIParams mtgb::ImageRenderer::GetUIParams() const
{
	return uiParams_;
}

void mtgb::ImageRenderer::OnPostRestore()
{
	if (imageFileName_.empty() == false)
	{
		handle_ = Image::Load(imageFileName_);
	}
	Game::System<mtgb::Draw>().RegisterUIElement(this);
}
