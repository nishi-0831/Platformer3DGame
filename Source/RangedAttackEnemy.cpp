#include "stdafx.h"
#include "RangedAttackEnemy.h"

unsigned int RangedAttackEnemy::generateCounter_ { 0 };

RangedAttackEnemy::RangedAttackEnemy()
	: IActor(GetEntityId())
	, ImGuiShowable(GetEntityId())
	, pTransform_ { Component<Transform>() }
	, pRigidBody_ { Component<RigidBody>() }
	, pMeshRenderer_ { Component<MeshRenderer>() }
	, pCollider_ { Component<Collider>() }
{
	pMeshRenderer_->meshFileName = "Model/Beholder2.fbx";
	pMeshRenderer_->meshHandle	 = Fbx::Load(pMeshRenderer_->meshFileName);
	pMeshRenderer_->layer		 = AllLayer();
	pMeshRenderer_->shaderType	 = ShaderType::FBX_PARTS_SKIN;

	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	pCollider_->SetExtents({ 1.0f, 1.0f, 1.0f });
	// 型情報に登録された名前を取得
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(RangedAttackEnemy));
	// name_				 = std::format("{} ({})", typeName, generateCounter_++);
	displayName_ = name_;

	InitializeState();
}

RangedAttackEnemy::~RangedAttackEnemy() {}

void RangedAttackEnemy::Update()
{
	if (animController_.has_value())
	{
		animController_->UpdateFrame();
		pMeshRenderer_->SetFrame(animController_->GetCurrentFrame());
	}
	state_.Update();
}

void RangedAttackEnemy::Draw() const {}

void RangedAttackEnemy::Start() {}

void RangedAttackEnemy::ShowImGui() {}

void RangedAttackEnemy::OnStomped(IActor* _pOther) {}

void RangedAttackEnemy::OnHitSide(IActor* _pOther) {}

void RangedAttackEnemy::TakeDamage(int _damage) {}

void RangedAttackEnemy::InitializeState()
{
	// アニメーションのコントローラを取得
	animController_ = Fbx::GetAnimationController(pMeshRenderer_->meshHandle);

	state_.OnStart(
		STATE::WAIT,
		[this]
		{
			animController_->PlayAnimation("Idle", true);
		}
	);

	state_.OnStart(
		STATE::AIMING,
		[this]
		{
			animController_->PlayAnimation("Aiming", true);
		}
	);

	state_.OnStart(
		STATE::ATTACK,
		[this]
		{
			animController_->PlayAnimation("Attack", true);
		}
	);

	state_.OnStart(
		STATE::DYING,
		[this]
		{
			animController_->PlayAnimation("Die", true);
		}
	);

	state_.Change(STATE::DYING);
}
