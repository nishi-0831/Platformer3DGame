#include "MovingSaw.h"

unsigned int mtgb::MovingSaw::generateCounter_ { 0 };
mtgb::MovingSaw::MovingSaw()
	: Saw()
	, pInterpolator_ { Component<Interpolator>() }
{
	// 型情報に登録された名前を取得
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(MovingSaw));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);
}

mtgb::MovingSaw::~MovingSaw() {}

void mtgb::MovingSaw::Update()
{
	Saw::Update();
	pInterpolator_->UpdateProgress();
	pTransform_->position = pInterpolator_->EvaluatePos();
}

void mtgb::MovingSaw::ShowImGui()
{
	GameObject::ShowImGui();
	MTImGui::ShowComponent<Interpolator>(GetEntityId());
}
