#pragma once
#include <unordered_map>
#include <string_view>
#include <vector>
#include <functional>
#include "FbxAnimationClip.h"
#include "StringComparators.h"
#include "Handlers.h"
#include "AnimationEvent.h"

namespace fbxsdk
{
	class FbxScene;
}
namespace mtgb
{
	class FbxAnimationController
	{
	  public:
		FbxAnimationController() = delete;
		explicit FbxAnimationController(fbxsdk::FbxScene* _fbxScene, std::string_view _fileName);
		~FbxAnimationController();

		/// <summary>
		/// アニメーションクリップを登録
		/// </summary>
		/// <param name="_animClip"></param>
		void RegisterAnimationClip(const FbxAnimationClip& _animClip);

		/// <summary>
		/// アニメーションクリップを登録
		/// </summary>
		/// <param name="_animClip"></param>
		void RegisterAnimationClip(FbxAnimationClip&& _animClip);

		/// <summary>
		/// 現在のアニメーションフレームを取得
		/// </summary>
		/// <returns></returns>
		int GetCurrentFrame() const;

		/// <summary>
		/// <para> アニメーションを再生 </para>
		/// <para> 名前に対応するクリップが無い場合は何もしない </para>
		/// </summary>
		/// <param name="_clipName">再生するクリップ名</param>
		/// <param name="_isLooping">ループするか</param>
		void PlayAnimation(std::string_view _clipName, bool _isLooping = true);

		/// <summary>
		/// フレームを更新
		/// </summary>
		void UpdateFrame();

		/// <summary>
		/// アニメーションを一時停止
		/// </summary>
		void PauseAnimation();

		/// <summary>
		/// アニメーションを再開
		/// </summary>
		void ResumeAnimation();
		/// <summary>
		/// アニメーションの再生速度を設定
		/// </summary>
		/// <param name="_animSpeed"></param>
		void SetAnimationSpeed(float _animSpeed);
		/// <summary>
		/// <para> アニメーションが終了したか </para>
		/// <para> ループしている状態では終了と判定しない </para>
		/// </summary>
		/// <returns></returns>
		bool IsFinishedAnimation();
		/// <summary>
		/// 現在のフレームに該当するイベントがあるか確認
		/// ある場合は、イベントの通知を行う
		/// </summary>
		void CheckEvents();

		using AnimationEventCallback = std::function<void(const AnimationEvent&)>;

		template <typename Func>
			requires std::is_convertible_v<Func, AnimationEventCallback>
		void SetEventCallback(std::string_view _eventName, Func&& _callback)
		{
			eventCallbackMap_.emplace(_eventName, std::forward<Func>(_callback));
		}

	  private:
		std::unordered_map<std::string, FbxAnimationClip, TransparentStringHash, TransparentStringEq> clipMap_;
		std::unordered_multimap<std::string, AnimationEventCallback, TransparentStringHash, TransparentStringEq>
			eventCallbackMap_;
		FbxAnimationClip* pCurrentClip_; // 現在再生中のクリップ
		float currentFrame_;			 // 現在のフレーム
		float animationSpeed_;			 // 再生速度(倍率)
		bool isPlaying_;				 // 再生中か
		bool isLooping_;				 // ループをするか
		bool isFinished_;				 // 終了しているか
		fbxsdk::FbxScene* pFbxScene_;
		std::string fileName_;
		std::vector<AnimationEvent> events_;
	};
} // namespace mtgb
