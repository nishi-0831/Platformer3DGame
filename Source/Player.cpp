#include "stdafx.h"
#include "Player.h"

namespace 
{
	float speed = 5.0f;
	float jumpHeight = 30.0f;
}

Player::Player()
	: GameObject()
	, ImGuiShowable(ShowType::Inspector,Entity::entityId_)
{
	tag_ = GameObjectTag::Player;

	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(Player));
	name_ = typeName;
	displayName_ = typeName;
	pTransform_ = &(Transform::Get(Entity::entityId_));
	pTransform_->position = { 0,5,10 };
	pTransform_->scale = { 1,1,1 };

	pRigidBody_ = &(RigidBody::Get(Entity::entityId_));
	pRigidBody_->isKinematic = false;

	pMeshRenderer_ = &(MeshRenderer::Get(Entity::entityId_));
	pMeshRenderer_->meshFileName = "Model/Box.fbx";
	pMeshRenderer_->meshHandle = Fbx::Load("Model/Box.fbx");

	pCollider_ = &(Collider::Get(Entity::entityId_));
	pCollider_->SetRadius(pTransform_->scale.x);
	pCollider_->colliderType = ColliderType::TYPE_SPHERE;
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
		axis.y = 1;
	}
	if (InputUtil::GetKey(KeyCode::Down))
	{
		axis.y = -1;
	}

	Vector3& velocity = pRigidBody_->velocity;

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
			pRigidBody_->velocity.y += jumpHeight;
		}
	}
}

void Player::Draw() const
{
}

void Player::ShowImGui()
{
	MTImGui::Instance().ShowComponents(Entity::entityId_);
}

void Player::SetCamera(CameraHandleInScene _hCamera)
{
	pCameraTransform_ = &Game::System<CameraSystem>().GetTransform(_hCamera);
}
