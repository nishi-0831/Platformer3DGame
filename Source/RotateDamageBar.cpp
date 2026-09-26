#include "RotateDamageBar.h"

unsigned int mtgb::RotateDamageBar::generateCounter_ { 0 };

mtgb::RotateDamageBar::RotateDamageBar()
	: GameObject()
	, pTransform_ { Component<Transform>() }
	, pMeshRenderer_ { Component<MeshRenderer>() }
	, pCollider_ { Component<Collider>() }
	, spikeRadius_ { 1.0f }
	, rotationSpeedSpinBox_ { "RotationSpeed", { "Slowly", "Normal", "Fast" }, { 30, 60, 120 }, 1 }
	, spikeCountSpinBox_ { SpinBox::CreateNumberSpinBox("SpikeCount", 0, MAX_SPIKE_COUNT, 1, 4) }
	, reverse_ { false }
	, initialRotationAngleSpinBox_ { SpinBox::CreateNumberSpinBox("InitialRotationAngle", 0, 360, 45, 0) }
{
	pMeshRenderer_->meshFileName = "Model/SawColumn.fbx";
	pMeshRenderer_->meshHandle	 = Fbx::Load(pMeshRenderer_->meshFileName);

	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	pCollider_->isStatic_	  = false;
	// 型情報に登録された名前を取得
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(RotateDamageBar));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);

	spikeCountSpinBox_.SetOnValueChangedCallback(
		[this](SpinBox& _spinBox)
		{
			int diff		= _spinBox.GetNumber() - pDamageObjs_.size();
			bool isDecrease = std::signbit(diff);
			int diffAbs		= std::abs(diff);
			for (int i = 0; i < diffAbs; i++)
			{
				if (isDecrease)
				{
					RemoveSpike();
				}
				else
				{
					AddSpike();
				}
			}
		}
	);

	initialRotationAngleSpinBox_.SetOnValueChangedCallback(
		[this](SpinBox& _spinBox)
		{
			RotateInitialAngle();
		}
	);
}

mtgb::RotateDamageBar::~RotateDamageBar()
{
	while (pDamageObjs_.empty() == false)
	{
		pDamageObjs_.top()->DestroyMe();
		pDamageObjs_.pop();
	}
}

void mtgb::RotateDamageBar::Update()
{
	rotationSpeedSpinBox_.Update();
	float rotationAngleSec = static_cast<float>(rotationSpeedSpinBox_.GetCurrValue());
	float angleRad		   = DirectX::XMConvertToRadians(rotationAngleSec * Time::DeltaTimeF());
	Quaternion rot		   = DirectX::XMQuaternionRotationAxis(Vector3::Up(), reverse_ ? -angleRad : angleRad);
	pTransform_->rotate	   = rot * pTransform_->rotate;
}

void mtgb::RotateDamageBar::OnPreDrawScene() const {}

void mtgb::RotateDamageBar::ShowImGui()
{
	GameObject::ShowImGui();
	spikeCountSpinBox_.ShowImGui();
	rotationSpeedSpinBox_.Update();
	rotationSpeedSpinBox_.GetSpinBox().ShowImGui();
	initialRotationAngleSpinBox_.ShowImGui();
	ImGui::Checkbox("Reverse", &reverse_);
}

void mtgb::RotateDamageBar::Start()
{
	RotateInitialAngle();
	while (pDamageObjs_.empty() == false)
	{
		pDamageObjs_.top()->DestroyMe();
		pDamageObjs_.pop();
	}
	for (int i = 0; i < spikeCountSpinBox_.GetNumber(); i++)
	{
		AddSpike();
	}
}

void mtgb::RotateDamageBar::StartOnEditMode()
{
	RotateInitialAngle();
	while (pDamageObjs_.empty() == false)
	{
		pDamageObjs_.top()->DestroyMe();
		pDamageObjs_.pop();
	}
	for (int i = 0; i < spikeCountSpinBox_.GetNumber(); i++)
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

nlohmann::json mtgb::RotateDamageBar::SerializeProperties() const
{
	nlohmann::json j		  = GameObject::SerializeProperties();
	j["rotationSpeed"]		  = rotationSpeedSpinBox_.SerializeCurrentSelection();
	j["spikeCount"]			  = spikeCountSpinBox_.Serialize();
	j["spikeRadius"]		  = spikeRadius_;
	j["initialRotationAngle"] = initialRotationAngleSpinBox_.Serialize();
	return j;
}

void mtgb::RotateDamageBar::DeserializeProperties(const nlohmann::json& _json)
{
	GameObject::DeserializeProperties(_json);
	rotationSpeedSpinBox_.DeserializeCurrentSelection(_json["rotationSpeed"]);
	spikeCountSpinBox_.Deserialize(_json.at("spikeCount"));
	spikeRadius_ = _json.at("spikeRadius").get<float>();
	initialRotationAngleSpinBox_.Deserialize(_json.at("initialRotationAngle"));
}

void mtgb::RotateDamageBar::RotateInitialAngle()
{
	using namespace DirectX;
	float angleRad		= XMConvertToRadians(static_cast<float>(initialRotationAngleSpinBox_.GetNumber()));
	pTransform_->rotate = XMQuaternionRotationAxis(Vector3::Up(), angleRad);
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

void mtgb::DamageObject::OnPreDrawScene() const {}

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
