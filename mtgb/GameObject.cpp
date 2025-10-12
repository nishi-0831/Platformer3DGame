#include "GameObject.h"
#include "SceneSystem.h"
#include "Transform.h"
#include "MTAssert.h"


mtgb::GameObject::GameObject(const GAME_OBJECT_DESC& _desc) :
	name_{_desc.name},
	status_
	{
		.isActive_ = _desc.isActive,
		.callUpdate_ = _desc.callUpdate,
		.callDraw_ = _desc.callDraw,
		.toDestroy_ = FALSE,  // �ŏ��͍폜���Ȃ�
	},
	layerFlag_{ _desc.layerFlag },
	tag_{ _desc.tag }
{
	Transform* pTransform_{ Component<Transform>() };
	pTransform_->position = _desc.position;
	pTransform_->rotate = _desc.rotate;
	pTransform_->scale = _desc.scale;
}

mtgb::GameObject::GameObject(const GameObject& _other)
	:Entity()
	,status_{_other.status_}
	,componentsFlag_{_other.componentsFlag_}
{
}

mtgb::GameObject::~GameObject()
{
	massert(status_.toDestroy_ &&
		"�Q�[���I�u�W�F�N�g���폜����Ƃ��͒���delete���Ăяo���Ȃ��ł��������I");
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
