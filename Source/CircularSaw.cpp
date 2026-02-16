#include "stdafx.h"
#include "CircularSaw.h"

unsigned int mtgb::CircularSaw::generateCounter_{0};

mtgb::CircularSaw::CircularSaw()
	: GameObject()
	, ImGuiShowable(ShowType::INSPECTOR, Entity::entityId_)
	, pTransform_{Component<Transform>()}
	, pMeshRenderer_{Component<MeshRenderer>()}
	, pCollider_{Component<Collider>()}
	, sawRadius_{2.5f}
	, sawOffset_{5.0f}
	, rotateAngleSec_{45.0f}
	, rotateAngleSecSaw_{360.0f}
{
	pTransform_->position.z = -5.0f;

	pMeshRenderer_->meshFileName = "Model/SawColumn.fbx";
	pMeshRenderer_->meshHandle	 = Fbx::Load(pMeshRenderer_->meshFileName);

	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	pCollider_->isStatic_	  = false;
	// 型情報に登録された名前を取得
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(CircularSaw));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);
	displayName_		 = name_;
}

mtgb::CircularSaw::~CircularSaw()
{
}

void mtgb::CircularSaw::Update()
{
	float angleRad		= DirectX::XMConvertToRadians(rotateAngleSec_ * Time::DeltaTimeF());
	Quaternion rot		= DirectX::XMQuaternionRotationAxis(Vector3::Up(), angleRad);
	pTransform_->rotate = rot * pTransform_->rotate;
}

void mtgb::CircularSaw::Draw() const
{
}

void mtgb::CircularSaw::ShowImGui()
{
}

void mtgb::CircularSaw::Start()
{
	pTransform_->Compute();

	// ノコギリを作成
	pSaw_					= Instantiate<Saw>();
	Transform& sawTransform = Transform::Get(pSaw_->GetEntityId());
	// ノコギリをオフセット分ずらして配置
	sawTransform.position = pTransform_->position + pTransform_->Forward() * sawOffset_;
	// 子にする
	sawTransform.SetParent(GetEntityId());

	// 支柱を作成
	GameObject* pPillerObject = new GameObject();
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pPillerObject);
	EntityId pillerId				   = pPillerObject->GetEntityId();
	pPillarMeshRenderer_			   = &(MeshRenderer::Get(pillerId));
	pPillarMeshRenderer_->meshFileName = "Model/SawPillar.fbx";
	pPillarMeshRenderer_->meshHandle   = Fbx::Load(pPillarMeshRenderer_->meshFileName);
	pPillarTransform_				   = &(Transform::Get(pillerId));
	pPillarTransform_->SetParent(GetEntityId());
	pPillarTransform_->position = pTransform_->position;
	// 回転の原点からノコギリまで支柱を伸ばす
	pPillarTransform_->scale.z = sawOffset_;

	// 回転の原点からノコギリの方向を向かせる
	Vector3 toSawDir		  = Vector3::Normalize(sawTransform.position - pTransform_->position);
	pPillarTransform_->rotate = Quaternion::LookRotation(toSawDir, pTransform_->Up());
}

mtgb::Saw::Saw()
	: GameObject()
	, IActor(GetEntityId())
	, pTransform_{Component<Transform>()}
	, pMeshRenderer_{Component<MeshRenderer>()}
	, pCollider_{Component<Collider>()}
	, pRigidBody_{Component<RigidBody>()}
	, rotateAngleSec_{360.0f}
	, radius_{3.0f}
	, takeDamageAmount_{1}
{
	pTransform_->scale			 = Vector3{radius_, 1.0f, radius_};
	pCollider_->colliderType_	 = ColliderType::TYPE_OBB;
	pMeshRenderer_->meshFileName = "Model/Saw.fbx";
	pMeshRenderer_->meshHandle	 = Fbx::Load("Model/Saw.fbx");
}

mtgb::Saw::~Saw()
{
}

void mtgb::Saw::Update()
{
	float angleRad		= DirectX::XMConvertToRadians(rotateAngleSec_ * Time::DeltaTimeF());
	Quaternion rot		= DirectX::XMQuaternionRotationAxis(Vector3::Up(), angleRad);
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
