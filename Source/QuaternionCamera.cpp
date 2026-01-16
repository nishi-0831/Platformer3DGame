#include "stdafx.h"
#include "QuaternionCamera.h"
namespace
{
	Vector3 euler;
}
mtgb::QuaternionCamera::QuaternionCamera(EntityId _entityId)
	: GameObject(GameObjectBuilder().SetPosition({0, 0, 0}).SetName("Camera").Build())
	, pTransform_{Component<Transform>()}
	, pTargetTransform_{&Transform::Get(_entityId)}
	, lookAtPositionOffset_{Vector3{0, 2, 0}}
	, rotateAngleDeg_{30.0f}
	, distance_{8.0f}
	, inputType_{InputType::JOYPAD}
{
}

mtgb::QuaternionCamera::~QuaternionCamera()
{
}

void mtgb::QuaternionCamera::Update()
{
	Vector3 movement;

	// デバイスから入力を取得する
	switch (inputType_)
	{
		// マウス
	case InputType::MOUSE :
		movement = InputUtil::GetMouseMove();
		break;

		// ゲームパッド
	case InputType::JOYPAD :
		Vector2F vec2 = InputUtil::GetAxis(StickType::RIGHT);
		movement.x	  = -vec2.y;
		movement.y	  = -vec2.x;
		break;
	}
	if (movement.Size() == 0)
	{
		return;
	}
	Quaternion& rotate		= pTransform_->rotate;
	Quaternion inputRotateX = Quaternion::AngleAxis(
		DirectX::XMConvertToRadians(rotateAngleDeg_ * Time::DeltaTimeF() * movement.x),
		Vector3::Right()
	);
	Quaternion inputRotateY = Quaternion::AngleAxis(
		DirectX::XMConvertToRadians(rotateAngleDeg_ * Time::DeltaTimeF() * movement.y),
		Vector3::Up()
	);
	rotate *= inputRotateY;
	rotate *= inputRotateX;

	lookAtPos_ = pTargetTransform_->position + lookAtPositionOffset_;

	Vector3 toTargetDir	  = DirectX::XMVector3Rotate(Vector3::Forward(), rotate);
	pTransform_->position = pTargetTransform_->position - (toTargetDir * distance_);
}
