#pragma once
#include "Handlers.h"
#include "MTStat/MTStat.h"
#include <string>
namespace mtgb
{
	class Transform;
	class ImGuiEditorCamera
	{
	public:
		ImGuiEditorCamera();
		~ImGuiEditorCamera();
		void Initialize();
		void SetCamera();
		
		void SetWindowName(const char* _name);
	private:
		void DoDolly();
		void DoPan();
		void DoOrbit();
		void DoTrack();


		void MoveCameraSpherical(float _distance);
		Transform* pCameraTransform_;
		Transform* pTargetTransform_;
		float angleX_, angleY_;
		CameraHandleInScene hCamera_;

		enum class CameraOperation
		{
			Track,
			Dolly, // カメラを左右に移動
			
			Pan,	// カメラ自身を軸として回転
			Orbit	// ターゲットを軸として回転
		};
		mtstat::MTStat<CameraOperation> sCameraOperation_;
		float rotateSensitivity_;
		float moveSpeed_;
		float orbitSpeed_;
		float orbitDistance_;

		std::string windowName_;
	};
}