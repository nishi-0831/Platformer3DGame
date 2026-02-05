#include "stdafx.h"
#include <fbxsdk.h>
#include "FbxAnimationController.h"
#include "GameTime.h"
#include "Debug.h"
#include "EventManager.h"
#include "Fbx.h"
#include <filesystem>
#include <fstream>
mtgb::FbxAnimationController::FbxAnimationController(fbxsdk::FbxScene* _fbxScene, std::string_view _fileName)
	: pCurrentClip_{nullptr}
	, currentFrame_{0.0f}
	, animationSpeed_{1.0f}
	, isPlaying_{false}
	, isLooping_{false}
	, isFinished_{false}
	, pFbxScene_{_fbxScene}
	, fileName_{_fileName}
{
	int animStackCount = _fbxScene->GetSrcObjectCount<FbxAnimStack>();

	for (int i = 0; i < animStackCount; i++)
	{
		FbxAnimStack* pAnimStack = _fbxScene->GetSrcObject<FbxAnimStack>(i);
		if (pAnimStack == nullptr)
			continue;

		// アニメーションクリップを作成
		FbxAnimationClip animClip(pAnimStack);

		// コントローラに登録
		RegisterAnimationClip(animClip);
	}
	
	std::filesystem::path path(fileName_);
	path.replace_extension();
	path.concat(".Event.json");
	path.make_preferred();
	if (std::filesystem::exists(path))
	{
		std::fstream input(path);
		if (input.fail())
		{
			LOGIMGUI("failed to read {}.Event.json", path.c_str());
			return;
		}
		
		nlohmann::json json;
		try
		{
			input >> json;
		}
		catch (const nlohmann::json::parse_error& e)
		{
			const char* errMsg = e.what();
			LOGIMGUI("parse error : {}", errMsg);
			return;
		}

		for (nlohmann::json::iterator itr = json.begin(); itr != json.end(); itr++)
		{
			nlohmann::json evtJson = *itr;
			AnimationEvent animEvt;
			animEvt = evtJson;
			events_.push_back(std::move(animEvt));
		}
	}
}

mtgb::FbxAnimationController::~FbxAnimationController()
{
}

void mtgb::FbxAnimationController::RegisterAnimationClip(const FbxAnimationClip& _animClip)
{
	clipMap_.emplace(_animClip.name, _animClip);
}

void mtgb::FbxAnimationController::RegisterAnimationClip(FbxAnimationClip&& _animClip)
{
	clipMap_.emplace(_animClip.name, std::move(_animClip));
}

int mtgb::FbxAnimationController::GetCurrentFrame() const
{
	return static_cast<int>(currentFrame_);
}

void mtgb::FbxAnimationController::PlayAnimation(std::string_view _clipName, bool _isLooping)
{
	auto itr = clipMap_.find(_clipName);
	if (itr == clipMap_.end())
	{
		LOGIMGUI("Animation clip not found: %s", _clipName.data());
		return;
	}
	pCurrentClip_ = &(itr->second);
	currentFrame_ = pCurrentClip_->startFrame;
	isLooping_	  = _isLooping;
	isPlaying_	  = true;
	isFinished_	  = false;
	pFbxScene_->SetCurrentAnimationStack(pCurrentClip_->pAnimStack);
}

void mtgb::FbxAnimationController::UpdateFrame()
{
	if (pCurrentClip_ == nullptr)
		return;

	if (isPlaying_ == false)
		return;

	currentFrame_ += animationSpeed_;

	int startFrame = pCurrentClip_->startFrame;
	int endFrame   = pCurrentClip_->endFrame;

	if (currentFrame_ < startFrame)
	{
		if (isLooping_)
		{
			currentFrame_ = static_cast<float>(endFrame);
		}
		else
		{
			currentFrame_ = static_cast<float>(startFrame);
			isPlaying_	  = false;
			isFinished_	  = true;
		}
	}
	else if (currentFrame_ > endFrame)
	{
		if (isLooping_)
		{
			currentFrame_ = static_cast<float>(startFrame);
		}
		else
		{
			currentFrame_ = static_cast<float>(endFrame);
			isPlaying_	  = false;
			isFinished_	  = true;
		}
	}
	CheckEvents();
}

void mtgb::FbxAnimationController::PauseAnimation()
{
	isPlaying_ = false;
}

void mtgb::FbxAnimationController::ResumeAnimation()
{
	isPlaying_ = true;
}

void mtgb::FbxAnimationController::SetAnimationSpeed(float _animSpeed)
{
	animationSpeed_ = _animSpeed;
}

bool mtgb::FbxAnimationController::IsFinishedAnimation()
{
	return isFinished_;
}

void mtgb::FbxAnimationController::CheckEvents()
{
	for (const AnimationEvent& event : events_)
	{
		// 現在のフレームが、イベント対象のフレームの範囲内か
		if (currentFrame_ >= event.startFrame && currentFrame_ <= event.endFrame)
		{
			using iterator					  = decltype(eventCallbackMap_)::iterator;
			// イベント名に該当するコールバック関数を取得
			std::pair<iterator, iterator> ret = eventCallbackMap_.equal_range(event.eventName);

			if (ret.first == eventCallbackMap_.end())
				return;

			for (iterator itr = ret.first; itr != ret.second; itr++)
			{
				auto& func = itr->second;
				func(event);
			}
		}
	}
}

void mtgb::FbxAnimationController::SetEventCallback(
	std::string_view _eventName,
	std::function<void(const AnimationEvent&)> _callback
)
{
	eventCallbackMap_.emplace(_eventName, _callback);
}
