#pragma once
#include <list>
#include <functional>
#include <map>
#include <set>
#include <cstdint>
#include "Core/ISystem.h"

using TimerHandle = void*;

namespace mtgb
{
	class Timer : public ISystem
	{
	  private:
		struct QUEUE_ELEMENT
		{
			std::function<void()> on;
			float timeLeft;
		};

	  public:
		/// <summary>
		/// 指定した秒数後に呼ばれるコールバックを設定
		/// </summary>
		/// <param name="_time"></param>
		/// <param name="_callback"></param>
		/// <returns></returns>
		static TimerHandle AddAram(float _time, const std::function<void()>& _callback);
		/// <summary>
		/// 指定した間隔(秒単位)で呼ばれるコールバックを設定
		/// </summary>
		/// <param name="_time">呼び出す間隔</param>
		/// <param name="_callback"></param>
		/// <param name="_firstCall">コールバックを設定したフレームに即座に呼び出すか否か</param>
		/// <returns></returns>
		static TimerHandle AddInterval(float _time, const std::function<void()>& _callback, bool _firstCall = false);
		static void Remove(TimerHandle _hTimer);

		/// <summary>
		/// タイマーキューをすべてクリアする
		/// </summary>
		static void Clear();

	  public:
		Timer();
		~Timer();

		void Initialize() override;
		void Update() override;
		void Release() override;

		void EnqueueTimer(QUEUE_ELEMENT* _pElement);

	  private:
		static Timer& Instance()
		{
			return *pInstance_;
		}

	  private:
		static Timer* pInstance_;
		std::list<QUEUE_ELEMENT*> pTimerQueue_;				 // タイマーキュー
		std::map<QUEUE_ELEMENT*, float> pReenqueueElements_; // 使いまわしする要素のタイマー情報
		std::set<QUEUE_ELEMENT*> toErase_;					 // 消す予定のキュー
	};
} // namespace mtgb
