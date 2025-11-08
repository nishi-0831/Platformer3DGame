#pragma once
#include "Handlers.h"
#include "MTStat/MTStat.h"
#include "ImGuiShowable.h"
#include <string>
#include "ISystem.h"
#include "ICamera.h"
namespace mtgb
{
	class Transform;
	enum class CameraOperation
	{
		Track,
		Dolly, // カメラを左右に移動
		Pan,	// カメラ自身を軸として回転
		Orbit	// ターゲットを軸として回転
	};
	class ImGuiEditorCamera : public ImGuiShowable ,public ISystem , public ICamera
	{
	public:
		ImGuiEditorCamera();
		~ImGuiEditorCamera();
		void ShowImGui() override;
		void Initialize() override;
		void SetCamera();
		
		void Update() override;
		void CreateCamera();
	private:
		void DoDolly();
		void DoPan();
		//void DoOrbit();
		void DoTrack();

		void FollowTarget();
		//void MoveCameraSpherical(float _distance);
		void MoveCameraSphericalOnTheSpot();
		void SelectTransform();
		//Transform* pCameraTransform_;
		//Transform* pTargetTransform_;
		//float angleX_, angleY_;
		CameraHandleInScene hCamera_;
		mtstat::MTStat<CameraOperation> sCameraOperation_;
		float rotateSensitivity_;
		float moveSpeed_;
		//float orbitSpeed_;
		//float orbitDistance_;
		//float followDistance_;
		//bool followTarget_;
		//bool adjustTargetDirection_;

		std::string windowName_;
	};
}