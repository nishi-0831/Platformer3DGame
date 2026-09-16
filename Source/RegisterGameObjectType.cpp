#include "stdafx.h"
#include "RegisterGameObjectType.h"
#include "Player.h"
#include "ScoreItem.h"
#include "Goal.h"
#include "GameOverZone.h"
#include "MovingFloor.h"
#include "PatrolChargerEnemy.h"
#include "SeaPlane.h"
#include "CircularSaw.h"
#include "RespawnManager.h"
#include "Button.h"
#include "Slider.h"
#include "RangedAttackEnemy.h"
void mtgb::RegisterGameObjectType(mtgb::IGameObjectRegistrar* _pGameObjectRegistrar)
{
	// ↓コピペ用
	/*
	mtgb::SpawnObjectDesc {
		.category	 = "",
		.displayName = "",
		.tooltip	 = "",
		.typeName	 = ""
	}
	*/
	_pGameObjectRegistrar->Register<MovingFloor>(mtgb::SpawnObjectDesc { .category	  = "Gimmick",
																		 .displayName = "MovingFloor",
																		 .tooltip	  = "2点間を移動する床",
																		 .typeName	  = "MovingFloor",
																		 .priority	  = 9 });
	_pGameObjectRegistrar->Register<Player>(
		mtgb::SpawnObjectDesc { .category	 = "",
								.displayName = "Player",
								.tooltip	 = "",
								.typeName	 = "Player",
								.priority	 = 1 },
		true
	);
	_pGameObjectRegistrar->Register<ScoreItem>(mtgb::SpawnObjectDesc { .category	= "",
																	   .displayName = "Ruby",
																	   .tooltip		= "",
																	   .typeName	= "ScoreItem",
																	   .priority	= 2 });
	_pGameObjectRegistrar->Register<Goal>(
		mtgb::SpawnObjectDesc { .category	 = "",
								.displayName = "Goal",
								.tooltip	 = "",
								.typeName	 = "Goal",
								.priority	 = 1 },
		true
	);
	_pGameObjectRegistrar->Register<GameOverZone>(mtgb::SpawnObjectDesc {
		.category	 = "",
		.displayName = "DamagePlane",
		.tooltip	 = "触れたら落下判定となり、ダメージを与えてリスポーンポイントに戻す",
		.typeName	 = "GameOverZone",
		.priority	 = 2 });
	_pGameObjectRegistrar->Register<PatrolChargerEnemy>(mtgb::SpawnObjectDesc {
		.category	 = "Enemy",
		.displayName = "Golem",
		.tooltip	 = "プレイヤーを見つけると突撃してくる敵",
		.typeName	 = "PatrolChargerEnemy",
		.priority	 = 7 });
	_pGameObjectRegistrar->Register<RangedAttackEnemy>(mtgb::SpawnObjectDesc { .category	= "Enemy",
																			   .displayName = "Rich",
																			   .tooltip	 = "近づくと球を撃ってくる敵",
																			   .typeName = "RangedAttackEnemy",
																			   .priority = 7 });
	_pGameObjectRegistrar->Register<SeaPlane>(mtgb::SpawnObjectDesc { .category	   = "",
																	  .displayName = "Sea",
																	  .tooltip	   = "",
																	  .typeName	   = "SeaPlane",
																	  .priority	   = 0 });
	_pGameObjectRegistrar->Register<CircularSaw>(mtgb::SpawnObjectDesc { .category	  = "Gimmick",
																		 .displayName = "Saw",
																		 .tooltip	  = "回転するのこぎり",
																		 .typeName	  = "CircularSaw",
																		 .priority	  = 8 });
	_pGameObjectRegistrar->Register<RespawnPoint>(mtgb::SpawnObjectDesc { .category	   = "",
																		  .displayName = "RespawnPoint",
																		  .tooltip = "触れると、落下時の復活場所になる",
																		  .typeName = "RespawnPoint",
																		  .priority = 6 });
	_pGameObjectRegistrar->Register<Button>(
		mtgb::SpawnObjectDesc { .category = "UI", .displayName = "Button", .tooltip = "", .typeName = "Button" }
	);
	_pGameObjectRegistrar->Register<Slider>(
		mtgb::SpawnObjectDesc { .category = "UI", .displayName = "Slider", .tooltip = "", .typeName = "Slider" }
	);
}
