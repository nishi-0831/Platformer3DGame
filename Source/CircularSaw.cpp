#include "stdafx.h"
#include "CircularSaw.h"

unsigned int mtgb::CircularSaw::generateCounter_ { 0 };

mtgb::CircularSaw::CircularSaw()
	: GameObject()
	, pTransform_ { Component<Transform>() }
	, pMeshRenderer_ { Component<MeshRenderer>() }
	, pPillarTransform_ { nullptr }
	, pPillarMeshRenderer_ { nullptr }
	, pSaw_ { nullptr }
	, pCollider_ { Component<Collider>() }
	, rotationSpeedSpinBox_ { "RotationSpeed", { "Slowly", "Normal", "Fast" }, { 30, 60, 120 }, 1 }
	, reverse_ { false }
	, initialRotationAngleSpinBox_ { SpinBox::CreateNumberSpinBox("InitialRotationAngle", 0, 360, 45, 0) }
	, sawOffsetSpinBox_ { SpinBox::CreateNumberSpinBox("Offset", 1, 10, 1, 5) }
{
	pTransform_->position.z = -5.0f;

	pMeshRenderer_->meshFileName = "Model/SawColumn.fbx";
	pMeshRenderer_->meshHandle	 = Fbx::Load(pMeshRenderer_->meshFileName);

	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	pCollider_->isStatic_	  = false;
	// 型情報に登録された名前を取得
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(CircularSaw));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);

	initialRotationAngleSpinBox_.SetOnValueChangedCallback(
		[this](SpinBox& _spinBox)
		{
			RotateInitialAngle();
		}
	);

	sawOffsetSpinBox_.SetOnValueChangedCallback(
		[this](SpinBox& _spinBox)
		{
			/// ノコギリをオフセット分ずらして配置
			Transform& sawTransform = Transform::Get(pSaw_->GetEntityId());
			float sawOffset			= static_cast<float>(sawOffsetSpinBox_.GetNumber());

			sawTransform.position = pTransform_->position + pTransform_->Forward() * sawOffset;
			// 回転の原点からノコギリまで支柱を伸ばす
			pPillarTransform_->scale.z = sawOffset;
		}
	);
}

mtgb::CircularSaw::~CircularSaw()
{
	// SEを停止
	Game::System<Audio>().Stop("Saw");
	if (pSaw_)
	{
		pSaw_->DestroyMe();
	}

	// 自身からのこぎりまでの柱を削除
	GameObject* pillar = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(pPillarTransform_->GetEntityId());
	if (pillar)
	{
		pillar->DestroyMe();
	}
}

void mtgb::CircularSaw::Update()
{
	// 丸影を落とす位置を指定する
	Game::System<ShadowSettings>().AddCaster(GetEntityId());

	// 回転速度をスピンボックスから受け取る
	rotationSpeedSpinBox_.Update();
	float rotateAngleSec = static_cast<float>(rotationSpeedSpinBox_.GetCurrValue());

	// 自転して、ノコギリを回転させる
	float angleRad		= DirectX::XMConvertToRadians(rotateAngleSec * Time::DeltaTimeF());
	Quaternion rot		= DirectX::XMQuaternionRotationAxis(Vector3::Up(), reverse_ ? -angleRad : angleRad);
	pTransform_->rotate = rot * pTransform_->rotate;
}
void mtgb::CircularSaw::ShowImGui()
{
	GameObject::ShowImGui();
	// 回転速度のスピンボックス表示
	rotationSpeedSpinBox_.Update();
	rotationSpeedSpinBox_.GetSpinBox().ShowImGui();
	// 自身からノコギリまでの距離のスピンボックス表示
	sawOffsetSpinBox_.ShowImGui();
	// プレイシーン開始時の回転角度のスピンボックス表示
	initialRotationAngleSpinBox_.ShowImGui();
	ImGui::Checkbox("Reverse", &reverse_);
}

void mtgb::CircularSaw::Start()
{
	CreateSaw();
}

void mtgb::CircularSaw::StartOnEditMode()
{
	CreateSaw();
}

nlohmann::json mtgb::CircularSaw::SerializeProperties() const
{
	nlohmann::json j		  = GameObject::SerializeProperties();
	j["rotationSpeed"]		  = rotationSpeedSpinBox_.SerializeCurrentSelection();
	j["reverse"]			  = reverse_;
	j["initialRotationAngle"] = initialRotationAngleSpinBox_.Serialize();
	j["sawOffset"]			  = sawOffsetSpinBox_.Serialize();
	return j;
}

void mtgb::CircularSaw::DeserializeProperties(const nlohmann::json& _json)
{
	GameObject::DeserializeProperties(_json);
	if (_json.contains("rotationSpeed"))
	{
		rotationSpeedSpinBox_.DeserializeCurrentSelection(_json.at("rotationSpeed"));
	}
	reverse_ = _json.value("reverse", false);
	if (_json.contains("initialRotationAngle"))
	{
		initialRotationAngleSpinBox_.Deserialize(_json.at("initialRotationAngle"));
	}
	if (_json.contains("sawOffset"))
	{
		sawOffsetSpinBox_.Deserialize(_json.at("sawOffset"));
	}
}

void mtgb::CircularSaw::RotateInitialAngle()
{
	using namespace DirectX;
	float angleRad		= XMConvertToRadians(static_cast<float>(initialRotationAngleSpinBox_.GetNumber()));
	pTransform_->rotate = XMQuaternionRotationAxis(Vector3::Up(), angleRad);
}

void mtgb::CircularSaw::CreateSaw()
{
	pTransform_->Compute();

	// ノコギリを作成
	pSaw_					= Instantiate<Saw>();
	pSaw_->isInspectable_	= false;
	Transform& sawTransform = Transform::Get(pSaw_->GetEntityId());
	// ノコギリをオフセット分ずらして配置
	float sawOffset		  = static_cast<float>(sawOffsetSpinBox_.GetNumber());
	sawTransform.position = pTransform_->position + pTransform_->Forward() * sawOffset;
	// 子にする
	sawTransform.SetParent(GetEntityId());

	// 支柱を作成
	GameObject* pPillerObject = new GameObject();
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pPillerObject);
	pPillerObject->isInspectable_	   = false;
	EntityId pillerId				   = pPillerObject->GetEntityId();
	pPillarMeshRenderer_			   = &(MeshRenderer::Get(pillerId));
	pPillarMeshRenderer_->meshFileName = "Model/SawPillar.fbx";
	pPillarMeshRenderer_->meshHandle   = Fbx::Load(pPillarMeshRenderer_->meshFileName);
	pPillarTransform_				   = &(Transform::Get(pillerId));
	pPillarTransform_->SetParent(GetEntityId());
	pPillarTransform_->position = pTransform_->position;
	// 回転の原点からノコギリまで支柱を伸ばす
	pPillarTransform_->scale.z = sawOffset;

	// 回転の原点からノコギリの方向を向かせる
	Vector3 toSawDir		  = Vector3::Normalize(sawTransform.position - pTransform_->position);
	pPillarTransform_->rotate = Quaternion::LookRotation(toSawDir, pTransform_->Up());

	RotateInitialAngle();
}
