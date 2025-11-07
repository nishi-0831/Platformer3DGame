#pragma once
#include "StatefulComponent.h"
#include "IComponentMemento.h"

#include "Vector3.h"
#include "RigidBodyData.h"
#include "RigidBodyCP.h"
#include <functional>
#include <DirectXCollision.h>

namespace mtgb
{

	class Transform;
	class RigidBodyCP;
	class RigidBody : public StatefulComponent<RigidBody, RigidBodyCP, RigidBodyData>
	{
	public:
		using StatefulComponent<RigidBody, RigidBodyCP, RigidBodyData>::StatefulComponent;
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
		/*void OnCollisionStay();
		void OnCollisionExit();*/
		bool IsJumping();
		static Vector3 GetPushAmount(const DirectX::BoundingSphere& _sphere, const DirectX::BoundingBox& _aabb);
	public:
		//Vector3 velocity;  // 速度

	private:
		//bool isNeedUpdate;
		std::function<void(const EntityId)> onHit_;
		Transform* pTransform_;  // TODO: 危ないTransform
	};

	using RigidBodyMemento = ComponentMemento<RigidBody, RigidBodyData>;
}
