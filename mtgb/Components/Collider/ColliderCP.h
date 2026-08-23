#pragma once
#include "Core/ISystem.h"
#include "Core/Component/ComponentPool.h"
#include "Components/Collider/Collider.h"
#include "Intersection.h"

#include <vector>
namespace mtgb
{

	class Collider;
	class GameObject;
	class ColliderCP : public ComponentPool<Collider, ColliderCP>
	{
	  public:
		ColliderCP();
		~ColliderCP();

		void Start() override;
		void Update() override;

		void Draw();
		/// <summary>
		/// 全エンティティとレイキャストをして、最も近いエンティティのIDを返す
		/// </summary>
		/// <param name="_origin">レイの原点</param>
		/// <param name="_dir">レイの方向</param>
		/// <param name="_maxDistance">レイが衝突を検知する最大距離</param>
		/// <param name="_info">レイキャストの情報が格納される構造体</param>
		/// <param name="_tag">判定するコライダーのタグ</param>
		/// <param name="_ignoreEntityId">判定をしないエンティティのID</param>
		/// <returns>最も近いエンティティのID。衝突していない場合INVALID_ENTITY</returns>
		EntityId RayCastHitAll(
			const Vector3& _origin,
			const Vector3& _dir,
			float _maxDistance,
			Intersection::RaycastInfo* _info,
			ColliderTag _tag		 = ColliderTag::GAME_OBJECT,
			EntityId _ignoreEntityId = INVALID_ENTITY
		);
		/// <summary>
		/// 指定したエンティティとレイキャストをする
		/// </summary>
		/// <param name="_origin">レイの原点</param>
		/// <param name="_dir">レイの方向</param>
		/// <param name="_maxDistance">レイが衝突を検知する最大距離</param>
		/// <param name="_info">レイキャストの情報が格納される構造体</param>
		/// <param name="_entityId">レイキャストをするエンティティのID</param>
		/// <returns>衝突している場合true、していない場合false</returns>
		bool RayCastHit(
			const Vector3& _origin,
			const Vector3& _dir,
			float _maxDistance,
			Intersection::RaycastInfo* _info,
			EntityId _entityId
		);
		/// <summary>
		/// 指定した球と全てのEntityで当たり判定をとる
		/// </summary>
		/// <param name="_center">球の中心</param>
		/// <param name="_radius">球の半径</param>
		/// <param name="_entityIds">衝突しているEntityIdの配列</param>
		void IsHitAll(const Vector3& _center, float _radius, std::vector<EntityId>* _entityIds);
	};
} // namespace mtgb
