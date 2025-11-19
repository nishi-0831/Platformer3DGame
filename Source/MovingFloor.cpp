#include "stdafx.h"
#include "MovingFloor.h"

namespace
{
	mtgb::Vector3 INIT_OFFSET{ 1.0f,0.0f,0.0f };
}
MovingFloor::MovingFloor()
{
}

MovingFloor::MovingFloor(EntityId _entityId)
	: StatefulComponent(_entityId)
	, pTransform_{nullptr}
	, dir_{1.0f}
	, elapsed_{0.0f}
{
	duration = 1.0f;
	pTransform_ = &Transform::Get(_entityId);
	pRigidBody_ = &RigidBody::Get(_entityId);
	pRigidBody_->OnCollisionEnter([this](EntityId _id)
		{
			OnCollisionEnter(_id);
		});

	GameObject* to = new GameObject();
	GameObject* from = new GameObject();

	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(to);
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(from);

	pToTransform_ = to->Component<Transform>();
	pFromTransform_ = from->Component<Transform>();
	pToTransform_->position = pTransform_->position - INIT_OFFSET;
	pFromTransform_->position = pTransform_->position + INIT_OFFSET;

	Collider* pToCollider = to->Component<Collider>();
	Collider* pFromCollider = from->Component<Collider>();
	pToCollider->colliderType = ColliderType::TYPE_SPHERE;
	pFromCollider->colliderType = ColliderType::TYPE_SPHERE;
	pToCollider->SetRadius(1.0f);
	pFromCollider->SetRadius(1.0f);	
}

void MovingFloor::Update()
{
	to = pToTransform_->position;
	from = pFromTransform_->position;

	elapsed_ += Time::DeltaTimeF() * dir_;
	float progress = elapsed_ / duration;
	pTransform_->position = Mathf::Lerp(pToTransform_->position, pFromTransform_->position, progress);

	if (progress > 1.0f || progress < 0.0f)
	{
		dir_ *= -1.0f;
		elapsed_ = std::clamp(elapsed_, 0.0f, 1.0f);
	}
	/*MTImGui::Instance().DirectShow([]() 
		{
			TypeRegistry::Instance().CallFunc<Collider>(, "to");
		}, "MovingFloor", ShowType::Inspector);*/
}

void MovingFloor::OnPostRestore()
{
	pToTransform_->position = to;
	pFromTransform_->position = from;
}

void MovingFloor::OnCollisionEnter(EntityId _entityId)
{
	Transform& otherTransform = Transform::Get(_entityId);

	if (pTransform_->position.y > otherTransform.position.y)
		return;

	Matrix4x4 parentInverse = DirectX::XMMatrixInverse(nullptr, pTransform_->matrixWorld_);
	Vector3 localPosition = otherTransform.position * parentInverse;

	otherTransform.parent = GetEntityId();

	otherTransform.position = localPosition;
	otherTransform.Compute();
}


