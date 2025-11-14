#include "Camera.h"


namespace
{
	const mtgb::Vector3 INIT_ANGLE{ 0,0,0 };
}

mtgb::Camera::Camera()
{
	pCameraTransform_ = Component<Transform>();
}

mtgb::Camera::Camera(GameObject* _pGameObj) : GameObject(GameObjectBuilder()
	.SetPosition({ 0,0,0 })
	.SetName("Camera")
	.Build())
{
	polarAngleRad_ = DirectX::XMConvertToRadians(INIT_ANGLE.x + 90.0f);
	azimuthalAngleRad_ = DirectX::XMConvertToRadians(INIT_ANGLE.y + 90.0f);

	orbitSpeed_ = 1.0f;
	distance_ = 5.0f;
	followTarget_ = true;
	adjustTargetDirection_ = false;
	minPolarAngleRad_ = DirectX::XMConvertToRadians(60.0f);
	maxPolarAngleRad_ = DirectX::XMConvertToRadians(150.0f);
	lookAtPositionOffset_ = { 0,0.5,0 };

	pCameraTransform_ = Component<Transform>();
	pTargetTransform_ = &Transform::Get(_pGameObj->GetEntityId());

	inputType_ = InputType::JOYPAD;
}

mtgb::Camera::~Camera()
{
}

void mtgb::Camera::Update()
{
	if (pTargetTransform_ == nullptr)
		return;

	MTImGui::Instance().DirectShow([&]()
		{
			TypeRegistry::Instance().CallFunc<Transform>(pCameraTransform_, "Transform");
			float degX = DirectX::XMConvertToDegrees(polarAngleRad_);
			float degY = DirectX::XMConvertToDegrees(azimuthalAngleRad_);
			ImGui::Text("polar angle, %.3f", degX);
			ImGui::Text("azimuthal angle, %3f", degY);
		},"Camera",ShowType::Inspector);

	DoOrbit();
	FollowTarget();
}

void mtgb::Camera::Draw() const
{
}