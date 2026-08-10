#include "Draw.h"
#include "Fbx.h"
#include "Game.h"
#include "SceneSystem.h"

#include "PlaneUVScroll.h"
#include "ReleaseUtility.h"
#include "Transform.h"
#include <cmath>

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
	: pSeaPlane_ { nullptr }
{
}

mtgb::Draw::~Draw()
{
	SAFE_DELETE(pSeaPlane_);
}

void mtgb::Draw::Initialize()
{
	pSeaPlane_ = new PlaneUVScroll();
	pSeaPlane_->Initialize();
	pSeaPlane_->LoadTexture(L"Image/sea.png");
	Game::System<SceneSystem>().OnMove(
		[this]()
		{
			uiElements_.clear();
			uiElementDirty_ = false;
		}
	);
}

void mtgb::Draw::Update() {}

void mtgb::Draw::RenderUI(GameObjectLayerFlag _layer)
{
	if (uiElementDirty_)
	{
		std::stable_sort(
			uiElements_.begin(),
			uiElements_.end(),
			[](IUIRenderable* _left, IUIRenderable* _right)
			{
				return _left->GetUIParams().depth > _right->GetUIParams().depth;
			}
		);
		uiElementDirty_ = false;
	}
	using mtbit::operator|;
	for (auto element : uiElements_)
	{
		if (element->CanRender() && element->GetUIParams().layerFlag.Has(_layer))
		{
			element->Render();
		}
	}
}

int mtgb::Draw::CalcScaledFontSize(int _baseSize)
{
	const Vector2F ratio = Game::System<Screen>().GetSizeRatio();
	float avg			 = (ratio.x + ratio.y) * 0.5f;
	return static_cast<int>(std::roundf(_baseSize * avg));
}

void mtgb::Draw::RegisterUIElement(IUIRenderable* _e)
{
	uiElements_.push_back(_e);
	uiElementDirty_ = true;
}

void mtgb::Draw::UnregisterUIElement(IUIRenderable* _e)
{
	uiElements_.erase(std::remove(uiElements_.begin(), uiElements_.end(), _e), uiElements_.end());
}