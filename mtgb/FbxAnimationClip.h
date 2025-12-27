#pragma once
#include <string>

namespace fbxsdk
{
	class FbxAnimStack;
}
namespace mtgb
{
	struct FbxAnimationClip
	{
		FbxAnimationClip(FbxAnimStack* _pAnimStack);
		std::string name;
		int startFrame;
		int endFrame;
		FbxAnimStack* pAnimStack;
	};
}