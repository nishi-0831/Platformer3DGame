#include "Draw.h"
#include "CameraSystem.h"
#include "DirectWrite.h"
#include "Fbx.h"
#include "Figure.h"
#include "Game.h"
#include "ImGuiRenderer.h"
#include "Image.h"
#include "MTAssert.h"
#include "MTStringUtility.h"
#include "OBJ.h"
#include "PlaneUVScroll.h"
#include "ReleaseUtility.h"
#include "Sprite.h"
#include "TextCache.h"
#include "Transform.h"
#include <cmath>
#include <dwrite.h>

void mtgb::Draw::CheckSetShader(ShaderType _default)
{
	if (onceShaderType_ == ShaderType::MAX)
	{
		// シェーダがセットされていないなら既定シェーダ
		DirectX11Draw::SetShader(_default);
	}
	else
	{
		// シェーダがセットされているなら優先
		DirectX11Draw::SetShader(onceShaderType_);
		onceShaderType_ = ShaderType::MAX; // 一度使ったら外す
	}
}

void mtgb::Draw::Box(const Vector2Int& _begin, const Vector2Int& _end, Color _color, const UIParams& _uiParams)
{
	CheckSetShader(ShaderType::FIGURE);
	const Vector2F ratio = Game::System<Screen>().GetSizeRatio();

	Box(RectInt::FromLine(_begin, _end), _color, _uiParams);
}

void mtgb::Draw::Box(const RectInt& _rect, Color _color, const UIParams& _uiParams)
{
	uiDrawCommands_.insert({ _uiParams,
							 [=]()
							 {
								 CheckSetShader(ShaderType::FIGURE);

								 const Vector2F ratio = Game::System<Screen>().GetSizeRatio();

								 Game::System<Draw>().pFigure_->Draw(RectF { _rect.point, _rect.size }, _color);
							 } });
}

void mtgb::Draw::Image(
	ImageHandle _hImage,
	const RectF& _draw,
	const RectF& _cut,
	float _rotationZ,
	const UIParams& _uiParams,
	Color _color
)
{
	uiDrawCommands_.insert(
		{ _uiParams,
		  [=]()
		  {
			  CheckSetShader(ShaderType::SPRITE2_D);
			  Sprite* pSprite { Game::System<mtgb::Image>().GetSprite(_hImage) };

			  const Vector2F ratio = Game::System<Screen>().GetSizeRatio();

			  pSprite->Draw(RectF { _draw.point, _draw.size }, _rotationZ, RectF { _cut.point, _cut.size }, _color);
		  } }
	);
}

void mtgb::Draw::Image(ImageHandle _hImage, const RectF& _draw, const UIParams& _uiParams, Color _color)
{
	Image(_hImage, _draw, { Vector2F::Zero(), Image::GetSizeF(_hImage) }, 0.0f, _uiParams, _color);
}

void mtgb::Draw::Image(ImageHandle _hImage, const Transform& _transform, Color _color, const UIParams& _uiParams)
{
	uiDrawCommands_.emplace(
		_uiParams,
		[=]()
		{
			CheckSetShader(ShaderType::SPRITE2_D);

			Sprite* pSprite { Game::System<mtgb::Image>().GetSprite(_hImage) };

			const Transform* pCameraTransform = &(Game::System<CameraSystem>().GetTransform());
			pSprite->Draw(&_transform, pCameraTransform, pSprite->GetSize(), _color);
		}
	);
}

void mtgb::Draw::Text(
	const TextHandle _hText,
	const Vector2F& _origin,
	TextAlignment _alignment,
	const UIParams& _uiParams
)
{
	uiDrawCommands_.emplace(
		_uiParams,
		[&]()
		{
			DirectX11Draw::SetIsWriteToDepthBuffer(false);
			CheckSetShader(ShaderType::SPRITE2_D);

			TextLayoutData* layoutData = Game::System<mtgb::TextCache>().GetTextLayoutData(_hText);
			FontFormatData* formatData = Game::System<mtgb::TextCache>().GetOrCreateTextFormat(layoutData->fontSize);

			Game::System<mtgb::DirectWrite>().SetTextAlignment(_alignment, layoutData->layout);

			Game::System<mtgb::DirectWrite>()
				.Draw(layoutData->layout, _origin.x, (_origin.y + formatData->pixelFontMetrics.textTopOffset));
		}
	);
}

void mtgb::Draw::ImmediateTextW(
	std::wstring_view _text,
	Vector2F _topLeft,
	int _size,
	TextAlignment _alignment,
	const UIParams& _uiParams
)
{
	Vector2Int layoutBoxSize = Game::System<Screen>().GetInitialSize();

	ImmediateTextW(
		_text,
		{ _topLeft.x, _topLeft.y, static_cast<float>(layoutBoxSize.x), static_cast<float>(layoutBoxSize.y) },
		_size,
		_alignment,
		_uiParams
	);
}

void mtgb::Draw::ImmediateTextW(
	std::wstring_view _text,
	RectF _rect,
	int _size,
	TextAlignment _alignment,
	const UIParams& _uiParams
)
{
	uiDrawCommands_.emplace(
		_uiParams,
		[=]()
		{
			DirectX11Draw::SetIsWriteToDepthBuffer(false);
			CheckSetShader(ShaderType::SPRITE2_D);

			FontFormatData* formatData =
				Game::System<mtgb::TextCache>().GetOrCreateTextFormat(CalcScaledFontSize(_size));
			Game::System<DirectWrite>().SetTextAlignment(_alignment, formatData->format);
			const Vector2F ratio = Game::System<Screen>().GetSizeRatio();

			Game::System<DirectWrite>().ImmediateDraw(
				_text,
				formatData->format,
				formatData->pixelFontMetrics,
				_rect.x * ratio.x,
				_rect.y * ratio.y,
				_rect.width * ratio.x,
				_rect.height * ratio.y
			);
		}
	);
}

void mtgb::Draw::ImmediateText(
	std::string_view _text,
	Vector2F _topLeft,
	int _size,
	TextAlignment _alignment,
	const UIParams& _uiParams
)
{
	Vector2Int layoutBoxSize = Game::System<Screen>().GetInitialSize();
	ImmediateText(
		_text,
		{ _topLeft.x, _topLeft.y, static_cast<float>(layoutBoxSize.x), static_cast<float>(layoutBoxSize.y) },
		_size,
		_alignment,
		_uiParams
	);
}

void mtgb::Draw::ImmediateText(
	std::string_view _text,
	RectF _rect,
	int _size,
	TextAlignment _alignment,
	const UIParams& _uiParams
)
{
	uiDrawCommands_.emplace(
		_uiParams,
		[=]()
		{
			DirectX11Draw::SetIsWriteToDepthBuffer(false);
			CheckSetShader(ShaderType::SPRITE2_D);

			FontFormatData* formatData =
				Game::System<mtgb::TextCache>().GetOrCreateTextFormat(CalcScaledFontSize(_size));
			Game::System<DirectWrite>().SetTextAlignment(_alignment, formatData->format);

			const Vector2F ratio = Game::System<Screen>().GetSizeRatio();
			Game::System<DirectWrite>().ImmediateDraw(
				MultiToWide(_text.data()),
				formatData->format,
				formatData->pixelFontMetrics,
				_rect.x * ratio.x,
				_rect.y * ratio.y,
				_rect.width * ratio.y,
				_rect.height * ratio.y
			);
		}
	);
}

void mtgb::Draw::ChangeFontSize(int _size)
{
	currentDefaultFontSize_		   = _size;
	FontFormatData* fontFormatData = Game::System<mtgb::TextCache>().GetOrCreateTextFormat(_size);
	Game::System<DirectWrite>().ChangeFormat(fontFormatData->format, fontFormatData->pixelFontMetrics);
}

void mtgb::Draw::ChangeTextAlignment(TextAlignment _alignment)
{
	currentDefaultTextAlignment_ = _alignment;
}

void mtgb::Draw::OBJModel(const OBJModelHandle _hOBJModel, const Transform* _pTransform)
{
	CheckSetShader(ShaderType::FBX_PARTS);

	Game::System<mtgb::OBJ>().Draw((int)_hOBJModel, _pTransform);
}

void mtgb::Draw::FBXModel(
	const FBXModelHandle _hFBXModel,
	const Transform& _pTransform,
	const int _frame,
	ShaderType _shaderType
)
{
	CheckSetShader(_shaderType);

	Game::System<mtgb::Fbx>().Draw(_hFBXModel, _pTransform, _frame);
}

void mtgb::Draw::SeaUVScroll(const Transform& _transform)
{
	Game::System<Draw>().pSeaPlane_->Draw(_transform);
}

mtgb::Draw::Draw()
	: pFbxModel_ { nullptr }
	, pFigure_ { nullptr }
	, pGround_ { nullptr }
	, pSeaPlane_ { nullptr }
{
}

mtgb::Draw::~Draw()
{
	SAFE_DELETE(pFigure_);
	SAFE_RELEASE(pFbxModel_);
	SAFE_DELETE(pGround_);
	SAFE_DELETE(pSeaPlane_);
}

void mtgb::Draw::Initialize()
{
	pFigure_ = new Figure {};
	pFigure_->Initialize();
	/*

	pFbxModel_ = new FbxModel{};
	pFbxModel_->Load("Model/GroundPlane.fbx");
	FbxParts* pParts{ pFbxModel_->GetFbxParts(0) };

	pGround_ = new Ground{ pParts->GetNode() };
	pGround_->Initialize();*/
	pSeaPlane_ = new PlaneUVScroll();
	pSeaPlane_->Initialize();
	pSeaPlane_->LoadTexture(L"Image/sea.png");
}

void mtgb::Draw::Update() {}

void mtgb::Draw::FlushUIDrawCommands(GameObjectLayerFlag _layer)
{
	using mtbit::operator|;
	for (auto& drawCommand : uiDrawCommands_)
	{
		if (drawCommand.params.layerFlag.Has(_layer))
		{
			drawCommand.drawFunction();
		}
	}
}

void mtgb::Draw::ClearUICommands()
{
	uiDrawCommands_.clear();
}

int mtgb::Draw::CalcScaledFontSize(int _baseSize)
{
	const Vector2F ratio = Game::System<Screen>().GetSizeRatio();
	float avg			 = (ratio.x + ratio.y) * 0.5f;
	return static_cast<int>(std::roundf(_baseSize * avg));
}

mtgb::ShaderType mtgb::Draw::onceShaderType_ { ShaderType::MAX };
int mtgb::Draw::currentDefaultFontSize_ { 36 };
mtgb::TextAlignment mtgb::Draw::currentDefaultTextAlignment_ { TextAlignment::CENTER };
mtgb::UIParams mtgb::Draw::defaultUIParams_ {};
std::multiset<mtgb::UIDrawCommand> mtgb::Draw::uiDrawCommands_ {};