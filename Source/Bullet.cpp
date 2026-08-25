#include "Bullet.h"

Bullet::Bullet(EntityId _gunnerId)
	: GameObject()
	, IActor(GetEntityId())
	, pRigidBody_ { Component<RigidBody>() }
	, pMeshRenderer_ { Component<MeshRenderer>() }
	, gunnerId_ { _gunnerId }
	, takeDamageAmount_ { 1 }
	, despawnTime_ { 3.0f }
{
	pMeshRenderer_->meshFileName = "Model/SpikeBall.fbx";
	pMeshRenderer_->SetMesh(Fbx::Load(pMeshRenderer_->meshFileName));
	pMeshRenderer_->shaderType = ShaderType::FBX_PARTS;

	// 指定した秒数後に破棄
	despawnTimerHandle_ = Timer::AddAram(
		despawnTime_,
		[this]()
		{
			DestroyMe();
		}
	);
}

Bullet::~Bullet() {}

void Bullet::OnStomped(IActor* _pOther)
{
	OnHit(_pOther);
}

void Bullet::OnHitSide(IActor* _pOther)
{
	OnHit(_pOther);
}

void Bullet::TakeDamage(int _damage) {}

void Bullet::OnHit(IActor* _pOther)
{
	if (_pOther == nullptr)
		return;

	// 自分を射出したガンナーにはダメージを与えない
	if (_pOther->GetId() == gunnerId_)
		return;

	// 接触したアクターにダメージを与えて、自分を破棄
	_pOther->TakeDamage(takeDamageAmount_);
	DestroyMe();
	Timer::Remove(despawnTimerHandle_);
}
