#pragma once
#include "ISystem.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Entity.h"
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
		ComPtr<ID3D11Buffer> pShadowCB_;
	};
} // namespace mtgb
