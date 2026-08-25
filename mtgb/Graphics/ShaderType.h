#pragma once
#include "cmtgb.h"

namespace mtgb
{
	/// <summary>
	/// シェーダタイプ
	/// </summary>
	enum struct ShaderType : int8_t
	{
		FIGURE,
		SPRITE2_D,
		FBX_PARTS,
		FBX_PARTS_SKIN,
		UNLIT3_D,
		DEBUG3_D,
		GROUND,
		TRAIL,
		TERRAIN,
		BOX3_D,
		SEA,
		OUTLINE,
		MAX,
	};
} // namespace mtgb
