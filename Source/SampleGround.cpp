#include "SampleGround.h"

mtgb::SampleGround::SampleGround() : GameObject(GameObjectBuilder()
	.SetPosition({0,0,0})
	.SetName("SampleGround")
	.Build())
	,pTransform_{Component<Transform>()}
	,pMeshRenderer_{Component<MeshRenderer>()}
{
	pMeshRenderer_->SetMesh(Fbx::Load("Model/TestGround.fbx"));
}

mtgb::SampleGround::~SampleGround()
{
}

void mtgb::SampleGround::Update()
{
}

void mtgb::SampleGround::Draw() const
{
}
