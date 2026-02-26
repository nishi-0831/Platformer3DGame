#pragma once
#include "ReflectionMacro.h"
#include "ISerializableObject.h"
#include "IComponent.h"
#include "IComponentMemento.h"

#include "Vector3.h"
#include "RigidBodyCP.h"
#include <functional>
#include <DirectXCollision.h>

#include "RigidBody.generated.h"
namespace mtgb
{

	class Transform;
	class RigidBodyCP;
	class [[MT_COMPONENT()]] RigidBody : public IComponent<RigidBodyCP, RigidBody>, public ISerializableObject
	{
	  public:
		MT_GENERATED_BODY()
		friend RigidBodyCP;
		using IComponent::IComponent;

		RigidBody(const EntityId _entityId);
		~RigidBody();
		RigidBody& operator=(const RigidBody& _other);

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
		static float GetGravity();
	  public:
		bool isNeedUpdate_;
		Vector3 velocity_;
		[[MT_PROPERTY()]]
		bool useGravity_;
		bool isGround_;
		[[MT_PROPERTY()]]
		bool isKinematic_;

	  private:
		std::function<void(const EntityId)> onHit_;
		std::function<void(const EntityId)> onStay_;
		std::function<void(const EntityId)> onExit_;
		Transform* pTransform_; // TODO: 危ないTransform
	};

} // namespace mtgb
