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

		/// <summary>
		/// コライダ同士の衝突に対するコールバック
		/// </summary>
		/// <param name="onHit_">void(const EntityId)</param>
		void OnCollisionEnter(const std::function<void(const EntityId)>& _onHit);
		/// <summary>
		/// コライダ同士が接触している際に呼ばれるコールバック
		/// </summary>
		/// <param name="_onStay"></param>
		void OnCollisionStay(const std::function<void(const EntityId)>& _onStay);
		/// <summary>
		/// コライダ同士が離れた時に呼ばれるコールバック
		/// </summary>
		/// <param name="_onExit"></param>
		void OnCollisionExit(const std::function<void(const EntityId)>& _onExit);
		/// <summary>
		/// ジャンプをしている状態か
		/// </summary>
		/// <returns>ジャンプをしているならtrue、していないならfalse</returns>
		bool IsJumping();
		/// <summary>
		/// 球とAABBの押し出し量を返す
		/// </summary>
		/// <param name="_sphere"></param>
		/// <param name="_aabb"></param>
		/// <returns>押し出し量のベクトル</returns>
		static Vector3 GetPushAmount(const DirectX::BoundingSphere& _sphere, const DirectX::BoundingBox& _aabb);
		/// <summary>
		/// 現在設定されている重力を返す
		/// </summary>
		/// <returns></returns>
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
