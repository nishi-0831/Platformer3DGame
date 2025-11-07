#include "stdafx.h"
#include "Player.h"

namespace 
{
	float speed = 1.0f;
	float jumpHeight = 30.0f;
}

Player::Player()
	: GameObject()
	, ImGuiShowable(ShowType::Inspector,Entity::entityId_)
{
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
	Vector2F axis = InputUtil::GetAxis();

	float x = axis.x;
	float y = axis.y;

	Vector3& velocity = pRigidBody_->velocity;

	velocity.x = x * speed;
	velocity.z = -y * speed;

	if (InputUtil::GetKeyDown(KeyCode::Space))
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
