#pragma once
#include "IComponentMemento.h"
#include "Transform.h"

namespace mtgb
{
	/*class TransformMemento : public IComponentMemento
	{
		friend Transform;
	public :
		explicit TransformMemento(
			EntityId _entityId,
			const Vector3& _position,
			const Vector3& _scale,
			const Quaternion& _rotate
		)
			: entityId_{_entityId}
			, position_{_position}
			, scale_{_scale}
			, rotate_{_rotate}
		{

		}

		std::type_index GetComponentType() const override
		{
			return std::type_index(typeid(class Transform));
		}

		EntityId GetEntityId() const override { return entityId_; }
		
	private:
		EntityId entityId_;
		Vector3 position_;
		Vector3 scale_;
		Quaternion rotate_;
	};*/
}
