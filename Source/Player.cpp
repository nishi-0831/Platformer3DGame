#include "stdafx.h"
#include "Player.h"
#include "QuaternionCamera.h"
#include "ActorManager.h"
#include "ResultScene.h"
#include "GameEvents.h"
#include <algorithm>
#include <cmath>
Player::Player()
	: GameObject(GameObjectBuilder()
					 .SetName(Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(Player)))
					 .SetPosition({ 0, 1, 0 })
					 .SetTag(GameObjectTag::PLAYER)
					 .Build())
	, IActor(GetEntityId())
	, pTransform_ { Component<Transform>() }
	, pCollider_ { Component<Collider>() }
	, pMeshRenderer_ { Component<MeshRenderer>() }
	, pRigidBody_ { Component<RigidBody>() }
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
	, walkJumpHeight_ { 5.0f }
	, runJumpHeight_ { 7.5f }
	, walkSpeed_ { 5.0f }
	, dashSpeed_ { 10.0f }
	, acceleration_ { 50.0f }
	, isRunning_ { false }
	, externalDeceleration_ { 5.0f }
	, movementVelocity_ { Vector3::Zero() }
	, externalVelocity_ { Vector3::Zero() }
	, friction_ { 30.0f }
	, isDashJumping_ { false }
{
	pRigidBody_->isKinematic_ = false;
	pRigidBody_->OnCollisionEnter(
		[this](EntityId _entityId)
		{
			OnCollisionEnter(_entityId);
		}
	);
	pMeshRenderer_->meshFileName = "Model/MinerAnim.fbx";
	pMeshRenderer_->meshHandle	 = Fbx::Load(pMeshRenderer_->meshFileName);
	pMeshRenderer_->shaderType	 = ShaderType::FBX_PARTS_SKIN;
	pCollider_->colliderType_	 = ColliderType::TYPE_SPHERE;
	pCollider_->SetRadius(pTransform_->scale.x);
	pCollider_->SetCenter(Vector3(0.0f, 0.8f, 0.0f));

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
	// 丸影を落とす位置を指定する
	Game::System<ShadowSettings>().AddCaster(GetEntityId());
	// オーディオリスナーの位置を指定する
	Game::System<Audio>().SetListenerEntityId(GetEntityId());
	isRunning_ = (InputQuery::GetGamePad(PadButton::L_STICK) || InputQuery::GetKey(KeyCode::LEFT_SHIFT)) &&
				 pRigidBody_->isGround_;
	if (pRigidBody_->isGround_)
	{
		isDashJumping_ = false;
	}
	// 力尽きた状態、勝利状態でない場合
	if (state_.Current() != STATE::DYING && state_.Current() != STATE::VICTORY)
	{
		bool jumpBtnPressed = InputQuery::GetGamePadDown(PadButton::SOUTH) || InputQuery::GetKeyDown(KeyCode::SPACE);
		// ジャンプ処理の更新
		jumpController_.Update(jumpBtnPressed);
		bool isDashJump = jumpController_.CanJump() && state_.Current() == STATE::RUN;
		// ジャンプボタン押下処理
		if (jumpController_.CanJump())
		{
			jumpController_.StartJump(isDashJump ? runJumpHeight_ : walkJumpHeight_);

			if (isDashJump)
			{
				externalVelocity_ += pTransform_->Forward() * (dashSpeed_ - walkSpeed_);
				isDashJumping_ = true;
			}

			// ジャンプ時のSE
			Game::System<Audio>().Play("Jump");

			// ジャンプ時の煙エフェクト
			Matrix4x4 worldMat;
			pTransform_->GenerateWorldMatrix(&worldMat);
			EffectParameters params;
			params.isLoop	= false;
			params.worldMat = worldMat;
			Game::System<EffectManager>().Play("JumpSmoke", params);
		}
		// ジャンプボタンを離した処理
		if (InputQuery::GetGamePadUp(PadButton::SOUTH) || InputQuery::GetKeyUp(KeyCode::SPACE))
		{
			if (pRigidBody_->IsJumping())
			{
				jumpController_.ReleaseButton();
			}
		}

		UpdateVelocity();
		// 姿勢更新
		UpdateRotate();

		externalVelocity_ = MoveTowards(externalVelocity_, Vector3::Zero(), externalDeceleration_ * Time::DeltaTimeF());
		if (pRigidBody_->isGround_)
		{
			externalVelocity_	= MoveTowards(externalVelocity_, Vector3::Zero(), friction_ * Time::DeltaTimeF());
			externalVelocity_.y = 0.0f;
		}
		Vector3 finalVelocity = movementVelocity_ + externalVelocity_ + surfaceVelocity_;
		finalVelocity.y += jumpController_.GetVelocityY();
		pRigidBody_->velocity_ = finalVelocity;
	}
	// アニメーションのステート更新
	state_.Update();

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
		// IDLE状態の処理
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
				// +Y方向に移動している場合、ジャンプ状態に遷移
				if (pRigidBody_->velocity_.y > 0.0f)
				{
					state_.Change(STATE::JUMP);
					return;
				}
				// 水平方向に移動している場合、走る状態に遷移
				if (GetMoveDir().Size() != 0)
				{
					if (isRunning_)
					{
						state_.Change(STATE::RUN);
					}
					else
					{
						state_.Change(STATE::WALK);
					}
					return;
				}
			}
		)
		// WALK状態の処理
		.OnStart(
			STATE::WALK,
			[this]
			{
				animController_->PlayAnimation("Walk", true);
				walkSmokeElapsedTime_ = 0.0f;
			}
		)
		.OnUpdate(
			STATE::WALK,
			[this]
			{
				STATE nextState;
				bool needsToTransition = TryGetNextStateOnMove(nextState);

				if (needsToTransition)
				{
					state_.Change(nextState);
					return;
				}
				if (isRunning_)
				{
					state_.Change(STATE::RUN);
				}

				// 歩いているときの煙エフェクトを発生させる
				walkSmokeElapsedTime_ += Time::DeltaTimeF();
				if (walkSmokeElapsedTime_ >= walkSmokeInterval_)
				{
					EffectParameters params;
					// ループなし
					params.isLoop = false;
					// エフェクトの発生位置をプレイヤーの座標に設定
					Matrix4x4 worldMat;
					pTransform_->GenerateWorldMatrix(&worldMat);
					params.worldMat = worldMat;
					Game::System<EffectManager>().Play("WalkSmoke", params);

					walkSmokeElapsedTime_ = 0.0f;
				}
			}
		)
		// RUN状態の処理
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
				STATE nextState;
				bool needsToTransition = TryGetNextStateOnMove(nextState);

				if (needsToTransition)
				{
					state_.Change(nextState);
					return;
				}
				if (isRunning_ == false)
				{
					state_.Change(STATE::WALK);
				}

				// 走っているときの煙エフェクトを発生させる
				walkSmokeElapsedTime_ += Time::DeltaTimeF();
				if (walkSmokeElapsedTime_ >= walkSmokeInterval_)
				{
					EffectParameters params;
					// ループなし
					params.isLoop = false;
					// エフェクトの発生位置をプレイヤーの座標に設定
					Matrix4x4 worldMat;
					pTransform_->GenerateWorldMatrix(&worldMat);
					params.worldMat = worldMat;
					Game::System<EffectManager>().Play("WalkSmoke", params);

					walkSmokeElapsedTime_ = 0.0f;
				}
			}
		)
		// JUMP状態の処理
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
				// ジャンプアニメーションが終了したら落下状態に遷移
				if (animController_->IsFinishedAnimation() && pRigidBody_->isGround_ == false)
				{
					state_.Change(STATE::FALL);
					return;
				}
				// 設置しているならIDLEに遷移
				if (pRigidBody_->isGround_)
				{
					state_.Change(STATE::IDLE);
					return;
				}
			}
		)
		// FALL状態の処理
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
				// 接地しているならIDLEに遷移
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

bool Player::TryGetNextStateOnMove(STATE& _state)
{
	// 落下状態に遷移
	if (jumpController_.IsFalling())
	{
		_state = STATE::FALL;
		return true;
	}
	// +Y方向に移動している場合、ジャンプ状態に遷移
	if (pRigidBody_->velocity_.y > 0.0f)
	{
		_state = STATE::JUMP;
		return true;
	}
	// 停止しているならIDLEに遷移
	if (movementVelocity_.x == 0.0f && movementVelocity_.z == 0.0f)
	{
		_state = STATE::IDLE;
		return true;
	}
	return false;
}

void Player::Start()
{
	// ステート更新処理の初期化
	InitializeState();
	state_.Change(STATE::IDLE);
	pHPViewer_ = Instantiate<HPViewer>(hp_);

	animController_->SetEventCallback(
		"Footstep", // イベント名
		[this](const AnimationEvent& _evt)
		{
			Game::System<Audio>().Play("MinerFootstep");
		}
	);
}

void Player::ShowImGui()
{
	GameObject::ShowImGui();
	ImGui::Checkbox("isGrounded", &pRigidBody_->isGround_);
	PropertyDisplayRegistry::Instance().ShowProperty(&pRigidBody_->velocity_, "vel");
	float jumpBufferRemainTime = jumpController_.GetJumpBufferRemainTime();
	PropertyDisplayRegistry::Instance().ShowProperty(&jumpBufferRemainTime, "jumpBufferRemainTime");
}

Vector3 Player::GetMoveDir()
{
	// 左スティックの入力を取得
	Vector2F axis = InputQuery::GetAxis(StickType::LEFT);

	// キーボード入力も取得
	if (InputQuery::GetKey(KeyCode::LEFT) || InputQuery::GetKey(KeyCode::A))
	{
		axis.x = -1;
	}
	if (InputQuery::GetKey(KeyCode::RIGHT) || InputQuery::GetKey(KeyCode::D))
	{
		axis.x = 1;
	}
	if (InputQuery::GetKey(KeyCode::UP) || InputQuery::GetKey(KeyCode::W))
	{
		axis.y = -1;
	}
	if (InputQuery::GetKey(KeyCode::DOWN) || InputQuery::GetKey(KeyCode::S))
	{
		axis.y = 1;
	}
	// 入力がない場合は、移動しない
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

Vector3 Player::MoveTowards(const Vector3& _curr, const Vector3& _target, float _maxDelta)
{
	// xyz、すべてにMoveTowardsを適用
	return Vector3(
		MoveTowards(_curr.x, _target.x, _maxDelta),
		MoveTowards(_curr.y, _target.y, _maxDelta),
		MoveTowards(_curr.z, _target.z, _maxDelta)
	);
}

float Player::MoveTowards(float _curr, float _target, float _maxDelta)
{
	// 現在と目的の値の差が、移動量より少ないなら
	if (std::abs(_target - _curr) <= _maxDelta)
	{
		// 目的値をそのまま返す
		return _target;
	}
	// 目的の値へ近づける。現在の値の方が小さいなら正方向、大きいなら負方向
	return _curr + std::copysign(_maxDelta, _target - _curr);
}

void Player::UpdateVelocity()
{
	Vector3 moveDir	  = GetMoveDir();
	float targetSpeed = 0.0f;
	// 走っているか否かで速度を変更
	if (isRunning_)
	{
		targetSpeed = dashSpeed_;
	}
	else
	{
		targetSpeed = walkSpeed_;
	}
	// 移動量
	Vector3 movement = moveDir * targetSpeed;
	// 移動速度を緩やかに変更
	// x成分を変更
	movementVelocity_.x = MoveTowards(movementVelocity_.x, movement.x, acceleration_ * Time::DeltaTimeF());
	// z成分を変更
	movementVelocity_.z = MoveTowards(movementVelocity_.z, movement.z, acceleration_ * Time::DeltaTimeF());
	// y成分は0のまま
	movementVelocity_.y = 0.0f;
}

void Player::UpdateRotate()
{
	// 移動方向に向かせる。ダッシュジャンプ中は向きを変えない
	if (Vector3 moveDir = GetMoveDir(); moveDir.Size() != 0 && isDashJumping_ == false)
	{
		pTransform_->rotate = Quaternion::LookRotation(moveDir, Vector3::Up());
	}
}

void Player::OnCollisionEnter(EntityId _entityId)
{
	// _entityIdに該当するゲームオブジェクトが存在するか確認
	GameObject* otherObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_entityId);
	if (!otherObj)
		return;

	// _entityIdに該当するアクターを取得
	IActor* pOtherActor = Game::System<ActorManager>().GetActor(_entityId);
	if (pOtherActor == nullptr)
		return;

	// 衝突したエンティティのトランスフォーム取得
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

	// 一定時間無敵にする
	isInvincible_ = true;
	// プレイヤーを点滅させる
	hTimerChangeVisibility_ = Timer::AddInterval(
		changeVisibilitySpan_,
		[this]
		{
			if (isInvincible_)
			{
				pMeshRenderer_->enabled_ = !pMeshRenderer_->enabled_;
			}
			else
			{
				pMeshRenderer_->enabled_ = true;
			}
		},
		true // firstCall: 即座に処理を呼ぶ
	);
}

void Player::AddExternalVelocity(const Vector3& _velocity)
{
	externalVelocity_ += _velocity;
}

void Player::SetSurfaceVelocity(const Vector3& _velocity)
{
	surfaceVelocity_ = _velocity;
}
