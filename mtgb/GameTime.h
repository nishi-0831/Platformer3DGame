#pragma once
#include "ISystem.h"

typedef union _LARGE_INTEGER LARGE_INTEGER;

namespace mtgb
{
	/// <summary>
	/// ゲーム内時間のシステムクラス
	/// </summary>
	class Time : public ISystem
	{
	  public:
		Time();
		~Time();

		void Initialize() override;
		void Update() override;

		static inline double DeltaTime()
		{
			return deltaTime_;
		}
		static inline float DeltaTimeF()
		{
			return static_cast<float>(deltaTime_);
		}
		/// <summary>
		/// FPSの目標値
		/// </summary>
		/// <returns></returns>
		static inline double TargetFrameRate()
		{
			return targetFrameRate_;
		}
		/// <summary>
		/// FPSの目標値
		/// </summary>
		/// <returns></returns>
		static inline float TargetFrameRateF()
		{
			return static_cast<float>(targetFrameRate_);
		}
		/// <summary>
		/// 指定したフレーム数だけゲームの更新を待機する
		/// </summary>
		/// <param name="_frame"></param>
		static inline void WaitFrame(unsigned int _frame)
		{
			waitFrame_ = _frame;
		};

		/// <summary>
		/// デルタタイムを安定させる。
		/// ステージの読み込みなど時間のかかる処理を行った後に呼び出す
		/// </summary>
		static inline void StabilizeDeltaTime()
		{
			waitFrame_ = DELTA_TIME_WARMUP_FRAMES;
		};

	  private:
		/// <summary>
		/// デルタタイムを安定させるために待機するフレーム数
		/// </summary>
		static constexpr unsigned int DELTA_TIME_WARMUP_FRAMES = 2;
		static double deltaTime_; // フレーム間時間 (秒)
		static constexpr double MAX_DELTA_TIME { 0.1 };
		static double targetFrameRate_;

		LARGE_INTEGER current_;			// 現在のCPU時間 (マイクロ秒)
		LARGE_INTEGER previous_;		// 前回のCPU時間 (マイクロ秒)
		LARGE_INTEGER frequency_;		// 周波数
		static unsigned int waitFrame_; // 更新を待機するフレーム数
		int currentFps_;				// 現在のFPS
		int frameCount_;				// 更新間の経過フレーム数
		double elapsed_;				// 前回更新からの経過時間
	};
} // namespace mtgb
