#pragma once
#include "ISystem.h"
#include "Core/Component/ComponentPool.h"
#include "Components/RigidBody/RigidBody.h"

namespace mtgb
{
	class RigidBody;

	class RigidBodyCP : public ComponentPool<RigidBody, RigidBodyCP>
	{
	  public:
		RigidBodyCP();
		RigidBodyCP(const RigidBodyCP&) = default;
		~RigidBodyCP();

		void Update() override;
	};
} // namespace mtgb
