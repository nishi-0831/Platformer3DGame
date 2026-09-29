#include "BeltConveyor.h"
#include "ActorManager.h"
#include <Graphics/ShaderManager.h>
#include <Graphics/Shader/UVScrollShader.h>

unsigned int BeltConveyor::generateCounter_ { 0 };

mtgb::BeltConveyor::BeltConveyor()
	: GameObject()
	, groundedEntity_ { INVALID_ENTITY }
	, pTransform_ { &Transform::Get(entityId_) }
	, pMeshRenderer_ { &MeshRenderer::Get(entityId_) }
	, pCollider_ { &Collider::Get(entityId_) }
	, pRigidBody_ { &RigidBody::Get(entityId_) }
	, pGrounedActor_ { nullptr }
	, reverse_ { false }
	, time_ { 0.0f }
	, scrollDir_ { Vector2::Zero() }
	, scrollSpeed_ { 1.0f }
	, speedSpinBox_ { "Speed", { "Slowly", "Normal", "Fast" }, { 1, 2, 4 }, 1 }
{
	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	// 型情報に登録された名前を取得
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(BeltConveyor));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);

	// RigidBodyの設定
	pRigidBody_->OnCollisionEnter(
		[this](EntityId _id)
		{
			OnCollisionEnter(_id);
		}
	);
	pRigidBody_->OnCollisionExit(
		[this](EntityId _id)
		{
			OnCollisionExit(_id);
		}
	);

	pMeshRenderer_->meshFileName = "Model/BeltConveyor.fbx";
	pMeshRenderer_->meshHandle	 = Fbx::Load(pMeshRenderer_->meshFileName);
	pMeshRenderer_->layer		 = AllLayer();
	pMeshRenderer_->shaderType	 = ShaderType::UV_SCROLL;
	pMeshRenderer_->SetOnPreRenderCallback(
		[this]()
		{
			SetConstantBuffer();
		}
	);

	scrollDir_.x = 0.0f;
	scrollDir_.y = 1.0f;
}

mtgb::BeltConveyor::~BeltConveyor() {}

void mtgb::BeltConveyor::Update()
{
	speedSpinBox_.Update();
	time_ += Time::DeltaTimeF();

	if (groundedEntity_ != INVALID_ENTITY && pGrounedActor_ != nullptr)
	{
		Vector3 externVel = reverse_ ? Vector3::Back() * speedSpinBox_.GetCurrValue()
									 : Vector3::Forward() * speedSpinBox_.GetCurrValue();
		pGrounedActor_->SetSurfaceVelocity(externVel);
	}
}

void mtgb::BeltConveyor::ShowImGui()
{
	GameObject::ShowImGui();
	speedSpinBox_.Update();
	speedSpinBox_.GetSpinBox().ShowImGui();
	ImGui::Checkbox("Reverse", &reverse_);
}

nlohmann::json mtgb::BeltConveyor::SerializeProperties() const
{
	nlohmann::json j = GameObject::SerializeProperties();
	j["speed"]		 = speedSpinBox_.SerializeCurrentSelection();
	j["reverse"]	 = reverse_;
	return j;
}

void mtgb::BeltConveyor::DeserializeProperties(const nlohmann::json& _json)
{
	GameObject::DeserializeProperties(_json);
	speedSpinBox_.DeserializeCurrentSelection(_json["speed"]);
	reverse_ = _json.value("reverse", false);
}

void mtgb::BeltConveyor::SetConstantBuffer() const
{
	auto cBuf = Game::System<ShaderManager>().GetShader(ShaderType::UV_SCROLL).GetConstantBuffer("Time");
	if (cBuf != nullptr)
	{
		UVScrollShader::TimeBuffer buf;
		buf.g_time			= time_;
		buf.g_scroll_speed	= Vector2(0.0f, scrollSpeed_ * speedSpinBox_.GetCurrValue() / pTransform_->scale.z);
		buf.g_texture_scale = Vector4(1.0f, 1.0f, pTransform_->scale.z, 1.0f);
		buf.g_reverse_uv	= reverse_;

		Vector2 scrollDir = scrollDir_;
		if (reverse_)
		{
			scrollDir.x *= -1.0f;
			scrollDir.y *= -1.0f;
		}

		buf.g_sroll_dir = scrollDir;
		cBuf->SetConstantBuffer(buf);
		cBuf->ApplyChanges(DirectX11Draw::pContext_.Get());
	}
}

void mtgb::BeltConveyor::OnCollisionEnter(EntityId _entityId)
{
	groundedEntity_ = _entityId;
	pGrounedActor_	= Game::System<ActorManager>().GetActor(groundedEntity_);
}

void mtgb::BeltConveyor::OnCollisionExit(EntityId _entityId)
{
	if (pGrounedActor_ != nullptr)
	{
		pGrounedActor_->SetSurfaceVelocity(Vector3::Zero());
	}
	groundedEntity_ = INVALID_ENTITY;
	pGrounedActor_	= nullptr;
}
