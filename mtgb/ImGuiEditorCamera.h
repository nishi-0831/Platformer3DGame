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
			Pan,	// �J�������g�����Ƃ��ĉ�]
			Orbit	// �^�[�Q�b�g�����Ƃ��ĉ�]
		};
		CameraOperation cameraOperation_;
		mtstat::MTStat<CameraOperation> sCameraOperation_;
		float rotateSensitivity_;
		float moveSpeed_;
		float orbitSpeed_;
		float orbitDistance_;
	};
}