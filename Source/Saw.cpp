#include "Saw.h"

mtgb::Saw::Saw()
	: GameObject()
	, IActor(GetEntityId())
	, pTransform_ { Component<Transform>() }
	, pMeshRenderer_ { Component<MeshRenderer>() }
	, pCollider_ { Component<Collider>() }
	, pRigidBody_ { Component<RigidBody>() }
	, rotateAngleSec_ { 360.0f }
	, radius_ { 3.0f }
	, takeDamageAmount_ { 1 }
	, audioSourceHandle_ { -1 }
{
	pTransform_->scale			 = Vector3 { radius_, 1.0f, radius_ };
	pCollider_->colliderType_	 = ColliderType::TYPE_OBB;
	pMeshRenderer_->meshFileName = "Model/Saw.fbx";
	pMeshRenderer_->meshHandle	 = Fbx::Load("Model/Saw.fbx");
}

mtgb::Saw::~Saw() {}

void mtgb::Saw::Update()
{
	float angleRad		= DirectX::XMConvertToRadians(rotateAngleSec_ * Time::DeltaTimeF());
	Quaternion rot		= DirectX::XMQuaternionRotationAxis(Vector3::Up(), angleRad);
	pTransform_->rotate = rot * pTransform_->rotate;

	Game::System<Audio>().SetEmitter(GetEntityId(), "Saw", audioSourceHandle_);
}

void mtgb::Saw::OnPreDrawScene() const {}

void mtgb::Saw::Start()
{
	audioSourceHandle_ = Game::System<Audio>().Play("Saw", true);
}

void mtgb::Saw::ShowImGui()
{
	GameObject::ShowImGui();
	ImGui::InputFloat("RotateAngleSec", &rotateAngleSec_);
}

void mtgb::Saw::OnStomped(IActor* _pOther)
{
	_pOther->TakeDamage(takeDamageAmount_);
}

void mtgb::Saw::OnHitSide(IActor* _pOther)
{
	_pOther->TakeDamage(takeDamageAmount_);
}

void mtgb::Saw::TakeDamage(int _damage) {}
