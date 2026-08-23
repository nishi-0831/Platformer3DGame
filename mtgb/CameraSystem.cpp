#include "CameraSystem.h"
#include "SceneSystem.h"
#include <algorithm>
#include "MTAssert.h"
#include "Screen.h"
#include <DirectXMath.h>

#include "Debug.h"
#include "Math/Vector2Int.h"
#include "WindowContext/WindowContextUtil.h"
#include "WindowContext/Direct3DResource.h"
#include "WindowContext/CameraResource.h"
#include "Editor/MTImGui.h"
#include "Audio.h"
namespace
{
	constexpr float DEFAULT_FOV { 60.0f };	   // デフォルトの視野角 (Field Of View)
	constexpr float DEFAULT_NEAR { 0.1f };	   // デフォルトのニヤー距離
	constexpr float DEFAULT_FAR { 100000.0f }; // デフォルトのファー距離
} // namespace

mtgb::CameraSystem::CameraSystem()
	: pTransforms_ {}
	, fov_ { DEFAULT_FOV }
	, near_ { DEFAULT_NEAR }
	, far_ { DEFAULT_FAR }
	, hCurrentCamera_ { INVALID_HANDLE }
	, currentFrameId_ { 0 }
{
}

mtgb::CameraSystem::~CameraSystem() {}

void mtgb::CameraSystem::Initialize() {}

void mtgb::CameraSystem::Update()
{
	currentFrameId_++;
}

mtgb::CameraHandleInScene mtgb::CameraSystem::RegisterDrawCamera(Transform* _pCameraTransform)
{
	CameraHandleInScene handle { INVALID_HANDLE };

	// もしカメラが未登録なら
	if (pTransforms_.size() == 0)
	{
		handle = 0;
		pTransforms_.push_back(_pCameraTransform);
		SetDrawCamera(handle);
		return handle;
	}

	auto itr { std::find(pTransforms_.begin(), pTransforms_.end(), _pCameraTransform) };

	// 見つからなかった
	if (itr == pTransforms_.end())
	{
		handle = static_cast<CameraHandleInScene>(pTransforms_.size());
		pTransforms_.push_back(_pCameraTransform);
	}
	else // 既にある
	{
		// インデックスを返す
		handle = static_cast<CameraHandleInScene>(itr - pTransforms_.begin());
	}

	return handle;
}

void mtgb::CameraSystem::ClearDrawCameraAll()
{
	pTransforms_.clear();
}

void mtgb::CameraSystem::UnregisterDrawCamera(const Transform* _pCameraTransform)
{
	for (auto itr = pTransforms_.begin(); itr != pTransforms_.end(); itr++)
	{
		if ((*itr) == _pCameraTransform)
		{
			(*itr) = nullptr;
			return; // 一致したらnullptrにして回帰
		}
	}
}

void mtgb::CameraSystem::SetDrawCamera(CameraHandleInScene _hCamera)
{
	hCurrentCamera_ = _hCamera;
}

mtgb::Vector3 mtgb::CameraSystem::GetWorldToScreenPos(const Vector3& _pos, const WorldToScreenData& _data) const
{
	return DirectX::XMVector3Project(
		_pos,
		_data.viewport.TopLeftX,
		_data.viewport.TopLeftY,
		_data.viewport.Width,
		_data.viewport.Height,
		_data.viewport.MinDepth,
		_data.viewport.MaxDepth,
		_data.projMat,
		_data.viewMat,
		DirectX::XMMatrixIdentity()
	);
}

mtgb::Vector3 mtgb::CameraSystem::GetWorldToScreenPos(const Vector3& _pos, WindowContext _context) const
{
	const D3D11_VIEWPORT& viewport = WinCtxRes::Get<Direct3DResource>(_context).GetViewport();
	CameraHandleInScene hCamera	   = WinCtxRes::Get<CameraResource>(_context).GetHCamera();

	Matrix4x4 projMat, viewMat;
	GetProjMatrix(&projMat);
	GetViewMatrix(&viewMat, hCamera);

	return DirectX::XMVector3Project(
		_pos,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		projMat,
		viewMat,
		DirectX::XMMatrixIdentity()
	);
}

const mtgb::Transform& mtgb::CameraSystem::GetTransform() const
{
	return GetTransform(hCurrentCamera_);
}

const mtgb::Transform& mtgb::CameraSystem::GetTransform(CameraHandleInScene _hCamera) const
{

	Transform* pTransform { pTransforms_[_hCamera] };

	massert(pTransform != nullptr && "既に無効化されたカメラが参照されました。");

	return *pTransform;
}

const mtgb::Transform& mtgb::CameraSystem::GetTransform(WindowContext _context) const
{
	return GetTransform(WinCtxRes::Get<CameraResource>(_context).GetHCamera());
}

void mtgb::CameraSystem::GetViewMatrix(Matrix4x4* _pView) const
{
	GetViewMatrix(_pView, hCurrentCamera_);
}

void mtgb::CameraSystem::GetViewMatrix(Matrix4x4* _pView, CameraHandleInScene _hCamera) const
{
	const Transform& cameraTransform { GetTransform(_hCamera) };

	Vector4 vEyePt { cameraTransform.GetWorldPosition() };			   // カメラ位置
	Vector4 vLookatPt { Vector4(cameraTransform.Forward()) + vEyePt }; // 注視位置
	Vector4 vUpVec { cameraTransform.Up() };						   // 上方位置
	*_pView = XMMatrixLookAtLH(vEyePt, vLookatPt, vUpVec);
}

void mtgb::CameraSystem::GetProjMatrix(Matrix4x4* _pProj) const
{
	using DirectX::XMConvertToRadians;
	using DirectX::XMMatrixPerspectiveFovLH;

	const Vector2Int SCREEN_SIZE { Game::System<Screen>().GetSize() };

	*_pProj = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(fov_),
		static_cast<float>(SCREEN_SIZE.x) / static_cast<float>(SCREEN_SIZE.y),
		near_,
		far_
	);
}

void mtgb::CameraSystem::GetPosition(Vector4* _pPosition) const
{
	*_pPosition = Vector4 { GetTransform().GetWorldPosition() };
}

float mtgb::CameraSystem::GetNear() const
{
	return near_;
}

float mtgb::CameraSystem::GetFar() const
{
	return far_;
}

float mtgb::CameraSystem::GetFov() const
{
	return fov_;
}

const mtgb::WorldToScreenData& mtgb::CameraSystem::GetWorldToScreenData(WindowContext _context)
{
	auto itr = worldToScreenDatas_.find(_context);

	// 新しく作成
	if (itr == worldToScreenDatas_.end())
	{
		WorldToScreenData data;
		CalculateWorldToScreenData(&data, _context);
		// 作成したものを返す
		return worldToScreenDatas_.insert(std::make_pair(_context, data)).first->second;
	}
	if (currentFrameId_ != itr->second.frameId)
	{
		CalculateWorldToScreenData(&itr->second, itr->first);
	}

	return itr->second;
}

void mtgb::CameraSystem::CalculateWorldToScreenData(WorldToScreenData* _data, WindowContext _context)
{
	_data->frameId	= currentFrameId_;
	_data->viewport = WinCtxRes::Get<Direct3DResource>(_context).GetViewport();

	GetProjMatrix(&_data->projMat);
	GetViewMatrix(&_data->viewMat, WinCtxRes::Get<CameraResource>(_context).GetHCamera());
}
