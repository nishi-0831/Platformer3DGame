#pragma once
#include "Handlers.h"
#include "MTStat/MTStat.h"
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
	private:
		Transform* pCameraTransform_;
		float angleX_, angleY_;
		CameraHandleInScene hCamera_;

		enum class CameraOperation
		{
			None,
			Translate,
			Pan,	// カメラ自身を軸として回転
			Orbit	// ターゲットを軸として回転
		};
		CameraOperation cameraOperation_;
		mtstat::MTStat<CameraOperation> sCameraOperation_;
		float rotateSensitivity_;
		float moveSpeed_;
		float orbitSpeed_;
		float orbitDistance_;
	};
}