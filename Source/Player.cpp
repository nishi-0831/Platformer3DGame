#include "stdafx.h"
#include "Player.h"
#include "Camera.h"
namespace 
{
	float speed = 5.0f;
	float jumpHeight = 15.0f;
}

Player::Player()
	: GameObject(GameObjectBuilder()
		.SetName(Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(Player)))
		.SetPosition({0,5,10})
		.SetTag(GameObjectTag::Player).Build())
	, ImGuiShowable(ShowType::Inspector,Entity::entityId_)
	, pTransform_{ Component<Transform>() }
	, pCollider_ { Component<Collider>() }
	
	, pMeshRenderer_{ Component<MeshRenderer>() }
	, pRigidBody_ { Component<RigidBody>() }
	, pCamera_{ Instantiate<Camera>(this) }
	, pCameraTransform_{ &Transform::Get(pCamera_->GetEntityId())}
{
	pRigidBody_->useGravity_ = true;
	pRigidBody_->isKinematic_ = false;
	pMeshRenderer_->meshFileName = "Model/Box.fbx";
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

	Vector3& velocity = pRigidBody_->velocity_;

	if (axis.Size() != 0)
	{
		// “ü—Í•ûŒü
		Vector3 inputDir{ axis.x,0.0f,-axis.y };

		// ƒJƒƒ‰‚Ì‰ñ“]s—ñ‚ðŽæ“¾
		Matrix4x4 cameraRotMat;
		pCameraTransform_->GenerateWorldRotationMatrix(&cameraRotMat);
		// “ü—Í•ûŒü‚ðƒJƒƒ‰‚ÌŒü‚«‚¾‚¯‰ñ“]
		Vector3 dir = inputDir * cameraRotMat;
		// Y¬•ª‚ðŽÌ‚Ä‚½XZ¬•ª‚Ì‚ÝŽæ“¾
		Vector3 horizontalDir = Vector3{ dir.x,0.0f,dir.z };

		Vector3 movement = Vector3::Normalize(horizontalDir) * speed;
		velocity.x = movement.x;
		velocity.z = movement.z;
	}
	else
	{
		// -------------------------------------------------------
		// * ’ˆÓ
		// “ü—Í‚ª‚È‚¢ê‡AXZ‚Ì‘¬“x‚ðƒ[ƒ‚É‚µ‚Ä‚¢‚é!!!!
		// “ü—ÍˆÈŠO‚Å‘¬“x‚ð•Ï‚¦‚éê‡‚ÍC³!!!!
		// -------------------------------------------------------
		velocity.x = 0.0f;
		velocity.z = 0.0f;
	}

	if (InputUtil::GetGamePadDown(PadCode::Cross) || InputUtil::GetKeyDown(KeyCode::Space))
	{
		if (pRigidBody_->IsJumping() == false)
		{
			pRigidBody_->velocity_.y += jumpHeight;
			//pRigidBody_->isGround = false;
		}
	}

	pCamera_->SetFollowMode(pRigidBody_->isGround_, pRigidBody_->velocity_);
}

void Player::Draw() const
{
}

void Player::Start()
{
	

	
}

void Player::ShowImGui()
{
	MTImGui::Instance().ShowComponents(Entity::entityId_);
}

void Player::SetCamera(Camera* _pCamera)
{
	
}


