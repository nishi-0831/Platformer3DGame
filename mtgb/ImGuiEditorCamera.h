#pragma once
#include "Handlers.h"
#include "MTStat/MTStat.h"
#include "ImGuiShowable.h"
#include <string>
#include <d3d11.h>
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
	class ImGuiEditorCamera : public ImGuiShowable
	{
	public:
		ImGuiEditorCamera();
		~ImGuiEditorCamera();
		void ShowImGui() override;
		void Initialize();
		void SetCamera();
		
		void UpdateState();
		void SetWindowName(const char* _name);
		void SetViewPort(const D3D11_VIEWPORT& _viewport);
	private:
		void DoDolly();
		void DoPan();
		void DoOrbit();
		void DoTrack();

		void FollowTarget();
		void MoveCameraSpherical(float _distance);
		void MoveCameraSphericalOnTheSpot();
		void SelectTransform();
		Transform* pCameraTransform_;
		Transform* pTargetTransform_;
		float angleX_, angleY_;
		CameraHandleInScene hCamera_;
		D3D11_VIEWPORT viewport_;
		mtstat::MTStat<CameraOperation> sCameraOperation_;
		float rotateSensitivity_;
		float moveSpeed_;
		float orbitSpeed_;
		float orbitDistance_;
		float followDistance_;
		bool followTarget_;
		bool adjustTargetDirection_;

		std::string windowName_;
	};
}