#include "CameraResource.h"
#include "WindowContextResource.h"
#include "GameObject.h"
#include "ReleaseUtility.h"
#include "SceneSystem.h"
#include "MTAssert.h"
#include "CameraSystem.h"

mtgb::CameraResource::CameraResource(WindowContext _windowContext)
    : WindowContextResource(_windowContext)
    , hCamera_{INVALID_HANDLE}
{
}

mtgb::CameraResource::~CameraResource()
{
}

void mtgb::CameraResource::SetResource()
{
    // カメラ管理クラスに、カメラのハンドルを登録
    Game::System<CameraSystem>().SetDrawCamera(hCamera_);
}

void mtgb::CameraResource::Release()
{

}

void mtgb::CameraResource::SetHCamera(CameraHandleInScene _hCamera)
{
    // カメラのハンドルを受け取る
    hCamera_ = _hCamera;
}

mtgb::CameraHandleInScene mtgb::CameraResource::GetHCamera()
{
    return hCamera_;
}

bool mtgb::CameraResource::NotRegisterCamera()
{
    return hCamera_ == INVALID_HANDLE;
}