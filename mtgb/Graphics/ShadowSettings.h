#pragma once
#include "Core/ISystem.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Core/Entity.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
using Microsoft::WRL::ComPtr;

namespace mtgb
{

	/// <summary>
	/// 丸影を描画するシステム
	/// </summary>
	class ShadowSettings : public ISystem
	{
	  public:
		ShadowSettings();
		void Initialize() override;
		void Update() override;
		void AddCaster(EntityId _id, float _radius = 1.0f);
		void SetCB();

	  private:
		/// 丸影描画用の構造体 ///

		/// <summary>
		/// オブジェクトが影を落とすためのパラメータ
		/// </summary>
		struct Caster
		{
			Caster(const Vector4& _pos, float _radius);
			Vector4 pos;
			float radius;
			Vector3 padding;
		};
		/// <summary>
		/// シェーダーに渡す構造体。
		/// シーン全体の影のパラメータを渡す
		/// </summary>
		struct ShadowParam
		{
			/// <summary>
			/// 無効な影のパラメータを返す。
			/// 影を描画しない場合に使う
			/// </summary>
			/// <returns></returns>
			static ShadowParam Disabled();
			std::vector<Caster> casters;
			int casterCount;
			Vector3 padding;
		};

		// シェーダーに渡す影パラメータ
		ShadowParam shadowParams_;
	};
} // namespace mtgb
