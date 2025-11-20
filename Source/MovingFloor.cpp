#include "stdafx.h"
#include "MovingFloor.h"
#include "Debug.h"
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
	pCollider_ = &Collider::Get(_entityId);
	pCollider_->colliderType = ColliderType::TYPE_AABB;
	pCollider_->extents = { 1,1,1 };
	pRigidBody_->OnCollisionEnter([this](EntityId _id)
		{
			OnCollisionEnter(_id);
		});
	pRigidBody_->OnCollisionExit([this](EntityId _id)
		{
			//OnCollisionExit(_id);
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
	MTImGui::Instance().DirectShow([this]() 
		{
			for (auto collider : pCollider_->onColliders_)
			{
				ImGui::PushID(collider->GetEntityId());
				ImGui::Text("EntityId: %lld", collider->GetEntityId());
				ImGui::PopID();

			}
		}, "MovingFloor", ShowType::Inspector);
}

void MovingFloor::OnPostRestore()
{
	pToTransform_->position = to;
	pFromTransform_->position = from;
}

void MovingFloor::OnCollisionEnter(EntityId _entityId)
{
	GameObject* gameObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_entityId);
	GameObjectTag tag = gameObj->GetTag();
	
	if (tag != GameObjectTag::Player)
		return;

	Transform& otherTransform = Transform::Get(_entityId);

	if (pTransform_->position.y > otherTransform.position.y)
		return;
	
	groundedEntity_ = _entityId;

	
	otherTransform.SetParent(GetEntityId());

	otherTransform.Compute();
	LOGIMGUI("OnCollisionEnter");

}

void MovingFloor::OnCollisionExit(EntityId _entityId)
{
	GameObject* gameObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_entityId);
	GameObjectTag tag = gameObj->GetTag();

	if (tag != GameObjectTag::Player)
		return;

	if (groundedEntity_ != _entityId)
		return;

	Transform& childTransform = Transform::Get(groundedEntity_);
	Matrix4x4 localMat;
	childTransform.GenerateLocalMatrix(&localMat);

	Matrix4x4 parentMat;
	pTransform_->GenerateWorldMatrix(&parentMat);

	Matrix4x4 newMat = localMat * parentMat;
	childTransform.DecomposeMatrix(newMat);
	childTransform.SetParent(INVALID_ENTITY);

	childTransform.Compute();

	groundedEntity_ = INVALID_ENTITY;

	LOGIMGUI("OnCollisionExit");
}


