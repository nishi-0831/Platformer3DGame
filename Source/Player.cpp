#include "stdafx.h"
#include "Player.h"
#include "Camera.h"
#include "ActorManager.h"
#include "ResultScene.h"
#include <algorithm>
namespace 
{
	float speed = 5.0f;
	float jumpHeight = 15.0f;
	const unsigned int TAKE_DAMGE_AMOUNT = 1;
	
}

Player::Player()
	: GameObject(GameObjectBuilder()
		.SetName(Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(Player)))
		.SetPosition({0,5,10})
		.SetTag(GameObjectTag::Player).Build())
	, ImGuiShowable(ShowType::Inspector,Entity::entityId_)
	, IActor(GetEntityId())
	, pTransform_{ Component<Transform>() }
	, pCollider_ { Component<Collider>() }
	, pMeshRenderer_{ Component<MeshRenderer>() }
	, pRigidBody_ { Component<RigidBody>() }
	, pCamera_{ Instantiate<Camera>(this) }
	, pCameraTransform_{ &Transform::Get(pCamera_->GetEntityId())}
	, hp_{3}
	, pHPViewer_{ nullptr }
{
	pRigidBody_->useGravity_ = true;
	pRigidBody_->isKinematic_ = false;
	pRigidBody_->OnCollisionEnter([this](EntityId _entityId) {OnCollisionEnter(_entityId); });
	pMeshRenderer_->meshFileName = "Model/MinerAnim.fbx";
	pMeshRenderer_->meshHandle = Fbx::Load(pMeshRenderer_->meshFileName);

	pCollider_->colliderType_ = ColliderType::TYPE_SPHERE ;
	pCollider_->SetRadius(pTransform_->scale.x);

	displayName_ = name_;

	CameraHandleInScene hCamera = Game::System<SceneSystem>().GetActiveScene()->RegisterCameraGameObject(pCamera_);

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera);
}

Player::~Player()
{
}

void Player::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::O))
	{
		EffectParameters params;
		params.isLoop = false;
		Matrix4x4 mat;
		pTransform_->GenerateWorldMatrix(&mat);
		params.worldMat = mat;
		Game::System<EffectManager>().Play("Stomp", params);
	}
	if (state_.Current() != STATE::DYING)
	{
		UpdatePosition();
		if (InputUtil::GetGamePadDown(PadCode::Cross) || InputUtil::GetKeyDown(KeyCode::Space))
		{
			if (pRigidBody_->IsJumping() == false)
			{
				pRigidBody_->velocity_.y += jumpHeight;
				state_.Change(STATE::JUMP);
			}
		}
		UpdateRotate();
	}


	pCamera_->SetFollowMode(pRigidBody_->isGround_, pRigidBody_->velocity_);

	state_.Update();
}

void Player::InitializeState()
{
	animController_ = Fbx::GetAnimationController(pMeshRenderer_->meshHandle);
	massert(animController_.has_value() && "Playerのアニメーションコントローラ取得に失敗");

	state_
		.OnAnyUpdate([this]
			{
				if (animController_.has_value())
				{
					animController_->UpdateFrame();
					pMeshRenderer_->SetFrame(animController_->GetCurrentFrame());
				}
			})
		.OnStart(STATE::IDLE, [this]
			{
				animController_->PlayAnimation("Idle", true);
			})
		.OnStart(STATE::RUN, [this]
			{
				animController_->PlayAnimation("Run", true);
			})
		.OnStart(STATE::JUMP, [this]
			{
				animController_->PlayAnimation("Jump", false);
			})
		.OnUpdate(STATE::JUMP, [this]
			{
				if (animController_->IsFinishedAnimation() && pRigidBody_->isGround_ == false)
				{
					state_.Change(STATE::FALL);
				}
			})
		.OnStart(STATE::FALL, [this]
			{
				animController_->PlayAnimation("Fall", true);
			})
		.OnStart(STATE::DYING, [this]
			{
				animController_->PlayAnimation("Dying", false);
			})
		.OnUpdate(STATE::DYING, [this]
			{
				if (animController_->IsFinishedAnimation())
				{
					Game::System<SceneSystem>().Move<ResultScene>();
				}
			});
}

void Player::Draw() const
{
}

void Player::Start()
{
	InitializeState();
	state_.Change(STATE::IDLE);
	pHPViewer_ = Instantiate<HPViewer>(hp_);
}

void Player::ShowImGui()
{
	MTImGui::Instance().ShowComponents(Entity::entityId_);
}

void Player::SetCamera(Camera* _pCamera)
{
	
}

Vector3 Player::GetMoveDir()
{
	Vector2F axis = InputUtil::GetAxis(StickType::LEFT);
	if (InputUtil::GetKey(KeyCode::Left))
	{
		axis.x = -1;
	}
	if (InputUtil::GetKey(KeyCode::Right))
	{
		axis.x = 1;
	}
	if (InputUtil::GetKey(KeyCode::Up))
	{
		axis.y = -1;
	}
	if (InputUtil::GetKey(KeyCode::Down))
	{
		axis.y = 1;
	}
	if (axis.Size() == 0)
		return Vector3::Zero();

	// 入力方向
	Vector3 inputDir{ axis.x,0.0f,-axis.y };

	// カメラの回転行列を取得
	Matrix4x4 cameraRotMat;
	pCameraTransform_->GenerateWorldRotationMatrix(&cameraRotMat);
	// 入力方向をカメラの向きだけ回転
	Vector3 dir = inputDir * cameraRotMat;
	// Y成分を捨てたXZ成分のみ取得
	Vector3 horizontalDir = Vector3{ dir.x,0.0f,dir.z };
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

		if (pRigidBody_->isGround_ && state_.Current() != STATE::RUN)
		{
			state_.Change(STATE::RUN);
		}
	}
	else
	{
		// -------------------------------------------------------
		// * 注意
		// 入力がない場合、XZの速度をゼロにしている!!!!
		// 入力以外で速度を変える場合は修正!!!!
		// -------------------------------------------------------
		velocity.x = 0.0f;
		velocity.z = 0.0f;
		if (pRigidBody_->isGround_ && state_.Current() != STATE::IDLE)
		{
			state_.Change(STATE::IDLE);
		}
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
	if (!otherObj) return;

	IActor* pOtherActor = Game::System<ActorManager>().GetActor(_entityId);
	if (pOtherActor == nullptr)
		return;

	Transform& otherTransform = Transform::Get(_entityId);
	bool isStomping = (pTransform_->position.y > otherTransform.position.y);

	if (isStomping)
	{
		pOtherActor->OnStomped(this);

		if (otherObj->GetTag() == GameObjectTag::Enemy)
		{
			Vector3& velocity = pRigidBody_->velocity_;
			velocity = Vector3(velocity.x, velocity.y + 10.0f, velocity.z);
		}
	}
	else
	{
		pOtherActor->OnHitSide(this);
	}
}



void Player::OnStomped(IActor* pOther)
{
}

void Player::OnHitSide(IActor* pOther)
{
}

void Player::TakeDamage(int _damage)
{
	// 負の値は無視
	if (_damage <= 0)
		return;

	hp_ = (std::max)(0, hp_ - _damage);

	if (hp_ <= 0)
	{
		state_.Change(STATE::DYING);
	}

	pHPViewer_->TakeDamage(_damage);
}


