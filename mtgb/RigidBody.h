#pragma once
#include "ReflectionMacro.h"
#include "RigidBody.generated.h"
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
	MT_COMPONENT()
	class RigidBody : public IComponent<RigidBodyCP,RigidBody>
	{
	public:
		MT_GENERATED_BODY()
		friend RigidBodyCP;

		RigidBody(const EntityId _entityId);
		~RigidBody();
		inline RigidBody& operator=(const RigidBody& _other)
		{
			if (&_other == this)
			{
				return *this;
			}

			this->velocity_ = _other.velocity_;
			this->onHit_ = _other.onHit_;
			this->isNeedUpdate_ = _other.isNeedUpdate_;
			this->useGravity_ = _other.useGravity_;
			this->pTransform_ = _other.pTransform_;

			return *this;
		}

		void UpdateVelocity();
		void OnGround();
		void OnPostRestore() {};
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
		MT_PROPERTY()
		bool isNeedUpdate_;
		MT_PROPERTY()
		Vector3 velocity_;
		MT_PROPERTY()
		bool useGravity_;
		MT_PROPERTY()
		bool isGround_;
		MT_PROPERTY()
		bool isKinematic_;
	private:

		std::function<void(const EntityId)> onHit_;
		std::function<void(const EntityId)> onStay_;
		std::function<void(const EntityId)> onExit_;
		Transform* pTransform_;  // TODO: 危ないTransform
	};

}
