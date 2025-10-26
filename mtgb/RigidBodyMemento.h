#pragma once
#include "IComponentMemento.h"
#include "RigidBody.h"
#include "Vector3.h"

namespace mtgb
{
	/*class RigidBodyMemento : public IComponentMemento
	{
		friend RigidBody;
	public:
		explicit RigidBodyMemento(EntityId _entityId, const Vector3& _velocity)
			: entityId_{ _entityId }
			, velocity_{ _velocity }
		{
		}

		std::type_index GetComponentType() const override
		{
			return std::type_index(typeid(class RigidBody));
		}

		EntityId GetEntityId() const override { return entityId_; }

	private:
		EntityId entityId_;
		Vector3 velocity_;
	};*/
}
