#pragma once
#include "ISystem.h"
#include "ComponentPool.h"
#include "Collider.h"
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
		EntityId RayCastHitAll(
			const Vector3& _origin,
			const Vector3& _dir,
			float _maxDistance,
			Intersection::RaycastInfo* _info,
			ColliderTag _tag		 = ColliderTag::GAME_OBJECT,
			EntityId _ignoreEntityId = INVALID_ENTITY
		);
		bool RayCastHit(
			const Vector3& _origin,
			const Vector3& _dir,
			float _maxDistance,
			Intersection::RaycastInfo* _info,
			EntityId _entityId
		);
		void IsHitAll(const Vector3& _center, float _radius, std::vector<EntityId>* _entityIds);
	};
} // namespace mtgb
