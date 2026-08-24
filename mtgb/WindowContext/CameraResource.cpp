#include "CameraResource.h"
#include "WindowContextResource.h"
#include "Core/GameObject/GameObject.h"
#include "Utility/ReleaseUtility.h"
#include "Core/SceneSystem.h"
#include "Utility/MTAssert.h"
#include "Camera/CameraSystem.h"

mtgb::CameraResource::CameraResource(WindowContext _windowContext)
	: WindowContextResource(_windowContext)
	, hCamera_ { INVALID_HANDLE }
{
}

mtgb::CameraResource::~CameraResource() {}

void mtgb::CameraResource::SetResource()
{
	if (hCamera_ == INVALID_HANDLE)
		return;
	// カメラ管理クラスに、カメラのハンドルを登録
	Game::System<CameraSystem>().SetDrawCamera(hCamera_);
}

void mtgb::CameraResource::Release() {}

void mtgb::CameraResource::SetHCamera(CameraHandleInScene _hCamera)
{
	// カメラのハンドルを受け取る
	hCamera_ = _hCamera;
}

mtgb::CameraHandleInScene mtgb::CameraResource::GetHCamera() const
{
	return hCamera_;
}

bool mtgb::CameraResource::NotRegisterCamera() const
{
	return hCamera_ == INVALID_HANDLE;
}