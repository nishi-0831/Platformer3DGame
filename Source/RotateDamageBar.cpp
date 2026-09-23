#include "RotateDamageBar.h"

unsigned int mtgb::RotateDamageBar::generateCounter_ { 0 };

mtgb::RotateDamageBar::RotateDamageBar()
	: GameObject()
	, pTransform_ { Component<Transform>() }
	, pMeshRenderer_ { Component<MeshRenderer>() }
	, pCollider_ { Component<Collider>() }
	, rotateAngleSec_ { 60.0f }
	, spikeCount_ { 4 }
	, spikeRadius_ { 1.0f }
	, rotateSpeedSpinBox_ { "RotateSpeed", { "Slowly", "Normal", "Fast" }, { 30, 60, 120 }, 1 }
	, spikeCountSpinBox_ { SpinBox::CreateNumberSpinBox("SpikeCount", 0, MAX_SPIKE_COUNT, 1) }
{
	pMeshRenderer_->meshFileName = "Model/SawColumn.fbx";
	pMeshRenderer_->meshHandle	 = Fbx::Load(pMeshRenderer_->meshFileName);

	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	pCollider_->isStatic_	  = false;
	// 型情報に登録された名前を取得
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(RotateDamageBar));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);
}

mtgb::RotateDamageBar::~RotateDamageBar() {}

void mtgb::RotateDamageBar::Update()
{
	rotateName_			= rotateSpeedSpinBox_.GetSpinBox().GetString();
	rotateAngleSec_		= rotateSpeedSpinBox_.GetCurrValue();
	float angleRad		= DirectX::XMConvertToRadians(rotateAngleSec_ * Time::DeltaTimeF());
	Quaternion rot		= DirectX::XMQuaternionRotationAxis(Vector3::Up(), angleRad);
	pTransform_->rotate = rot * pTransform_->rotate;
}

void mtgb::RotateDamageBar::Draw() const {}

void mtgb::RotateDamageBar::ShowImGui()
{
	GameObject::ShowImGui();
	if (ImGui::Button("Increment Spike"))
	{
		AddSpike();
	}
	if (ImGui::Button("Decrement Spike"))
	{
		RemoveSpike();
	}
	rotateSpeedSpinBox_.GetSpinBox().ShowImGui();
}

void mtgb::RotateDamageBar::Start()
{
	while (pDamageObjs_.empty() == false)
	{
		pDamageObjs_.top()->DestroyMe();
		pDamageObjs_.pop();
	}
	for (int i = 0; i < spikeCount_; i++)
	{
		AddSpike();
	}
}

void mtgb::RotateDamageBar::StartOnEditMode()
{
	while (pDamageObjs_.empty() == false)
	{
		pDamageObjs_.top()->DestroyMe();
		pDamageObjs_.pop();
	}
	for (int i = 0; i < spikeCount_; i++)
	{
		AddSpike();
	}
}

void mtgb::RotateDamageBar::AddSpike()
{
	int damageObjCnt				  = static_cast<int>(pDamageObjs_.size());
	float spikeDiameter				  = spikeRadius_ * 2;
	float offset					  = (damageObjCnt + 1) * spikeDiameter;
	auto pDamageObj					  = Instantiate<DamageObject>();
	pDamageObj->pTransform_->position = pTransform_->position + pTransform_->Forward() * offset;
	pDamageObj->pTransform_->SetParent(GetEntityId());
	pDamageObj->pTransform_->scale			 = Vector3(spikeRadius_, spikeRadius_, spikeRadius_);
	pDamageObj->pMeshRenderer_->meshFileName = "Model/SpikeBall.fbx";
	pDamageObj->pMeshRenderer_->meshHandle	 = Fbx::Load(pDamageObj->pMeshRenderer_->meshFileName);
	pDamageObj->pCollider_->colliderType_	 = ColliderType::TYPE_SPHERE;
	pDamageObj->pRigidBody_->isKinematic_	 = true;
	pDamageObjs_.push(pDamageObj);
}

void mtgb::RotateDamageBar::RemoveSpike()
{
	pDamageObjs_.top()->DestroyMe();
	pDamageObjs_.pop();
}

void mtgb::RotateDamageBar::OnPreSave()
{
	rotateName_ = rotateSpeedSpinBox_.GetSpinBox().GetString();
}

nlohmann::json mtgb::RotateDamageBar::Serialize() const
{
	nlohmann::json j = GameObject::Serialize();
	j["rotateName"]	 = rotateName_;
	j["spikeCount"]	 = spikeCount_;
	j["spikeRadius"] = spikeRadius_;
	return j;
}

void mtgb::RotateDamageBar::Deserialize(const nlohmann::json& _json)
{
	GameObject::Deserialize(_json);
	rotateName_	 = _json.at("rotateName").get<std::string>();
	spikeCount_	 = _json.at("spikeCount").get<int>();
	spikeRadius_ = _json.at("spikeRadius").get<float>();
	rotateSpeedSpinBox_.GetSpinBox().SetString(rotateName_);
}

mtgb::DamageObject::DamageObject()
	: GameObject()
	, IActor(GetEntityId())
	, pTransform_ { Component<Transform>() }
	, pMeshRenderer_ { Component<MeshRenderer>() }
	, pCollider_ { Component<Collider>() }
	, pRigidBody_ { Component<RigidBody>() }
	, takeDamageAmount_ { 1 }
{
	pCollider_->colliderType_ = ColliderType::TYPE_OBB;
}

mtgb::DamageObject::~DamageObject() {}

void mtgb::DamageObject::Update() {}

void mtgb::DamageObject::Draw() const {}

void mtgb::DamageObject::Start() {}

void mtgb::DamageObject::OnStomped(IActor* _pOther)
{
	_pOther->TakeDamage(takeDamageAmount_);
}

void mtgb::DamageObject::OnHitSide(IActor* _pOther)
{
	_pOther->TakeDamage(takeDamageAmount_);
}

void mtgb::DamageObject::TakeDamage(int _damage) {}
