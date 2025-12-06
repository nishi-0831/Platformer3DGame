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
		void DoTrack();

		void MoveCameraSphericalOnTheSpot();
		void SelectTransform();
		CameraHandleInScene hCamera_;
		mtstat::MTStat<CameraOperation> sCameraOperation_;
		float rotateSensitivity_;
		float moveSpeed_;

		std::string windowName_;
	};
}