#include "ImGuiEditorCamera.h"
#include "Game.h"
#include "GameObject.h"
#include "Transform.h"
#include "SceneSystem.h"
#include "InputData.h"
#include "CameraSystem.h"
#include "GameTime.h"
#include "ImGuiRenderer.h"
mtgb::ImGuiEditorCamera::ImGuiEditorCamera()
	: orbitDistance_{10.0f}
	, moveSpeed_{10.0f}
	, orbitSpeed_{1.0f}
	, rotateSensitivity_{1.0f}
	, pCameraTransform_{nullptr}
	, pTargetTransform_{nullptr}
	, angleX_{0.0f}
	, angleY_{0.0f}
	, hCamera_{INVALD_ENTITY}
{
	// Dolly
	sCameraOperation_
		.OnUpdate(CameraOperation::Dolly, [this]
			{
				DoDolly();
			})
		.RegisterTransition(CameraOperation::Dolly, CameraOperation::Track, []() { return (InputUtil::GetMouse(MouseCode::Middle) == false); });

	// Orbit
	sCameraOperation_
		.OnUpdate(CameraOperation::Orbit, [this]
			{
				DoOrbit();
			})
		.RegisterTransition(CameraOperation::Orbit, CameraOperation::Track, []() {return (InputUtil::GetKey(KeyCode::LeftMenu) == false); });

	// Pan
	sCameraOperation_
		.OnUpdate(CameraOperation::Pan, [this]
			{
				DoPan();
			})
		.RegisterTransition(CameraOperation::Pan, CameraOperation::Track, []() {return (InputUtil::GetMouse(MouseCode::Right) == false); });

	// Track
	sCameraOperation_
		.OnUpdate(CameraOperation::Track, [this]
			{
				DoTrack();
			})
		.RegisterTransition(CameraOperation::Track, CameraOperation::Pan,
			[this]()
			{
				return InputUtil::GetMouse(MouseCode::Right) && IsMouseInWindow(windowName_.c_str());
			})
		.RegisterTransition(CameraOperation::Track, CameraOperation::Orbit,
			[this]()
			{
				return InputUtil::GetKey(KeyCode::LeftMenu) && IsMouseInWindow(windowName_.c_str());
			})
		.RegisterTransition(CameraOperation::Track, CameraOperation::Dolly,
			[this]()
			{
				return InputUtil::GetMouse(MouseCode::Middle) && IsMouseInWindow(windowName_.c_str());
			});
		
}

mtgb::ImGuiEditorCamera::~ImGuiEditorCamera()
{
}

void mtgb::ImGuiEditorCamera::Initialize()
{
	GameObject* pCamera = new GameObject(
		GameObjectBuilder()
		.SetPosition({ 0,0,0 })
		.SetName("Camera")
		.Build());
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pCamera);

	pCameraTransform_ = &Game::System<TransformCP>().Get(pCamera->GetEntityId());
	hCamera_ = Game::System<CameraSystem>().RegisterDrawCamera(pCameraTransform_);
}

void mtgb::ImGuiEditorCamera::SetCamera()
{
	Game::System<CameraSystem>().SetDrawCamera(hCamera_);
}

void mtgb::ImGuiEditorCamera::SetWindowName(const char* _name)
{
	windowName_ = _name;
}

void mtgb::ImGuiEditorCamera::DoDolly()
{
	Vector3 mouseMove = InputUtil::GetMouseMove();
	if (mouseMove.Size() != 0)
	{
		// カメラの右、上ベクトル
		Vector3 right = pCameraTransform_->Right();
		Vector3 up = pCameraTransform_->Up();

		// 移動量を合成
		Vector3 move = right * -mouseMove.x + up * mouseMove.y;

		pCameraTransform_->position += move * moveSpeed_ * Time::DeltaTimeF();
	}
}

void mtgb::ImGuiEditorCamera::DoPan()
{
	Vector3 mouseMove = InputUtil::GetMouseMove();
	if (mouseMove.Size() != 0)
	{
		// マウス移動量を角度に変換
		angleY_ += mouseMove.x * rotateSensitivity_ * Time::DeltaTimeF(); // 水平角度
		angleX_ += mouseMove.y * rotateSensitivity_ * Time::DeltaTimeF(); // 鉛直角度

		// 鉛直角度を制限
		/*const float MAX_VERTICAL = DirectX::XMConvertToRadians(89.0f);
		angleX_ = std::clamp(angleX_, -MAX_VERTICAL, MAX_VERTICAL);*/

		static const float ROTATE_DISTANCE = 0.01f;
		// 本当は0.0fを渡してその場で回転させたかったが上手くいかなかった
		MoveCameraSpherical(ROTATE_DISTANCE);
	}
}

void mtgb::ImGuiEditorCamera::DoOrbit()
{
	Vector3 mouseMove = InputUtil::GetMouseMove();
	if (mouseMove.Size() != 0)
	{
		angleX_ += mouseMove.y * orbitSpeed_ * Time::DeltaTimeF();
		angleY_ += mouseMove.x * orbitSpeed_ * Time::DeltaTimeF();

		MoveCameraSpherical(orbitDistance_);
	}
}

void mtgb::ImGuiEditorCamera::DoTrack()
{
	Vector3 mouseMove = InputUtil::GetMouseMove();
	if (mouseMove.Size() != 0)
	{
		Vector3 forward = pCameraTransform_->Forward();

		Vector3 move = forward * mouseMove.z;

		pCameraTransform_->position += move * moveSpeed_ * Time::DeltaTimeF();
	}
}

void mtgb::ImGuiEditorCamera::MoveCameraSpherical(float _distance)
{
	// ref:https://ja.wikipedia.org/wiki/%E7%90%83%E9%9D%A2%E5%BA%A7%E6%A8%99%E7%B3%BB

	// 現在のカメラ位置から回転中心を計算
	Vector3 center = Vector3::Zero();
	// ターゲットを中心に回転
	if (pTargetTransform_)
	{
		Vector3 toTarget = pTargetTransform_->position - pCameraTransform_->position;
		center = pTargetTransform_->position + (Vector3::Normalize(-toTarget) * _distance);
	}
	// ターゲットがいない場合は正面を向いて回転
	else
	{
		center = pCameraTransform_->position + (pCameraTransform_->Forward() * _distance);
	}

	// θ (polar angle) : 鉛直方向
	float theta = angleX_;

	// φ (azimuthal angle): 水平方向
	float phi = angleY_;

	// 回転中心からのオフセット
	Vector3 offset;

	// 回転中心の方向を向く
	Vector3 lookDir = Vector3::Zero();
	if (_distance <= std::numeric_limits<float>::epsilon())
	{
		// 変換
		offset.x = sinf(theta) * cos(phi);
		offset.y = cos(theta);
		offset.z = sin(theta) * sin(phi);
	}
	else
	{
		// 変換
		offset.x = _distance * sinf(theta) * cos(phi);
		offset.y = -_distance * cos(theta);
		offset.z = -_distance * sin(theta) * sin(phi);

		// 位置を反映
		pCameraTransform_->position = center + offset;

		lookDir = center - pCameraTransform_->position;
	}

	if (lookDir.Size() == 0.0f)
	{
		// その場回転の時はoffsetの方向を向く
		pCameraTransform_->rotate = Quaternion::LookRotation(offset, Vector3::Up());
	}
	else
	{
		pCameraTransform_->rotate = Quaternion::LookRotation(lookDir.Normalize(), Vector3::Up());
	}
}
