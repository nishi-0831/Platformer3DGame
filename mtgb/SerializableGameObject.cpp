#include "SerializableGameObject.h"
#include <format>

unsigned int mtgb::SerializableGameObject::generateCounter_ { 0 };


mtgb::SerializableGameObject::SerializableGameObject():GameObject(),ImGuiShowable(ShowType::INSPECTOR,Entity::entityId_) 
{
	// 型情報に登録された名前を取得
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(SerializableGameObject));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);
	displayName_		 = name_;
}

mtgb::SerializableGameObject::~SerializableGameObject() {}

void mtgb::SerializableGameObject::Update() {}

void mtgb::SerializableGameObject::Draw() const {}

void mtgb::SerializableGameObject::ShowImGui() 
{
	MTImGui::ShowComponents(Entity::entityId_);
	ImGui::Text("EntityId:%lld", Entity::entityId_);
}

void mtgb::SerializableGameObject::Start() {}
