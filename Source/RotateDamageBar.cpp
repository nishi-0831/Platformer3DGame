#include "RotateDamageBar.h"

unsigned int mtgb::RotateDamageBar::generateCounter_ { 0 };

mtgb::RotateDamageBar::RotateDamageBar()
	: GameObject()
	, pTransform_ { Component<Transform>() }
	, pMeshRenderer_ { Component<MeshRenderer>() }
	, pCollider_ { Component<Collider>() }
	, damageObjRadius_ { 1.0f }
	, rotationSpeedSpinBox_ { "RotationSpeed", { "Slowly", "Normal", "Fast" }, { 30, 60, 120 }, 1 }
	, damageObjCountSpinBox_ { SpinBox::CreateNumberSpinBox("SpikeCount", 0, MAX_DAMAGE_OBJ_COUNT, 1, 4) }
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

	// ダメージオブジェクト数のスピンボックスの値が変わった際のコールバック
	damageObjCountSpinBox_.SetOnValueChangedCallback(
		[this](SpinBox& _spinBox)
		{
			// スピンボックスの数値とダメージオブジェクトの数を合わせる

			// 数値とオブジェクト数の差
			int diff = _spinBox.GetNumber() - pDamageObjs_.size();
			// オブジェクトを減らすか否か
			bool isDecrease = std::signbit(diff);
			int diffAbs		= std::abs(diff);
			for (int i = 0; i < diffAbs; i++)
			{
				// 減らす場合はオブジェクトを削除、増やす場合は追加で作成
				if (isDecrease)
				{
					RemoveDamageObject();
				}
				else
				{
					AddDamageObject();
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
	// ダメージオブジェクトを全て破棄
	while (pDamageObjs_.empty() == false)
	{
		pDamageObjs_.top()->DestroyMe();
		pDamageObjs_.pop();
	}
}

void mtgb::RotateDamageBar::Update()
{
	// 丸影を落とす位置を指定する
	Game::System<ShadowSettings>().AddCaster(GetEntityId());

	// 回転速度をスピンボックスから受け取る
	rotationSpeedSpinBox_.Update();

	// 自転して、子を回転させる
	float rotationAngleSec = static_cast<float>(rotationSpeedSpinBox_.GetCurrValue());
	float angleRad		   = DirectX::XMConvertToRadians(rotationAngleSec * Time::DeltaTimeF());
	Quaternion rot		   = DirectX::XMQuaternionRotationAxis(Vector3::Up(), reverse_ ? -angleRad : angleRad);
	pTransform_->rotate	   = rot * pTransform_->rotate;
}

void mtgb::RotateDamageBar::OnPreDrawScene() const {}

void mtgb::RotateDamageBar::ShowImGui()
{
	GameObject::ShowImGui();
	// ダメージオブジェクト数のスピンボックス表示
	damageObjCountSpinBox_.ShowImGui();
	// 回転速度のスピンボックス表示
	rotationSpeedSpinBox_.Update();
	rotationSpeedSpinBox_.GetSpinBox().ShowImGui();
	// プレイシーン開始時の回転角度のスピンボックス表示
	initialRotationAngleSpinBox_.ShowImGui();
	ImGui::Checkbox("Reverse", &reverse_);
}

void mtgb::RotateDamageBar::Start()
{
	// シーン開始時に初期値分だけ回転させる
	RotateInitialAngle();
	// ダメージオブジェクトを全て破棄
	while (pDamageObjs_.empty() == false)
	{
		pDamageObjs_.top()->DestroyMe();
		pDamageObjs_.pop();
	}
	// ダメージオブジェクト作成
	for (int i = 0; i < damageObjCountSpinBox_.GetNumber(); i++)
	{
		AddDamageObject();
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
	for (int i = 0; i < damageObjCountSpinBox_.GetNumber(); i++)
	{
		AddDamageObject();
	}
}

void mtgb::RotateDamageBar::AddDamageObject()
{
	// 自身からダメージオブジェクトまでのオフセットを計算
	int damageObjCnt	= static_cast<int>(pDamageObjs_.size());
	float spikeDiameter = damageObjRadius_ * 2;
	float offset		= (damageObjCnt + 1) * spikeDiameter;

	// ダメージオブジェクト作成
	auto pDamageObj = Instantiate<DamageObject>();

	// 座標、スケール、親子関係を設定
	pDamageObj->pTransform_->position = pTransform_->position + pTransform_->Forward() * offset;
	pDamageObj->pTransform_->SetParent(GetEntityId());
	pDamageObj->pTransform_->scale = Vector3(damageObjRadius_, damageObjRadius_, damageObjRadius_);

	// 3Dモデル設定
	pDamageObj->pMeshRenderer_->meshFileName = "Model/SpikeBall.fbx";
	pDamageObj->pMeshRenderer_->meshHandle	 = Fbx::Load(pDamageObj->pMeshRenderer_->meshFileName);

	// コライダーの種類設定
	pDamageObj->pCollider_->colliderType_ = ColliderType::TYPE_SPHERE;

	// 他オブジェクトに押し出されない
	pDamageObj->pRigidBody_->isKinematic_ = true;

	pDamageObjs_.push(pDamageObj);
}

void mtgb::RotateDamageBar::RemoveDamageObject()
{
	pDamageObjs_.top()->DestroyMe();
	pDamageObjs_.pop();
}

nlohmann::json mtgb::RotateDamageBar::SerializeProperties() const
{
	nlohmann::json j		  = GameObject::SerializeProperties();
	j["rotationSpeed"]		  = rotationSpeedSpinBox_.SerializeCurrentSelection();
	j["damageObjCount"]		  = damageObjCountSpinBox_.Serialize();
	j["damageObjRadius"]	  = damageObjRadius_;
	j["initialRotationAngle"] = initialRotationAngleSpinBox_.Serialize();
	return j;
}

void mtgb::RotateDamageBar::DeserializeProperties(const nlohmann::json& _json)
{
	GameObject::DeserializeProperties(_json);
	rotationSpeedSpinBox_.DeserializeCurrentSelection(_json["rotationSpeed"]);
	damageObjCountSpinBox_.Deserialize(_json.at("damageObjCount"));
	damageObjRadius_ = _json.at("damageObjRadius").get<float>();
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

void mtgb::DamageObject::Update()
{
	// 丸影を落とす位置を指定する
	// 丸影の半径に、X軸のスケールを使用する(XYZ軸が同じスケールである前提で)
	Game::System<ShadowSettings>().AddCaster(GetEntityId(), pTransform_->scale.x);
}

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
