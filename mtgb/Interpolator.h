#pragma once
#include "Vector3.h"

namespace mtgb
{
	struct Interpolator
	{
		Interpolator();
		/// <summary>
		/// 補間の進行状況を更新する
		/// 経過時間を進め、端まで到達した場合は移動方向を反転する
		/// </summary>
		void UpdateProgress();
		/// <summary>
		/// 現在の進行状況で補間された値を返す
		/// </summary>
		/// <returns></returns>
		Vector3 Evaluate();
		/// <summary>
		/// 端点を設定する
		/// </summary>
		/// <param name="_start">始点</param>
		/// <param name="_end">終点</param>
		void SetEndpoints(const Vector3& _start, const Vector3& _end);

		// 進行方向
		float dir;

		// 経過時間
		float elapsed;

		// 補間にかかる時間(秒)
		float duration;

		// 始点
		Vector3 start;

		// 終点
		Vector3 end;
	};
}