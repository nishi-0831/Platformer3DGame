#pragma once
#include "WindowContextResource.h"
#include "Handlers.h"
namespace mtgb
{
	class GameObject;
	/// <summary>
	/// �E�B���h�E���Ƃ̃J����
	/// </summary>
	class CameraResource : public WindowContextResource
	{
	public:
		CameraResource();
		~CameraResource();
		CameraResource(const CameraResource& other);
		// WindowContextResource ����Čp������܂���
		void Initialize(WindowContext _windowContext) override;
		void Release() override;
		/// <summary>
		/// �J������؂�ւ���
		/// </summary>
		void SetResource() override;
		WindowContextResource* Clone() const override;

		void SetHCamera(CameraHandleInScene _hCamera);
		CameraHandleInScene GetHCamera();
		//void SetCamera(GameObject* obj);
	private:
		//GameObject* pCamera_;
		CameraHandleInScene hCamera_;
	};
}