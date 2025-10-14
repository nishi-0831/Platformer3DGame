#include "Camera.h"

mtgb::Camera::Camera() : GameObject(GameObjectBuilder()
	.SetPosition({ 0,0,0 })
	.SetName("Camera")
	.Build())
	, pTransform_{ Component<Transform>() }
{
}

mtgb::Camera::~Camera()
{
}

void mtgb::Camera::Update()
{
	MTImGui::Instance().TypedShow(&pTransform_->position, "Camera");
	if (InputUtil::GetKeyDown(KeyCode::A))
	{
		pTransform_->position.x -= 0.03f;
	}
	if (InputUtil::GetKeyDown(KeyCode::D))
	{
		pTransform_->position.x += 0.03f;
	}
}

void mtgb::Camera::Draw() const
{
}
