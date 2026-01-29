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

void mtgb::Draw::CheckSetShader(const ShaderType _default)
{
	if (onceShaderType_ == ShaderType::Max)
	{
		// シェーダがセットされていないなら既定シェーダ
		DirectX11Draw::SetShader(_default);
	}
	else
	{
		// シェーダがセットされているなら優先
		DirectX11Draw::SetShader(onceShaderType_);
		onceShaderType_ = ShaderType::Max; // 一度使ったら外す
	}
}

void mtgb::Draw::Box(const Vector2Int& _begin, const Vector2Int& _end, const Color& _color, const UIParams& _uiParams)
{
	CheckSetShader(ShaderType::Figure);
	const Vector2F ratio = Game::System<Screen>().GetSizeRatio();

	Box(RectInt::FromLine(_begin, _end), _color, _uiParams);
}

void mtgb::Draw::Box(const RectInt& _rect, const Color& _color, const UIParams& _uiParams)
{
	uiDrawCommands_.insert(
		{_uiParams,
		 [=]()
		 {
			 CheckSetShader(ShaderType::Figure);

			 const Vector2F ratio = Game::System<Screen>().GetSizeRatio();

			 Game::System<Draw>().pFigure_->Draw(RectF{_rect.point, _rect.size}, _color);
		 }}
	);
}

void mtgb::Draw::Image(
	const ImageHandle _hImage,
	const RectF& _draw,
	const RectF& _cut,
	const float _rotationZ,
	const UIParams& _uiParams,
	const Color& _color
)
{
	uiDrawCommands_.insert(
		{_uiParams,
		 [=]()
		 {
			 CheckSetShader(ShaderType::Sprite2D);
			 Sprite* pSprite{Game::System<mtgb::Image>().GetSprite(_hImage)};

			 const Vector2F ratio = Game::System<Screen>().GetSizeRatio();

			 pSprite->Draw(RectF{_draw.point, _draw.size}, _rotationZ, RectF{_cut.point, _cut.size}, _color);
		 }}
	);
}

void mtgb::Draw::Image(const ImageHandle _hImage, const RectF& _draw, const UIParams& _uiParams, const Color& _color)
{
	Image(_hImage, _draw, {Vector2F::Zero(), Image::GetSizeF(_hImage)}, 0.0f, _uiParams, _color);
}

void mtgb::Draw::Image(
	const ImageHandle _hImage,
	const Transform& _transform,
	const Color& _color,
	const UIParams& _uiParams
)
{
	uiDrawCommands_.insert(
		{_uiParams,
		 [=]()
		 {
			 CheckSetShader(ShaderType::Sprite2D);

			 Sprite* pSprite{Game::System<mtgb::Image>().GetSprite(_hImage)};

			 const Transform* pCameraTransform = &(Game::System<CameraSystem>().GetTransform());
			 pSprite->Draw(&_transform, pCameraTransform, pSprite->GetSize(), _color);
		 }}
	);
}

void mtgb::Draw::Image(
	const ImageHandle _hImage,
	Transform&& _transform,
	const Color& _color,
	const UIParams& _uiParams
)
{
	uiDrawCommands_.insert(
		{_uiParams,
		 [=, transform = std::move(_transform)]() mutable
		 {
			 CheckSetShader(ShaderType::Sprite2D);

			 Sprite* pSprite{Game::System<mtgb::Image>().GetSprite(_hImage)};

			 const Transform* pCameraTransform = &(Game::System<CameraSystem>().GetTransform());
			 pSprite->Draw(&transform, pCameraTransform, pSprite->GetSize(), _color);
		 }}
	);
}

void mtgb::Draw::Model(const ModelHandle _hModel, const Transform* _pTransform)
{
	// TODO: FbxとObjをモデルとしてハンドル含め統合、自動分岐する
	massert(false && "Draw::Modelが呼ばれていますが未実装です。FbxとObjで別関数を呼んでください。 @Draw::Model");
}

void mtgb::Draw::Text(
	const TextHandle _hText,
	const Vector2F& _origin,
	TextAlignment _alignment,
	const UIParams& _uiParams
)
{
	uiDrawCommands_.insert(
		{_uiParams,
		 [=]()
		 {
			 DirectX11Draw::SetIsWriteToDepthBuffer(false);
			 CheckSetShader(ShaderType::Sprite2D);

			 TextLayoutData* layoutData = Game::System<mtgb::TextCache>().GetTextLayoutData(_hText);
			 FontFormatData* formatData = Game::System<mtgb::TextCache>().GetOrCreateTextFormat(layoutData->fontSize);

			 Game::System<mtgb::DirectWrite>().SetTextAlignment(_alignment, layoutData->layout);

			 Game::System<mtgb::DirectWrite>()
				 .Draw(layoutData->layout, _origin.x, (_origin.y + formatData->pixelFontMetrics.textTopOffset));
		 }}
	);
}

void mtgb::Draw::ImmediateTextW(
	const std::wstring& _text,
	Vector2F _topLeft,
	int _size,
	TextAlignment _alignment,
	const UIParams& _uiParams
)
{
	Vector2Int layoutBoxSize = Game::System<Screen>().GetInitialSize();

	ImmediateTextW(
		_text,
		{_topLeft.x, _topLeft.y, static_cast<float>(layoutBoxSize.x), static_cast<float>(layoutBoxSize.y)},
		_size,
		_alignment,
		_uiParams
	);
}

void mtgb::Draw::ImmediateTextW(
	const std::wstring& _text,
	RectF _rect,
	int _size,
	TextAlignment _alignment,
	const UIParams& _uiParams
)
{
	uiDrawCommands_.insert(
		{_uiParams,
		 [=]()
		 {
			 DirectX11Draw::SetIsWriteToDepthBuffer(false);
			 CheckSetShader(ShaderType::Sprite2D);

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
		 }}
	);
}

void mtgb::Draw::ImmediateText(
	const std::string& _text,
	Vector2F _topLeft,
	int _size,
	TextAlignment _alignment,
	const UIParams& _uiParams
)
{
	Vector2Int layoutBoxSize = Game::System<Screen>().GetInitialSize();
	ImmediateText(
		_text,
		{_topLeft.x, _topLeft.y, static_cast<float>(layoutBoxSize.x), static_cast<float>(layoutBoxSize.y)},
		_size,
		_alignment,
		_uiParams
	);
}

void mtgb::Draw::ImmediateText(
	std::string&& _text,
	Vector2F _topLeft,
	int _size,
	TextAlignment _alignment,
	const UIParams& _uiParams
)
{
	Vector2Int layoutBoxSize = Game::System<Screen>().GetInitialSize();
	ImmediateText(
		std::move(_text),
		{_topLeft.x, _topLeft.y, static_cast<float>(layoutBoxSize.x), static_cast<float>(layoutBoxSize.y)},
		_size,
		_alignment,
		_uiParams
	);
}

void mtgb::Draw::ImmediateText(
	const std::string& _text,
	RectF _rect,
	int _size,
	TextAlignment _alignment,
	const UIParams& _uiParams
)
{
	uiDrawCommands_.insert(
		{_uiParams,
		 [=]()
		 {
			 DirectX11Draw::SetIsWriteToDepthBuffer(false);
			 CheckSetShader(ShaderType::Sprite2D);

			 FontFormatData* formatData =
				 Game::System<mtgb::TextCache>().GetOrCreateTextFormat(CalcScaledFontSize(_size));
			 Game::System<DirectWrite>().SetTextAlignment(_alignment, formatData->format);

			 const Vector2F ratio = Game::System<Screen>().GetSizeRatio();
			 Game::System<DirectWrite>().ImmediateDraw(
				 MultiToWide(_text),
				 formatData->format,
				 formatData->pixelFontMetrics,
				 _rect.x * ratio.x,
				 _rect.y * ratio.y,
				 _rect.width * ratio.y,
				 _rect.height * ratio.y
			 );
		 }}
	);
}

void mtgb::Draw::ImmediateText(
	std::string&& _text,
	RectF _rect,
	int _size,
	TextAlignment _alignment,
	const UIParams& _uiParams
)
{

	uiDrawCommands_.insert(
		{_uiParams,
		 [=, text = std::move(_text)]() mutable
		 {
			 DirectX11Draw::SetIsWriteToDepthBuffer(false);
			 CheckSetShader(ShaderType::Sprite2D);

			 FontFormatData* formatData =
				 Game::System<mtgb::TextCache>().GetOrCreateTextFormat(CalcScaledFontSize(_size));
			 Game::System<DirectWrite>().SetTextAlignment(_alignment, formatData->format);

			 const Vector2F ratio = Game::System<Screen>().GetSizeRatio();
			 Game::System<DirectWrite>().ImmediateDraw(
				 MultiToWide(text),
				 formatData->format,
				 formatData->pixelFontMetrics,
				 _rect.x * ratio.x,
				 _rect.y * ratio.y,
				 _rect.width * ratio.x,
				 _rect.height * ratio.y
			 );
		 }}
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
	CheckSetShader(ShaderType::FbxParts);

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
	: pFigure_{nullptr}
	, pGround_{nullptr}
	, pFbxModel_{nullptr}
	, pSeaPlane_{nullptr}
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
	pFigure_ = new Figure{};
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

void mtgb::Draw::Update()
{
}

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

ShaderType mtgb::Draw::onceShaderType_{ShaderType::Max};
int mtgb::Draw::currentDefaultFontSize_{36};
TextAlignment mtgb::Draw::currentDefaultTextAlignment_{TextAlignment::center};
mtgb::UIParams mtgb::Draw::defaultUIParams_{};
std::multiset<mtgb::UIDrawCommand> mtgb::Draw::uiDrawCommands_{};