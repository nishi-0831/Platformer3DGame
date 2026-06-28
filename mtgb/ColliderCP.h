#pragma once
#include "ISystem.h"
#include "ComponentPool.h"
#include "Collider.h"

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
		EntityId RayCastHitAll(const Vector3& _origin, const Vector3& _dir, float* _dist,ColliderTag _tag = ColliderTag::GAME_OBJECT);
		bool RayCastHit(const Vector3& _origin, const Vector3& _dir, float* _dist, EntityId _entityId);
		void IsHitAll(const Vector3& _center, float _radius, std::vector<EntityId>* _entityIds);
	};
} // namespace mtgb
