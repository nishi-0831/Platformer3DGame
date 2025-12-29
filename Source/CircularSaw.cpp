#include "stdafx.h"
#include "CircularSaw.h"

mtgb::CircularSaw::CircularSaw()
	: GameObject()
	, pTransform_{ Component<Transform>() }
	, pMeshRenderer_{ Component<MeshRenderer>() }
	, pCollider_{ Component<Collider>() }
	, ImGuiShowable(ShowType::Inspector, Entity::entityId_)
	, sawRadius_{ 2.5f }
	, rotateAngleSec_{ 30.0f }
	, sawOffset_{ 5.0f }
	, rotateAngleSecSaw_{ 360.0f }
{
	pMeshRenderer_->meshFileName = "Model/SawColumn.fbx";
	pMeshRenderer_->meshHandle = Fbx::Load(pMeshRenderer_->meshFileName);

	GameObject* pPillerObject = new GameObject();
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pPillerObject);
	EntityId pillerId = pPillerObject->GetEntityId();
	pPillarMeshRenderer_ = &(MeshRenderer::Get(pillerId));
	pPillarMeshRenderer_->meshFileName = "Model/SawPillar.fbx";
	pPillarMeshRenderer_->meshHandle = Fbx::Load(pPillarMeshRenderer_->meshFileName);
	pPillarTransform_ = &(Transform::Get(pillerId));
	pPillarTransform_->SetParent(GetEntityId());
	pPillarTransform_->position = pTransform_->position;
	pPillarTransform_->scale.z = sawOffset_;
	
	pSaw_ = Instantiate<Saw>();
	Transform& sawTransform = Transform::Get(pSaw_->GetEntityId());
	sawTransform.position = pTransform_->position + pTransform_->Forward() * sawOffset_;
	sawTransform.SetParent(GetEntityId());

	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	pCollider_->isStatic_ = false;
	displayName_ = "Saw";
}

mtgb::CircularSaw::~CircularSaw()
{
}

void mtgb::CircularSaw::Update()
{
	float angleRad = DirectX::XMConvertToRadians(rotateAngleSec_ * Time::DeltaTimeF());
	Quaternion rot = DirectX::XMQuaternionRotationAxis(Vector3::Up(), angleRad);
	pTransform_->rotate = rot * pTransform_->rotate;
	//RotatePillar();
}

void mtgb::CircularSaw::Draw() const
{
}

void mtgb::CircularSaw::ShowImGui()
{
}

void mtgb::CircularSaw::Start()
{
	Transform& sawTransform = Transform::Get(pSaw_->GetEntityId());
	Vector3 toSawDir = Vector3::Normalize(sawTransform.position - pTransform_->position);
	pPillarTransform_->rotate = Quaternion::LookRotation(toSawDir, pTransform_->Up());
}

mtgb::Saw::Saw()
	: GameObject()
	, IActor(GetEntityId())
	, pTransform_{ Component<Transform>() }
	, pMeshRenderer_{ Component<MeshRenderer>() }
	, pCollider_{ Component<Collider>() }
	, pRigidBody_{ Component<RigidBody>() }
	, rotateAngleSec_{ 360.0f }
	, radius_{ 3.0f }
	, takeDamageAmount_{ 1 }
{
	pTransform_->scale = Vector3{ radius_,radius_,radius_ };
	pCollider_->colliderType_ = ColliderType::TYPE_OBB;
	pMeshRenderer_->meshFileName = "Model/Saw.fbx";
	pMeshRenderer_->meshHandle = Fbx::Load("Model/Saw.fbx");
}

mtgb::Saw::~Saw()
{
}

void mtgb::Saw::Update()
{
	float angleRad = DirectX::XMConvertToRadians(rotateAngleSec_ * Time::DeltaTimeF());
	Quaternion rot = DirectX::XMQuaternionRotationAxis(Vector3::Up(), angleRad);
	pTransform_->rotate = rot * pTransform_->rotate;
}

void mtgb::Saw::Draw() const
{
}

void mtgb::Saw::Start()
{
}

void mtgb::Saw::ShowImGui()
{
	MTImGui::Instance().ShowComponents(Entity::entityId_);
}

void mtgb::Saw::OnStomped(IActor* _pOther)
{
	_pOther->TakeDamage(takeDamageAmount_);
}

void mtgb::Saw::OnHitSide(IActor* _pOther)
{
	_pOther->TakeDamage(takeDamageAmount_);
}

void mtgb::Saw::TakeDamage(int _damage)
{
}
