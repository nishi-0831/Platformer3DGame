#include "stdafx.h"
#include "ScoreItem.h"
#include <format>
unsigned int ScoreItem::generateCounter_{ 0 };

ScoreItem::ScoreItem()
	: GameObject()
{
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(ScoreItem));
	name_ = std::format("{} ({})", typeName, generateCounter_++);
}

ScoreItem::~ScoreItem()
{
}

void ScoreItem::Update()
{
}

void ScoreItem::Start()
{
	pTransform_ = Component<Transform>();
	pCollider_ = Component<Collider>();
	pRigidBody_ = Component<RigidBody>();

	pRigidBody_->OnCollisionEnter([this](EntityId _entityId)
		{
			GameObjectTag tag = FindGameObject(_entityId)->GetTag();
			if (tag == GameObjectTag::Player)
			{
				DestroyMe();
			}
		});
}

void ScoreItem::Draw() const
{
}

std::vector<IComponentMemento*> ScoreItem::GetDefaultMementos(EntityId _entityId) const
{
    std::vector<IComponentMemento*> mementos;

	TransformData transformData
	{
		.position{0,0,0},
		.scale{1,1,1}
	};

	ColliderData colliderData
	{
		.colliderType{ColliderType::TYPE_SPHERE},
		.isStatic{false},
		.colliderTag{},
		.center{transformData.position},
		.radius{transformData.scale.x * 0.5f},
		.extents{transformData.scale * 0.5f},
	};

	MeshRendererData meshData
	{
		.meshFileName{"Model/Ruby.fbx"},
		.meshHandle{Fbx::Load(meshData.meshFileName)},
		.layer{AllLayer()},
		.shaderType{ShaderType::FbxParts}
	};

	mementos.push_back(new TransformMemento(_entityId, transformData));
	mementos.push_back(new ColliderMemento(_entityId, colliderData));
	mementos.push_back(new MeshRendererMemento(_entityId, meshData));
    return mementos;
}
