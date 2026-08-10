#include "stdafx.h"
#include "RangedAttackEnemy.h"
#include "Bullet.h"
#include <format>
unsigned int RangedAttackEnemy::generateCounter_ { 0 };

RangedAttackEnemy::RangedAttackEnemy()
	: IActor(GetEntityId())
	, pTransform_ { Component<Transform>() }
	, pRigidBody_ { Component<RigidBody>() }
	, pMeshRenderer_ { Component<MeshRenderer>() }
	, pCollider_ { Component<Collider>() }
	, foundDistance_ { 10.0f }
	, attackTimer_ { 5.0f }
	, bulletSpeed_ { 3.0f }
	, onStompedBounce_ { 5.0f }
{
	tag_						 = GameObjectTag::ENEMY;
	pMeshRenderer_->meshFileName = "Model/Lich.fbx";
	pMeshRenderer_->meshHandle	 = Fbx::Load(pMeshRenderer_->meshFileName);
	pMeshRenderer_->layer		 = AllLayer();
	pMeshRenderer_->shaderType	 = ShaderType::FBX_PARTS_SKIN;
	pRigidBody_->isKinematic_	 = false;
	pCollider_->colliderType_	 = ColliderType::TYPE_AABB;
	pCollider_->SetExtents({ 1.0f, 1.0f, 1.0f });
	// 型情報に登録された名前を取得
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(RangedAttackEnemy));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);
	InitializeState();
	if (animController_.has_value())
	{
		// 弾丸を撃つイベント
		animController_->SetEventCallback(
			"Shot",
			[this](const AnimationEvent& _evt)
			{
				// 弾丸を発射
				Shot();

				// エフェクト描画
				Matrix4x4 mat;
				pTransform_->GenerateWorldMatrix(&mat);
				EffectParameters params;
				params.isLoop	= false;
				params.worldMat = mat;
				Game::System<EffectManager>().Play("MuzzleFlash", params);

				// SE再生
				Game::System<Audio>().Play("Shot");
			}
		);
	}
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

void RangedAttackEnemy::Start()
{
	// TagがPlayerのGameObjectを探す
	GameObject* targetGameObj = FindGameObject(GameObjectTag::PLAYER);
	if (targetGameObj == nullptr)
	{
		LOGIMGUI("Not Found Target");
		return;
	}

	pTargetTransform_ = &Transform::Get(targetGameObj->GetEntityId());
}

void RangedAttackEnemy::ShowImGui() {}

void RangedAttackEnemy::OnStomped(IActor* _pOther)
{
	if (state_.Current() != STATE::DYING)
	{
		// 踏まれたエフェクト再生
		EffectParameters params;
		params.isLoop	  = false;
		Vector3 effectPos = Vector3 { pTransform_->position.x,
									  pTransform_->position.y + pCollider_->GetExtents().y,
									  pTransform_->position.z };
		Matrix4x4 mat	  = DirectX::XMMatrixTranslation(effectPos.x, effectPos.y, effectPos.z);
		params.worldMat	  = mat;
		Game::System<EffectManager>().Play("Stomp", params);

		// 踏まれたSE再生
		Game::System<Audio>().Play("Stomp");

		// 踏んだアクターを上に飛ばす
		EntityId id				  = _pOther->GetId();
		RigidBody& otherRigidBody = RigidBody::Get(id);
		otherRigidBody.velocity_.y += onStompedBounce_;
	}

	state_.Change(STATE::DYING);
}

void RangedAttackEnemy::OnHitSide(IActor* _pOther) {}

void RangedAttackEnemy::TakeDamage(int _damage)
{
	state_.Change(STATE::DYING);
}

void RangedAttackEnemy::Wait()
{
	Vector3 toTarget = pTargetTransform_->GetWorldPosition() - pTransform_->GetWorldPosition();
	if (toTarget.Size() <= foundDistance_)
	{
		state_.Change(STATE::AIMING);
	}
}

void RangedAttackEnemy::Aiming()
{
	Vector3 targetWorldPos = pTargetTransform_->GetWorldPosition();
	Vector3 distPos		   = { targetWorldPos.x, pTransform_->position.y, targetWorldPos.z };
	Vector3 toTarget	   = distPos - pTransform_->GetWorldPosition();
	pTransform_->rotate	   = Quaternion::LookRotation(Vector3::Normalize(toTarget), Vector3::Up());

	// エフェクトの更新
	if (auto effect = pEnergyChargeEffect_.lock())
	{
		Matrix4x4 worldMat;
		pTransform_->GenerateWorldMatrix(&worldMat);
		effect->worldMat = worldMat;
	}

	// 一定距離より遠い場合、待機状態に戻る
	if (toTarget.Size() > foundDistance_)
	{
		state_.Change(STATE::WAIT);
		elapsedTime_ = 0.0f;
		DestroyEnergyChargeEffect();
	}

	// チャージ終了後、攻撃をする
	elapsedTime_ += Time::DeltaTimeF();
	if (elapsedTime_ >= attackTimer_)
	{
		state_.Change(STATE::ATTACK);
		elapsedTime_ = 0.0f;
		DestroyEnergyChargeEffect();
	}
}

void RangedAttackEnemy::Dying()
{
	if (animController_->IsFinishedAnimation())
	{
		DestroyMe();
	}
}

void RangedAttackEnemy::Shot()
{
	// 弾丸を生成
	GameObject* bullet = Instantiate<Bullet>(GetEntityId());

	Collider* pCollier		  = bullet->Component<Collider>();
	pCollider_->colliderType_ = ColliderType::TYPE_SPHERE;
	RigidBody* pRigidBody	  = bullet->Component<RigidBody>();

	// プレイヤーの方向に飛ばす
	Vector3 toTarget						 = pTargetTransform_->GetWorldPosition() - pTransform_->GetWorldPosition();
	pRigidBody->velocity_					 = Vector3::Normalize(toTarget) * bulletSpeed_;
	bullet->Component<Transform>()->position = pTransform_->GetWorldPosition();

	// チャージ時間を初期化して、チャージ状態に戻る
	elapsedTime_ = 0.0f;
	state_.Change(STATE::AIMING);
}

void RangedAttackEnemy::InitializeState()
{
	// アニメーションのコントローラを取得
	animController_ = Fbx::GetAnimationController(pMeshRenderer_->meshHandle);

	state_
		.OnStart(
			STATE::WAIT,
			[this]
			{
				animController_->PlayAnimation("Idle", true);
			}
		)
		.OnUpdate(
			STATE::WAIT,
			[this]
			{
				Wait();
			}
		);

	state_
		.OnStart(
			STATE::AIMING,
			[this]
			{
				animController_->PlayAnimation("Aiming", true);

				DestroyEnergyChargeEffect();

				Matrix4x4 mat;
				pTransform_->GenerateWorldMatrix(&mat);
				EffectParameters params;
				params.isLoop		 = true;
				params.worldMat		 = mat;
				pEnergyChargeEffect_ = Game::System<EffectManager>().Play("EnergyCharge", params);
			}
		)
		.OnUpdate(
			STATE::AIMING,
			[this]
			{
				Aiming();
			}
		);

	state_.OnStart(
		STATE::ATTACK,
		[this]
		{
			animController_->PlayAnimation("Attack", true);
		}
	);

	state_
		.OnStart(
			STATE::DYING,
			[this]
			{
				animController_->PlayAnimation("Dying", false);
				DestroyEnergyChargeEffect();
			}
		)
		.OnUpdate(
			STATE::DYING,
			[this]
			{
				Dying();
			}
		);

	state_.Change(STATE::WAIT);
}

void RangedAttackEnemy::DestroyEnergyChargeEffect()
{
	if (auto effect = pEnergyChargeEffect_.lock())
	{
		effect->destroyMe = true;
		effect->isLoop	  = true;
	}
}
