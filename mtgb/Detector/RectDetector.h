#pragma once

#include <vector>
#include <string>
#include <functional>
#include "WindowContext/WindowContext.h"
#include "Math/RectF.h"
#include "RectContainsInfo.h"
#include "Core/GameObject/GameObjectTag.h"
#include "IDetector.h"
#include "DetectorConfigs.h"
#include "Graphics/UIParams.h"
namespace mtgb
{
	/// <summary>
	/// 矩形内の対象検出システム
	/// </summary>
	class RectDetector : public IDetector
	{
	  public:
		RectDetector() = default;
		RectDetector(const RectDetectorConfig& _config);
		RectDetector(RectDetectorConfig&& _config);
		~RectDetector() = default;

		/// <summary>
		/// <para> 検出を実行、結果を取得</para>
		/// <para> 事前に割り当てられた設定を使用</para>
		/// </summary>
		void UpdateDetection() override;

		/// <summary>
		/// <para> 検出を実行、結果を取得</para>
		/// <para> 引数の設定を使って検出</para>
		/// <param name="_config"> 検出に使用する設定</param>
		void UpdateDetection(RectDetectorConfig& _config);

		/// <summary>
		/// <para> 引数の設定を割り当て、検出を実行</para>
		/// </summary>
		/// <param name="_config"> 割り当て、検出に使用する設定</param>
		void UpdateAndSetDetection(RectDetectorConfig& _config);

		/// <summary>
		/// <para> 引数の設定を割り当て、検出を実行</para>
		/// </summary>
		/// <param name="_config"> 割り当て、検出に使用する設定</param>
		void UpdateAndSetDetection(RectDetectorConfig&& _config);

		RectF GetDetectionArea() const override;

		/// <summary>
		/// <para> カメラからターゲットに視線が通っているか</para>
		/// <para> !!!detectedTargets内で総当たりをしているので非効率的。ゲームが重くなったら真っ先に改善!!!</para>
		/// </summary>
		/// <param name="_cameraPos">カメラ</param>
		/// <param name="_targetInfo">ターゲット</param>
		/// <returns></returns>
		bool IsLineOfSight(const Vector3& _cameraPos, const ScreenCoordContainsInfo& _targetInfo);

		RectDetectorConfig config;
	};
} // namespace mtgb