#include "stdafx.h"
#include "Goal.h"
#include "ResultScene.h"
#include "StageManager.h"
#include "GameEvents.h"
unsigned int Goal::generateCounter_ { 0 };

Goal::Goal()
	: GameObject()
	, ImGuiShowable(ShowType::INSPECTOR, GetEntityId())
	, pTransform_ { Component<Transform>() }
	, pCollider_ { Component<Collider>() }
	, pRigidBody_ { Component<RigidBody>() }
	, pMeshRenderer_ { Component<MeshRenderer>() }
	, transitionSceneDelay_ { 4.0f }
{
	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	pCollider_->isStatic_	  = false;
	pCollider_->SetExtents(pTransform_->scale * 0.5f);
	pMeshRenderer_->meshFileName = "Model/Goal.fbx";
	pMeshRenderer_->SetMesh(Fbx::Load(pMeshRenderer_->meshFileName));
	pMeshRenderer_->layer	   = AllLayer();
	pMeshRenderer_->shaderType = ShaderType::FBX_PARTS;

	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(Goal));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);
	displayName_		 = name_;
}

Goal::~Goal()
{
	if (auto effect = pEffect_.lock())
	{
		effect->destroyMe = true;
		effect->isLoop	  = true;
	}
}

void Goal::Update() {}

void Goal::Start()
{
	pTransform_ = Component<Transform>();
	pRigidBody_ = Component<RigidBody>();

	pRigidBody_->OnCollisionEnter(
		[this](EntityId _entityId)
		{
			GameObject* pGameObj = FindGameObject(_entityId);
			if (pGameObj == nullptr)
				return;

			GameObjectTag tag = pGameObj->GetTag();
			if (tag == GameObjectTag::PLAYER)
			{
				OnClear();
			}
		}
	);

	Matrix4x4 mat;
	pTransform_->GenerateWorldMatrix(&mat);
	EffectParameters params;
	params.isLoop	= true;
	params.worldMat = mat;
	pEffect_		= Game::System<EffectManager>().Play("Treasure", params);
}

void Goal::Draw() const {}

void Goal::ShowImGui()
{
	MTImGui::ShowComponents(Entity::entityId_);
	ImGui::Text("EntityId:%lld", Entity::entityId_);
}

void Goal::OnClear()
{
	Game::System<Audio>().Play("GetTreasure");

	Timer::AddAram(
		transitionSceneDelay_,
		[]
		{
			Game::System<StageManager>().ClearCurrentStage();
			Game::System<SceneSystem>().Move<ResultScene>();
		}
	);

	Game::System<EventManager>().GetEvent<PlayerReachedGoalEvent>().Invoke(PlayerReachedGoalEvent {});
}
