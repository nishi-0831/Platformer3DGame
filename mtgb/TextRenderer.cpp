#include "stdafx.h"
#include "TextRenderer.h"
#include "Draw.h"
#include "DirectX11Draw.h"
#include "TextCache.h"
#include "DirectWrite.h"
#include "Screen.h"
#include <dwrite.h>

mtgb::TextRenderer::TextRenderer() {}
mtgb::TextRenderer::TextRenderer(EntityId _entityId)
	: IComponent(_entityId)
	, alignment { TextAlignment::BOTTOM_LEFT }
	, text_ { "" }
	, rect_ { 0, 0, 0, 0 }
	, fontSize_ { 24 }
	, params_ { .depth = 0, .layerFlag = AllLayer() }
{
}

void mtgb::TextRenderer::Render() const
{
	DirectX11Draw::SetIsWriteToDepthBuffer(false);
	Game::System<Draw>().CheckSetShader(ShaderType::SPRITE2_D);

	FontFormatData* formatData =
		Game::System<TextCache>().GetOrCreateTextFormat(Game::System<mtgb::Draw>().CalcScaledFontSize(fontSize_));
	Game::System<DirectWrite>().SetTextAlignment(alignment, formatData->format);

	const Vector2F ratio = Game::System<Screen>().GetSizeRatio();
	Game::System<DirectWrite>().ImmediateDraw(
		UTF8ToWide(text_.c_str()),
		formatData->format,
		formatData->pixelFontMetrics,
		rect_.x * ratio.x,
		rect_.y * ratio.y,
		rect_.width * ratio.y,
		rect_.height * ratio.y
	);
}

void mtgb::TextRenderer::Initialize()
{
	Game::System<mtgb::Draw>().RegisterUIElement(this);
}

void mtgb::TextRenderer::Reset()
{
	Game::System<mtgb::Draw>().UnregisterUIElement(this);
}

bool mtgb::TextRenderer::CanRender() const
{
	return enabled_;
}

UIParams mtgb::TextRenderer::GetUIParams() const
{
	return params_;
}
