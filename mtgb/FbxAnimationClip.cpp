#include "stdafx.h"
#include <fbxsdk.h>

#include "FbxAnimationClip.h"

mtgb::FbxAnimationClip::FbxAnimationClip(FbxAnimStack* _pAnimStack)
	: name{""}
	, startFrame{0}
	, endFrame{0}
	, pAnimStack{_pAnimStack}
{
	if (_pAnimStack == nullptr)
		return;

	// クリップ名を取得
	name = _pAnimStack->GetName();

	// FbxAnimStackからアニメーション範囲を取得
	FbxTimeSpan timeSpan = _pAnimStack->GetLocalTimeSpan();
	FbxTime startTime	 = timeSpan.GetStart();
	FbxTime endTime		 = timeSpan.GetStop();

	startFrame = static_cast<int>(startTime.GetFrameCount(FbxTime::EMode::eFrames60));
	endFrame   = static_cast<int>(endTime.GetFrameCount(FbxTime::EMode::eFrames60));
}
