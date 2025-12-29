#include "stdafx.h"
#include "SkySphere.h"

mtgb::SkySphere::SkySphere()
	: GameObject()
	, pTransform_{ Component<Transform>() }
	, pMeshRenderer_{ Component<MeshRenderer>() }
	, ImGuiShowable(ShowType::Inspector, Entity::entityId_)
	, rotateAngleSec_{ 1.0f }
{
	name_ = "SkySphere";
	displayName_ = name_;
	pTransform_->scale = { 1000,1000,1000 };
	pMeshRenderer_->meshFileName = "Model/SkySphere.fbx";
	pMeshRenderer_->meshHandle = Fbx::Load(pMeshRenderer_->meshFileName);
	pMeshRenderer_->layer = AllLayer();
	pMeshRenderer_->shaderType = ShaderType::FbxParts;
}

mtgb::SkySphere::~SkySphere()
{
}

void mtgb::SkySphere::Update()
{
	float angleRad = DirectX::XMConvertToRadians(rotateAngleSec_ * Time::DeltaTimeF());
	Quaternion rot = DirectX::XMQuaternionRotationAxis(Vector3::Up(), angleRad);
	pTransform_->rotate = rot * pTransform_->rotate;
}

void mtgb::SkySphere::Draw() const
{
}

void mtgb::SkySphere::ShowImGui()
{
	MTImGui::Instance().ShowComponents(Entity::entityId_);
}

void mtgb::SkySphere::Start()
{
}
