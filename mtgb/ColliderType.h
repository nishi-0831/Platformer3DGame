#pragma once
enum class ColliderType
{
	TYPE_SPHERE,  // 球(中心からの一定距離)
	TYPE_CAPSULE,  // カプセル(線分からの一定距離)
	TYPE_AABB,	  // 軸並行境界ボックス(各軸に平行な辺)
};

enum class ColliderTag
{
	GAME_OBJECT,
	STAGE,
	STAGE_BOUNDARY, // ステージの境界、範囲外のコライダーを意味するタグ
};