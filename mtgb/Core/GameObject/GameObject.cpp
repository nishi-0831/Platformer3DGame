#include "Core/GameObject/GameObject.h"
#include "Core/SceneSystem.h"
#include "Components/Transform/Transform.h"
#include "MTAssert.h"
#include "Core/EntityManager.h"
#include "MTStringUtility.h"
#include "Editor/MTImGui.h"
mtgb::GameObject::GameObject(const GAME_OBJECT_DESC& _desc) :
	name_{_desc.name},
	status_
	{
		.isActive_ = _desc.isActive,
		.callUpdate_ = _desc.callUpdate,
		.callDraw_ = _desc.callDraw,
		.toDestroy_ = FALSE,  // 最初は削除しない
	},
	layerFlag_{ _desc.layerFlag },
	tag_{ _desc.tag }
{
	entityId_		  = Game::System<EntityManager>().CreateEntity();
	isNotCalledStart_ = true;
	isInspectable_	  = true;
	Transform* pTransform_ { Component<Transform>() };
	pTransform_->position = _desc.position;
	pTransform_->rotate	  = _desc.rotate;
	pTransform_->scale	  = _desc.scale;
}

mtgb::GameObject::GameObject()
	: status_ { .isActive_ = true, .callUpdate_ = true, .callDraw_ = true, .toDestroy_ = FALSE }
	, layerFlag_ { AllLayer() }
	, tag_ { GameObjectTag::UNTAGGED }
{
	entityId_		  = Game::System<EntityManager>().CreateEntity();
	name_			  = std::format("GameObject ({})", std::to_string(entityId_));
	isNotCalledStart_ = true;
	isInspectable_	  = true;
}

mtgb::GameObject::GameObject(const GameObject& _other)
	: Entity()
	, status_ { _other.status_ }
	, tag_ { GameObjectTag::UNTAGGED }
{
	isNotCalledStart_ = true;
	isInspectable_	  = true;
}

mtgb::GameObject::~GameObject()
{
	massert(status_.toDestroy_ && "ゲームオブジェクトを削除するときは直接deleteを呼び出さないでください！");
}

void mtgb::GameObject::ShowImGui()
{
	MTImGui::ShowComponents(Entity::entityId_);
	ImGui::Text("EntityId:%lld", Entity::entityId_);
}

nlohmann::json mtgb::GameObject::Serialize() const
{
	nlohmann::json j {};
	j["name"]			= GetName();
	j["classType"]		= mtgb::ExtractClassName(GetName());
	j["tag"]			= GetTag();
	j["EntityId"]		= entityId_;
	auto componentTypes = Game::System<ComponentRegistry>().GetComponentTypes(entityId_);
	if (componentTypes.has_value() == false)
	{
		return j;
	}
	for (const auto& typeIdx : (*componentTypes).get())
	{
		std::optional<std::type_index> componentPoolType =
			Game::System<ComponentRegistry>().GetComponentPoolType(typeIdx);
		if (componentPoolType.has_value() == false)
			continue;

		nlohmann::json componentJson = Game::SerializeComponent(componentPoolType.value(), entityId_);
		j.merge_patch(componentJson);
	}
	return j;
}

void mtgb::GameObject::Deserialize(const nlohmann::json& _json)
{
	name_ = _json.at("name").get<std::string>();
	tag_  = _json.at("tag").get<GameObjectTag>();
}

mtgb::GameObject* mtgb::GameObject::FindGameObject(GameObjectTag _tag)
{
	return mtgb::Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_tag);
}

mtgb::GameObject* mtgb::GameObject::FindGameObject(std::string_view _name)
{
	return mtgb::Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_name);
}

void mtgb::GameObject::FindGameObjects(std::string_view _name, std::vector<GameObject*>* _pFoundGameObjects)
{
	mtgb::Game::System<SceneSystem>().GetActiveScene()->GetGameObjects(_name, _pFoundGameObjects);
}

mtgb::GameObject* mtgb::GameObject::FindGameObject(EntityId _entityId)
{
	return mtgb::Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_entityId);
}

void mtgb::GameObject::FindGameObjects(GameObjectTag _tag, std::vector<GameObject*>* _pFoundGameObjects)
{
	mtgb::Game::System<SceneSystem>().GetActiveScene()->GetGameObjects(_tag, _pFoundGameObjects);
}

std::string mtgb::GameObject::GetClassTypeName() const
{
	return mtgb::ExtractClassName(name_);
}
