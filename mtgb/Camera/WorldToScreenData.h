#pragma once
#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Handlers.h"
#include <d3d11.h>
namespace mtgb
{
	/// <summary>
	/// ワールド座標をスクリーン座標に変換する際に使用するデータ
	/// </summary>
	struct WorldToScreenData
	{
		Matrix4x4 viewMat, projMat;
		D3D11_VIEWPORT viewport;
		uint64_t frameId;
	};
} // namespace mtgb