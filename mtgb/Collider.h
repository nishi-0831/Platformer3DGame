#pragma once
#include "ReflectionMacro.h"
#include "ISerializableObject.h"
#include "IComponentMemento.h"
#include "IComponent.h"
#include <unordered_set>
#include <DirectXCollision.h>
#include "Vector3.h"
#include "ColliderType.h"
#include "ColliderCP.h"
#include <concepts>
#include "CallbackConcepts.h"
#include "Collider.generated.h"

namespace mtgb
{

	class ColliderCP;
	class Transform;

	class [[MT_COMPONENT()]] Collider : public IComponent<ColliderCP, Collider>, public ISerializableObject
	{

	  public:
		MT_GENERATED_BODY()

		friend ColliderCP;
		using IComponent::IComponent;

		// 衝突判定をするか否かのタグ

	  public:
		Collider(EntityId _entityId);
		Collider(EntityId _entityId, ColliderTag _colliderTag);
		~Collider();
		Collider& operator=(const Collider& _other);

		bool IsHit(const Collider& _other) const;

		/// <summary>
		/// <para> レイとの交差判定 </para>
		/// <para> 自身のコライダーに合わせて判定を行う </para>
		/// </summary>
		/// <param name="_origin">レイの原点</param>
		/// <param name="_dir">レイの方向</param>
		/// <param name="_dist">レイの原点とコライダーの距離を格納</param>
		/// <returns>交差している場合はtrue、していない場合はfalse</returns>
		bool IsHit(const Vector3& _origin, const Vector3& _dir, float* _dist) const;
		bool IsHit(const Vector3& _center, float _radius) const;

		void ForEachCollisionEnter(const EntityCallback& _func);
		void ForEachCollisionStay(const EntityCallback& _func);
		void ForEachCollisionExit(const EntityCallback& _func);

		void Draw() const;

		// BoundingSphereを初期化
		void UpdateBoundingData();

		void SetCenter(const Vector3& _center);
		void SetExtents(const Vector3& _extents);
		void SetRadius(float _radius);
		Vector3 GetCenter() const;
		Vector3 GetExtents() const;
		float GetRadius() const;
		inline ColliderTag GetColliderTag() const
		{
			return colliderTag_;
		}

		void OnPostRestore() override;
		void Reset() override;

		[[MT_PROPERTY()]]
		// 当たり判定の形
		ColliderType colliderType_;
		[[MT_PROPERTY()]]
		// 静的な、Transform不要なコライダー用のフラグ
		bool isStatic_;
		[[MT_PROPERTY()]]
		ColliderTag colliderTag_;
		[[MT_PROPERTY()]]
		bool isTrigger_;

	  private:
		std::unordered_set<Collider*> onColliders_;
		std::unordered_set<Collider*> onCollidersPrev_;
		void Push(const Collider& _other);
		void HandleCollision(const Collider& _other);
		void UpdateBoundingSphere();
		void UpdateBoundingBox();
		union
		{
			DirectX::BoundingSphere computeSphere_;
			DirectX::BoundingBox computeBox_;
			DirectX::BoundingOrientedBox computeOBB_;
		};

		Transform* pTransform_;

		static FBXModelHandle hSphereModel_;
		static FBXModelHandle hBoxModel_;
		[[MT_PROPERTY()]]
		Vector3 center_;
		[[MT_PROPERTY()]]
		float radius_;
		[[MT_PROPERTY()]]
		Vector3 extents_;
	};
} // namespace mtgb
