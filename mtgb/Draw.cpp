#include "Draw.h"
#include "Model/Fbx.h"
#include "Core/Game.h"
#include "SceneSystem.h"

#include "ReleaseUtility.h"
#include "Components/Transform/Transform.h"
#include "Screen.h"
#include <cmath>

mtgb::Draw::Draw() {}

mtgb::Draw::~Draw() {}

void mtgb::Draw::Initialize()
{
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