#include "SampleGround.h"

mtgb::SampleGround::SampleGround() : GameObject(GameObjectBuilder()
	.SetPosition({0,0,10})
	.SetScale({1,1,1})
	.SetName("SampleGround")
	.Build())
	,pTransform_{Component<Transform>()}
	,pMeshRenderer_{Component<MeshRenderer>()}
	,pCollider_{Component<Collider>()}
{
	pMeshRenderer_->SetMesh(Fbx::Load("Model/TestGround.fbx"));
	pCollider_->type_ = Collider::TYPE_AABB;
	pCollider_->SetCenter(pTransform_->position);
	pCollider_->SetExtents(pTransform_->scale);
}

mtgb::SampleGround::~SampleGround()
{
}

void mtgb::SampleGround::Update()
{
	MTImGui::Instance().TypedShow(&pTransform_->position, "Ground");
}

void mtgb::SampleGround::Draw() const
{
	pCollider_->Draw();
}
