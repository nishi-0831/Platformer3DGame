#include "GameObject.h"
#include "SceneSystem.h"
#include "Transform.h"
#include "MTAssert.h"
#include "EntityManager.h"
#include "MTStringUtility.h"
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
	entityId_ = Game::System<EntityManager>().CreateEntity();
	isNotCalledStart_ = true;

	Transform* pTransform_{ Component<Transform>() };
	pTransform_->position = _desc.position;
	pTransform_->rotate = _desc.rotate;
	pTransform_->scale = _desc.scale;
}

mtgb::GameObject::GameObject()
	: status_
	{
		.isActive_ = true,
		.callUpdate_ = true,
		.callDraw_ = true,
		.toDestroy_ = FALSE
	}
	, layerFlag_{AllLayer()}
	, tag_{GameObjectTag::Untagged}
{
	entityId_ = Game::System<EntityManager>().CreateEntity();
	isNotCalledStart_ = true;

}

mtgb::GameObject::GameObject(const GameObject& _other)
	:Entity()
	,status_{_other.status_}
	,componentsFlag_{_other.componentsFlag_}
	, tag_{GameObjectTag::Untagged}
{
	isNotCalledStart_ = true;
}

mtgb::GameObject::~GameObject()
{
	massert(status_.toDestroy_ &&
		"ゲームオブジェクトを削除するときは直接deleteを呼び出さないでください！");
}

nlohmann::json mtgb::GameObject::Serialize() const
{
	nlohmann::json j{};
	j["name"] = GetName();
	j["classType"] = mtgb::ExtractClassName(GetName());
	j["tag"] = GetTag();
	j["EntityId"] = entityId_;
	auto componentTypes = Game::System<ComponentRegistry>().GetComponentTypes(entityId_);
	if (componentTypes.has_value() == false)
	{
		return j;
	}
	for (const auto& typeIdx : (*componentTypes).get())
	{
		std::optional<std::type_index> componentPoolType = Game::System<ComponentRegistry>().GetComponentPoolType(typeIdx);
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
	tag_ = _json.at("tag").get<GameObjectTag>();
}

mtgb::GameObject* mtgb::GameObject::FindGameObject(const std::string& _name)
{
	return mtgb::Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_name);
}

void mtgb::GameObject::FindGameObjects(const std::string& _name, std::vector<GameObject*>* _pFoundGameObjects)
{
	mtgb::Game::System<SceneSystem>().GetActiveScene()->GetGameObjects(_name, _pFoundGameObjects);
}

mtgb::GameObject* mtgb::GameObject::FindGameObject(const EntityId _entityId)
{
	return mtgb::Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_entityId);
}

void mtgb::GameObject::FindGameObjects(GameObjectTag _tag, std::vector<GameObject*>* _pFoundGameObjects)
{
	mtgb::Game::System<SceneSystem>().GetActiveScene()->GetGameObjects(_tag, _pFoundGameObjects);
}
