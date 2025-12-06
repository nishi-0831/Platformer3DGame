#pragma once
#include "StatefulComponent.h"
#include "IComponentMemento.h"

#include "Vector3.h"
#include "RigidBodyState.h"
#include "RigidBodyCP.h"
#include <functional>
#include <DirectXCollision.h>

namespace mtgb
{

	class Transform;
	class RigidBodyCP;
	class RigidBody : public StatefulComponent<RigidBody, RigidBodyCP, RigidBodyState>
	{
	public:
		using StatefulComponent<RigidBody, RigidBodyCP, RigidBodyState>::StatefulComponent;
		friend RigidBodyCP;

		RigidBody(const EntityId _entityId);
		~RigidBody();
		inline RigidBody& operator=(const RigidBody& _other)
		{
			if (&_other == this)
			{
				return *this;
			}

			this->velocity = _other.velocity;
			this->onHit_ = _other.onHit_;
			this->isNeedUpdate = _other.isNeedUpdate;
			this->useGravity = _other.useGravity;
			this->pTransform_ = _other.pTransform_;

			return *this;
		}

		void UpdateVelocity();
		void OnGround();
		/// <summary>
		/// 当たったときのイベントコールバック
		/// </summary>
		/// <param name="onHit_">void(const EntityId)</param>
		void OnCollisionEnter(const std::function<void(const EntityId)>& _onHit);
		void OnCollisionStay(const std::function<void(const EntityId)>& _onHit);
		void OnCollisionExit(const std::function<void(const EntityId)>& _onExit);
		bool IsJumping();
		static Vector3 GetPushAmount(const DirectX::BoundingSphere& _sphere, const DirectX::BoundingBox& _aabb);
	public:

	private:
		std::function<void(const EntityId)> onHit_;
		std::function<void(const EntityId)> onStay_;
		std::function<void(const EntityId)> onExit_;
		Transform* pTransform_;  // TODO: 危ないTransform
	};

	using RigidBodyMemento = ComponentMemento<RigidBody, RigidBodyState>;
}
