#pragma once
#include "IComponentMemento.h"
#include "Collider.h"
#include "Vector3.h"

namespace mtgb
{
	/*class ColliderMemento : public IComponentMemento
	{
		friend Collider;
	public:
		explicit ColliderMemento(EntityId _entityId, ColliderType _type, bool _isStatic, ColliderTag _colliderTag)
			: entityId_{ _entityId }
			, type_{ _type }
			, isStatic_{ _isStatic }
			, colliderTag_{ _colliderTag }
		{
		}

		std::type_index GetComponentType() const override
		{
			return std::type_index(typeid(class Collider));
		}

		EntityId GetEntityId() const override { return entityId_; }

	private:
		EntityId entityId_;
		ColliderType type_;
		bool isStatic_;
		ColliderTag colliderTag_;
	};*/
}