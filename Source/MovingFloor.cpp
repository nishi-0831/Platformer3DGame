#include "stdafx.h"
#include "MovingFloor.h"
#include "Debug.h"
namespace
{
	mtgb::Vector3 INIT_OFFSET{ 1.0f,0.0f,0.0f };
}

MovingFloor::MovingFloor()
	: GameObject()
	, groundedEntity_{INVALID_ENTITY}
	, pTransform_{ &Transform::Get(entityId_) }
	, pMeshRenderer_{&MeshRenderer::Get(entityId_)}
	, pRigidBody_{ &RigidBody::Get(entityId_) }
	, pCollider_{ &Collider::Get(entityId_) }
	, pInterpolator_{ &Interpolator::Get(entityId_) }
{
	pMeshRenderer_->meshFileName = "Model/WallBox.fbx";
	pMeshRenderer_->meshHandle = Fbx::Load(pMeshRenderer_->meshFileName);
	pMeshRenderer_->layer = AllLayer();
	pMeshRenderer_->shaderType = ShaderType::FbxParts;
	// 型情報に登録された名前を取得
	name_ = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(MovingFloor));
	// コライダーの設定
	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	pCollider_->SetExtents(Vector3(1, 1, 1));

	// RigidBodyの設定
	pRigidBody_->OnCollisionEnter([this](EntityId _id)
		{
			OnCollisionEnter(_id);
		});
	pRigidBody_->OnCollisionExit([this](EntityId _id)
		{
			OnCollisionExit(_id);
		});
}

void MovingFloor::Update()
{
	pInterpolator_->UpdateProgress();
	pTransform_->position = pInterpolator_->Evaluate();
}

void MovingFloor::OnCollisionEnter(EntityId _entityId)
{
	GameObject* gameObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_entityId);
	GameObjectTag tag = gameObj->GetTag();
	
	// プレイヤーのみ対象にする
	// TODO: プレイヤー以外のアクターも対象にする
	if (tag != GameObjectTag::Player)
		return;

	// 自身に触れたTransform
	Transform& otherTransform = Transform::Get(_entityId);

	// 自身より上にいる場合、着地していると判定
	if (pTransform_->position.y > otherTransform.position.y)
		return;
	
	groundedEntity_ = _entityId;
	
	otherTransform.SetParent(GetEntityId());
}

void MovingFloor::OnCollisionExit(EntityId _entityId)
{
	GameObject* gameObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_entityId);
	GameObjectTag tag = gameObj->GetTag();

	// プレイヤーのみ対象にする
	// TODO: プレイヤー以外のアクターも対象にする
	if (tag != GameObjectTag::Player)
		return;

	// 現在着地しているEntityと異なるならスキップ
	if (groundedEntity_ != _entityId)
		return;

	// 自身に触れたTransform
	Transform& otherTransform = Transform::Get(_entityId);

	// INVALID_ENTITYを渡して親子関係解消
	otherTransform.SetParent(INVALID_ENTITY);
	groundedEntity_ = INVALID_ENTITY;
}


