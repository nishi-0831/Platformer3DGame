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
	, pTransform_{ &Transform::Get(_entityId) }
	, pRigidBody_{ &RigidBody::Get(_entityId) }
	, pCollider_ { &Collider::Get(_entityId) }
	, dir_{1.0f}
	, elapsed_{0.0f}
{
	duration_ = 1.0f;

	// コライダーの設定
	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	pCollider_->extents_ = { 1,1,1 };

	// RigidBodyの設定
	pRigidBody_->OnCollisionEnter([this](EntityId _id)
		{
			OnCollisionEnter(_id);
		});
	pRigidBody_->OnCollisionExit([this](EntityId _id)
		{
			OnCollisionExit(_id);
		});
	
	// 始点、終点の作成

	// ゲームオブジェクト作成
	GameObject* to = new GameObject();
	GameObject* from = new GameObject();

	// ゲームオブジェクトをシーンに登録
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(to);
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(from);

	// Transform作成
	pToTransform_ = to->Component<Transform>();
	pFromTransform_ = from->Component<Transform>();
	// オフセット分動かす
	pToTransform_->position = pTransform_->position - INIT_OFFSET;
	pFromTransform_->position = pTransform_->position + INIT_OFFSET;

	// コライダー作成、設定
	Collider* pToCollider = to->Component<Collider>();
	Collider* pFromCollider = from->Component<Collider>();
	pToCollider->colliderType_ = ColliderType::TYPE_SPHERE;
	pFromCollider->colliderType_ = ColliderType::TYPE_SPHERE;
	pToCollider->SetRadius(1.0f);
	pFromCollider->SetRadius(1.0f);	
}

void MovingFloor::Update()
{
	UpdateProgress();
	pTransform_->position = Evaluate();

	// ImGuiにプロパティを表示
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

void MovingFloor::UpdateProgress()
{
	elapsed_ += Time::DeltaTimeF() * dir_;
	float progress = elapsed_ / duration_;
	if (progress > 1.0f || progress < 0.0f)
	{
		dir_ *= -1.0f;
		elapsed_ = std::clamp(elapsed_, 0.0f, 1.0f);
	}
}

Vector3 MovingFloor::Evaluate()
{
	float progress = elapsed_ / duration_;
	return Mathf::Lerp(pToTransform_->position, pFromTransform_->position, progress);
}

void MovingFloor::OnPostRestore()
{
	// 読み込んだ値を始点、終点の座標に代入
	pToTransform_->position = to_;
	pFromTransform_->position = from_;
}

void MovingFloor::OnPreSave()
{
	// 保存用の変数に始点、終点の座標を代入
	to_ = pToTransform_->position;
	from_ = pFromTransform_->position;
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


