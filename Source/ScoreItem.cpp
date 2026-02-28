#include "stdafx.h"
#include "ScoreItem.h"
#include <format>
unsigned int ScoreItem::generateCounter_ { 0 };

ScoreItem::ScoreItem()
	: GameObject()
	, pTransform_ { Component<Transform>() }
	, pCollider_ { Component<Collider>() }
	, pRigidBody_ { Component<RigidBody>() }
	, pMeshRenderer_ { Component<MeshRenderer>() }
	, addScore_ { 100 }
{
	pCollider_->colliderType_ = ColliderType::TYPE_SPHERE;
	pCollider_->isStatic_	  = false;
	pCollider_->SetRadius(pTransform_->scale.x * 0.5f);

	pRigidBody_->OnCollisionEnter(
		[this](EntityId _entityId)
		{
			GameObjectTag tag = FindGameObject(_entityId)->GetTag();
			if (tag == GameObjectTag::PLAYER)
			{
				Game::System<ScoreManager>().AddScore(addScore_);
				EffectParameters params;
				params.isLoop = false;
				Matrix4x4 worldMat;
				pTransform_->GenerateWorldMatrix(&worldMat);
				params.worldMat = worldMat;
				Game::System<EffectManager>().Play("ScoreItem", params);

				Audio::PlayOneShotFile("Sound/ItemGetSound.mp3");
				DestroyMe();
			}
		}
	);

	pMeshRenderer_->meshFileName = "Model/Ruby.fbx";
	pMeshRenderer_->SetMesh(Fbx::Load(pMeshRenderer_->meshFileName));
	pMeshRenderer_->layer	   = AllLayer();
	pMeshRenderer_->shaderType = ShaderType::FBX_PARTS;

	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(ScoreItem));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);
}

ScoreItem::~ScoreItem() {}

void ScoreItem::Update() {}

void ScoreItem::Start() {}

void ScoreItem::Draw() const {}
