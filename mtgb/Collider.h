#pragma once
#include "ReflectionMacro.h"
#include "Collider.generated.h"
#include "ISerializableObject.h"
#include "StatefulComponent.h"
#include "ColliderState.h"
#include "IComponentMemento.h"
#include <set>
#include <DirectXCollision.h>
#include "Vector3.h"
#include "ColliderType.h"
#include "ColliderCP.h"
#include <optional>

namespace mtgb
{
	struct IntersectInfo
	{
		Vector3 closest;
		Vector3 push;
	};
	class ColliderCP;
	class Transform;
	MT_COMPONENT()
	class Collider :  public IComponent<ColliderCP,Collider> , public ISerializableObject
	{

	public:
		MT_GENERATED_BODY()

		friend ColliderCP;


		// 衝突判定をするか否かのタグ

	public:
		
		Collider(EntityId _entityId);
		Collider(EntityId _entityId, ColliderTag _colliderTag);
		~Collider();
		inline Collider& operator= (const Collider& _other)
		{
			if (&_other == this)
			{
				return *this;
			}
			return *this;
		}

		bool IsHit(const Collider& _other) const;
		/// <summary>
		/// レイとの交差判定
		/// </summary>
		/// <param name="_origin">原点</param>
		/// <param name="_dir">方向(内部で正規化される)</param>
		/// <param name="dist">
		/// <para> レイの原点からコライダーとの最初の交点までの距離を格納。</para>
		/// <para> レイの原点が球の内側の場合は、球を出る点までの距離</para>
		/// </param>
		/// <returns></returns>
		bool IsHit(const DirectX::BoundingSphere& _sphere,const Vector3& _origin, const Vector3& _dir, float* dist);
		bool IsHit(const DirectX::BoundingBox& _aabb, const Vector3& _origin, const Vector3& _dir, float* dist);
		bool IsHit(const Vector3& _origin, const Vector3& _dir, float* dist);
		bool IsHit(const Vector3& _center, float _radius) const;

		void Draw() const;

		// BoundingSphereを初期化
		void UpdateBoundingData();
		
		void SetCenter(const Vector3& _center);
		void SetExtents(const Vector3& _extents);
		void SetRadius(float _radius);
		ColliderTag GetColliderTag() const { return colliderTag_; }
		
		static std::optional<IntersectInfo> Intersect(const DirectX::BoundingSphere& _sphere, const DirectX::BoundingBox& _aabb);
		void Push(const Collider& _other);
	public:
		void OnPostRestore() override;

		std::set<Collider*> onColliders_;
		std::set<Collider*> onColldiersPrev_;
		
	private:
		void UpdateBoundingSphere();
		void UpdateBoundingBox();
		union
		{
			DirectX::BoundingSphere computeSphere_;
			DirectX::BoundingBox computeBox_;
		};

		Transform* pTransform_;  // TODO: 危ないTransform
		//ColliderTag colliderTag;
		static FBXModelHandle hSphereModel_;
		static FBXModelHandle hBoxModel_;
		MT_PROPERTY()
		// 当たり判定の形
		ColliderType colliderType_;
		MT_PROPERTY()
		// 静的な、Transform不要なコライダー用のフラグ
		bool isStatic_;
		MT_PROPERTY()
		ColliderTag colliderTag_;
		MT_PROPERTY()
		Vector3 center_;
		MT_PROPERTY()
		float radius_;
		MT_PROPERTY()
		Vector3 extents_;
	};
}
