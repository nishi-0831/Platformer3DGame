#pragma once
#include "Core/ISystem.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Core/Entity.h"
#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

namespace mtgb
{
	struct ShadowParams
	{
		ShadowParams();
		Vector4 casterPos;
		float softness;
		float padding[3];
	};
	/// <summary>
	/// 丸影を描画するシステム
	/// </summary>
	class ShadowSettings : public ISystem
	{
	  public:
		ShadowSettings();
		// ISystem を介して継承されました
		void Initialize() override;
		void Update() override;
		void SetCaster(EntityId _id);
		void SetCB();

	  private:
		ShadowParams params;
	};
} // namespace mtgb
