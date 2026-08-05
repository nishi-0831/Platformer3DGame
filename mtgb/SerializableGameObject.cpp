#include "SerializableGameObject.h"
#include <format>

unsigned int mtgb::SerializableGameObject::generateCounter_ { 0 };

mtgb::SerializableGameObject::SerializableGameObject()
	: GameObject()
{
	// 型情報に登録された名前を取得
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(SerializableGameObject));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);
}

mtgb::SerializableGameObject::~SerializableGameObject() {}

void mtgb::SerializableGameObject::Update() {}

void mtgb::SerializableGameObject::Draw() const {}

void mtgb::SerializableGameObject::Start() {}
