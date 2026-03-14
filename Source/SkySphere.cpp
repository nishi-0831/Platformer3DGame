#include "stdafx.h"
#include "SkySphere.h"

mtgb::SkySphere::SkySphere()
	: GameObject()
	, ImGuiShowable(ShowType::INSPECTOR, Entity::entityId_)
	, pTransform_ { Component<Transform>() }
	, pMeshRenderer_ { Component<MeshRenderer>() }
	, rotateAngleSec_ { 1.0f }
	, sphereScale_ { 1000.0f }
{
	name_						 = "SkySphere";
	displayName_				 = name_;
	pTransform_->scale			 = { sphereScale_, sphereScale_, sphereScale_ };
	pMeshRenderer_->meshFileName = "Model/SkySphere.fbx";
	pMeshRenderer_->meshHandle	 = Fbx::Load(pMeshRenderer_->meshFileName);
	pMeshRenderer_->layer		 = AllLayer();
	pMeshRenderer_->shaderType	 = ShaderType::FBX_PARTS;
}

mtgb::SkySphere::~SkySphere() {}

void mtgb::SkySphere::Update()
{
	float angleRad		= DirectX::XMConvertToRadians(rotateAngleSec_ * Time::DeltaTimeF());
	Quaternion rot		= DirectX::XMQuaternionRotationAxis(Vector3::Up(), angleRad);
	pTransform_->rotate = rot * pTransform_->rotate;
}

void mtgb::SkySphere::Draw() const {}

void mtgb::SkySphere::ShowImGui()
{
	MTImGui::ShowComponents(Entity::entityId_);
}

void mtgb::SkySphere::Start() {}
