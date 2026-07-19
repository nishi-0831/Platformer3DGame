#include "stdafx.h"
#include "Player.h"
#include "QuaternionCamera.h"
#include "ActorManager.h"
#include "ResultScene.h"
#include "GameEvents.h"
#include <algorithm>
namespace
{
	float speed		 = 5.0f;
	float jumpHeight = 5.0f;
} // namespace

Player::Player()
	: GameObject(GameObjectBuilder()
					 .SetName(Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(Player)))
					 .SetPosition({ 0, 5, 10 })
					 .SetTag(GameObjectTag::PLAYER)
					 .Build())
	, ImGuiShowable(ShowType::INSPECTOR, GetEntityId())
	, IActor(GetEntityId())
	, pTransform_ { Component<Transform>() }
	, pCollider_ { Component<Collider>() }
	, pMeshRenderer_ { Component<MeshRenderer>() }
	, pRigidBody_ { Component<RigidBody>() } //, pCamera_{Instantiate<Camera>(this)}
	, pCamera_ { Instantiate<QuaternionCamera>(GetEntityId()) }
	, pCameraTransform_ { &Transform::Get(pCamera_->GetEntityId()) }
	, hp_ { 3 }
	, pHPViewer_ { nullptr }
	, isInvincible_ { false }
	, invincibilityTimeSec_ { 2.0f }
	, changeVisibilitySpan_ { 0.3f }
	, elapsedInvincibilityTime_ { 0.0f }
	, jumpController_ { GetEntityId() }
	, walkSmokeInterval_ { 0.3f }
	, walkSmokeElapsedTime_ { 0.0f }
{
	// pRigidBody_->useGravity_ = true;
	pRigidBody_->isKinematic_ = false;
	pRigidBody_->OnCollisionEnter(
		[this](EntityId _entityId)
		{
			OnCollisionEnter(_entityId);
		}
	);
	pMeshRenderer_->meshFileName = "Model/MinerAnim.fbx";
	pMeshRenderer_->meshHandle	 = Fbx::Load(pMeshRenderer_->meshFileName);

	pCollider_->colliderType_ = ColliderType::TYPE_SPHERE;
	pCollider_->SetRadius(pTransform_->scale.x);

	displayName_ = name_;

	CameraHandleInScene hCamera = Game::System<SceneSystem>().GetActiveScene()->RegisterCameraGameObject(pCamera_);

	WinCtxRes::Get<CameraResource>(WindowContext::FIRST).SetHCamera(hCamera);

	// ゴールイベントを購読
	Game::System<EventManager>().GetEvent<PlayerReachedGoalEvent>().Subscribe(
		[this](const PlayerReachedGoalEvent& _event)
		{
			pRigidBody_->velocity_ = Vector3::Zero();
			state_.Change(STATE::VICTORY);
		},
		EventScope::SCENE
	);
}

Player::~Player() {}

void Player::Update()
{
	Game::System<ShadowSettings>().SetCaster(GetEntityId());
	Game::System<Audio>().SetListenerEntityId(GetEntityId());

	if (state_.Current() != STATE::DYING && state_.Current() != STATE::VICTORY)
	{
		UpdatePosition();
		if (InputUtil::GetGamePadDown(PadCode::CROSS) || InputUtil::GetKeyDown(KeyCode::SPACE))
		{
			if (pRigidBody_->isGround_)
			{
				jumpController_.StartJump(jumpHeight);
				Game::System<Audio>().Play("Jump");

				Matrix4x4 worldMat;
				pTransform_->GenerateWorldMatrix(&worldMat);
				EffectParameters params;
				params.isLoop	= false;
				params.worldMat = worldMat;
				Game::System<EffectManager>().Play("JumpSmoke", params);
			}
		}
		if (InputUtil::GetGamePadUp(PadCode::CROSS) || InputUtil::GetKeyUp(KeyCode::SPACE))
		{
			if (pRigidBody_->IsJumping())
			{
				jumpController_.ReleaseButton();
			}
		}
		UpdateRotate();
	}

	state_.Update();

	jumpController_.Update();

	// ダメージを受けた後の無敵時間
	if (isInvincible_)
	{
		elapsedInvincibilityTime_ += Time::DeltaTimeF();
		if (elapsedInvincibilityTime_ >= invincibilityTimeSec_)
		{
			isInvincible_			  = false;
			pMeshRenderer_->enabled_  = true;
			elapsedInvincibilityTime_ = 0.0f;
			Timer::Remove(hTimerChangeVisibility_);
		}
	}
}

void Player::InitializeState()
{
	animController_ = Fbx::GetAnimationController(pMeshRenderer_->meshHandle);
	massert(animController_.has_value() && "Playerのアニメーションコントローラ取得に失敗");

	state_
		.OnAnyUpdate(
			[this]
			{
				if (animController_.has_value())
				{
					animController_->UpdateFrame();
					pMeshRenderer_->SetFrame(animController_->GetCurrentFrame());
				}
			}
		)
		.OnStart(
			STATE::IDLE,
			[this]
			{
				animController_->PlayAnimation("Idle", true);
			}
		)
		.OnUpdate(
			STATE::IDLE,
			[this]
			{
				if (pRigidBody_->velocity_.y > 0.0f)
				{
					state_.Change(STATE::JUMP);
					return;
				}
				if (GetMoveDir().Size() != 0)
				{
					state_.Change(STATE::RUN);
					return;
				}
			}
		)
		.OnStart(
			STATE::RUN,
			[this]
			{
				animController_->PlayAnimation("Run", true);
				walkSmokeElapsedTime_ = 0.0f;
			}
		)
		.OnUpdate(
			STATE::RUN,
			[this]
			{
				if (pRigidBody_->velocity_.Size() == 0.0f)
				{
					state_.Change(STATE::IDLE);
					return;
				}
				if (pRigidBody_->velocity_.y > 0.0f)
				{
					state_.Change(STATE::JUMP);
					return;
				}
				if (pRigidBody_->velocity_.y < 0.0f)
				{
					state_.Change(STATE::FALL);
					return;
				}

				walkSmokeElapsedTime_ += Time::DeltaTimeF();
				if (walkSmokeElapsedTime_ >= walkSmokeInterval_)
				{
					EffectParameters params;
					params.isLoop = false;
					Matrix4x4 worldMat;
					pTransform_->GenerateWorldMatrix(&worldMat);
					params.worldMat = worldMat;
					Game::System<EffectManager>().Play("WalkSmoke", params);

					walkSmokeElapsedTime_ = 0.0f;
				}
			}
		)
		.OnStart(
			STATE::JUMP,
			[this]
			{
				animController_->PlayAnimation("Jump", false);
			}
		)
		.OnUpdate(
			STATE::JUMP,
			[this]
			{
				if (animController_->IsFinishedAnimation() && pRigidBody_->isGround_ == false)
				{
					state_.Change(STATE::FALL);
					return;
				}
				if (pRigidBody_->isGround_)
				{
					state_.Change(STATE::IDLE);
					return;
				}
			}
		)
		.OnStart(
			STATE::FALL,
			[this]
			{
				animController_->PlayAnimation("Fall", true);
			}
		)
		.OnUpdate(
			STATE::FALL,
			[this]
			{
				if (pRigidBody_->isGround_)
				{
					state_.Change(STATE::IDLE);
					return;
				}
			}
		)
		.OnStart(
			STATE::DYING,
			[this]
			{
				animController_->PlayAnimation("Dying", false);
			}
		)
		.OnStart(
			STATE::VICTORY,
			[this]
			{
				animController_->PlayAnimation("Dancing", true);
			}
		);
}

void Player::Draw() const {}

void Player::Start()
{
	InitializeState();
	state_.Change(STATE::IDLE);
	pHPViewer_ = Instantiate<HPViewer>(hp_);

	animController_.value().SetEventCallback(
		"Footstep",
		[this](const AnimationEvent& _evt)
		{
			OnFootstep(_evt);
		}
	);
}

void Player::ShowImGui()
{
	MTImGui::ShowComponents(Entity::entityId_);
	ImGui::Checkbox("isGrounded", &pRigidBody_->isGround_);
	PropertyDisplayRegistry::Instance().ShowProperty(&pRigidBody_->velocity_, "vel");
}

Vector3 Player::GetMoveDir()
{
	Vector2F axis = InputUtil::GetAxis(StickType::LEFT);
	if (InputUtil::GetKey(KeyCode::LEFT) || InputUtil::GetKey(KeyCode::A))
	{
		axis.x = -1;
	}
	if (InputUtil::GetKey(KeyCode::RIGHT) || InputUtil::GetKey(KeyCode::D))
	{
		axis.x = 1;
	}
	if (InputUtil::GetKey(KeyCode::UP) || InputUtil::GetKey(KeyCode::W))
	{
		axis.y = -1;
	}
	if (InputUtil::GetKey(KeyCode::DOWN) || InputUtil::GetKey(KeyCode::S))
	{
		axis.y = 1;
	}
	if (axis.Size() == 0)
		return Vector3::Zero();

	// 入力方向
	Vector3 inputDir { axis.x, 0.0f, -axis.y };

	// カメラの回転行列を取得
	Matrix4x4 cameraRotMat;
	pCameraTransform_->GenerateWorldRotationMatrix(&cameraRotMat);
	// 入力方向をカメラの向きだけ回転
	Vector3 dir = inputDir * cameraRotMat;
	// Y成分を捨てたXZ成分のみ取得
	Vector3 horizontalDir = Vector3 { dir.x, 0.0f, dir.z };
	return Vector3::Normalize(horizontalDir);
}

void Player::UpdatePosition()
{
	Vector3& velocity = pRigidBody_->velocity_;

	if (Vector3 moveDir = GetMoveDir(); moveDir.Size() != 0)
	{
		Vector3 movement = moveDir * speed;

		velocity.x = movement.x;
		velocity.z = movement.z;
	}
	else
	{
		// -------------------------------------------------------
		// WARNING:
		// 入力がない場合、XZの速度をゼロにしている!!!!
		// 入力以外で速度を変える場合は修正!!!!
		// -------------------------------------------------------
		velocity.x = 0.0f;
		velocity.z = 0.0f;
	}
}

void Player::UpdateRotate()
{
	if (Vector3 moveDir = GetMoveDir(); moveDir.Size() != 0)
	{
		pTransform_->rotate = Quaternion::LookRotation(moveDir, Vector3::Up());
	}
}

void Player::OnCollisionEnter(EntityId _entityId)
{
	GameObject* otherObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_entityId);
	if (!otherObj)
		return;

	IActor* pOtherActor = Game::System<ActorManager>().GetActor(_entityId);
	if (pOtherActor == nullptr)
		return;

	Transform& otherTransform = Transform::Get(_entityId);
	bool isStomping			  = (pTransform_->position.y > otherTransform.position.y);

	if (isStomping)
	{
		pOtherActor->OnStomped(this);
	}
	else
	{
		pOtherActor->OnHitSide(this);
	}
}

void Player::OnFootstep(const AnimationEvent& _event)
{
	Game::System<Audio>().Play("MinerFootstep");
}

void Player::OnStomped(IActor* _pOther) {}

void Player::OnHitSide(IActor* _pOther) {}

void Player::TakeDamage(int _damage)
{
	// 無敵ならダメージ処理は行わない
	if (isInvincible_)
		return;
	// 負の値は無視
	if (_damage <= 0)
		return;

	hp_ = (std::max)(0, hp_ - _damage);

	if (hp_ <= 0)
	{
		state_.Change(STATE::DYING);
		pRigidBody_->velocity_ = Vector3::Zero();

		// プレイヤーのHPが0になったことを通知
		PlayerHpReachedZeroEvent event { .playerEntityId = GetEntityId() };
		Game::System<EventManager>().GetEvent<PlayerHpReachedZeroEvent>().Invoke(event);
	}

	pHPViewer_->TakeDamage(_damage);

	isInvincible_			= true;
	hTimerChangeVisibility_ = Timer::AddInterval(
		changeVisibilitySpan_,
		[this]
		{
			pMeshRenderer_->enabled_ = !pMeshRenderer_->enabled_;
		},
		true // firstCall: 即座に処理を呼ぶ
	);
}
