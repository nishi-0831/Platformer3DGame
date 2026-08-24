#pragma once
#include "Editor/ReflectionMacro.h"
#include "Editor/ISerializableObject.h"
#include "Core/Component/IComponentMemento.h"
#include "Core/Component/IComponent.h"
#include <unordered_set>
#include <DirectXCollision.h>
#include "Math/Vector3.h"
#include "Collision/ColliderType.h"
#include "Components/Collider/ColliderCP.h"
#include <concepts>
#include "Utility/CallbackConcepts.h"
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
		/// <param name="_maxDistance">レイキャストの有効距離</param>
		/// <param name="_info">レイキャストの結果を格納する構造体</param>
		/// <returns>交差している場合はtrue、していない場合はfalse</returns>
		bool IsHit(const Vector3& _origin, const Vector3& _dir, float _maxDistance, Intersection::RaycastInfo* _info)
			const;
		bool IsHit(const Vector3& _center, float _radius) const;
		/// <summary>
		/// 接触した瞬間のコライダーにコールバックを呼ぶ
		/// </summary>
		/// <param name="_func"></param>
		void ForEachCollisionEnter(const EntityCallback& _func);
		/// <summary>
		/// 接触中のコライダーにコールバックを呼ぶ
		/// </summary>
		/// <param name="_func"></param>
		void ForEachCollisionStay(const EntityCallback& _func);
		/// <summary>
		/// 接触して離れた瞬間のコライダーにコールバックを呼ぶ
		/// </summary>
		/// <param name="_func"></param>
		void ForEachCollisionExit(const EntityCallback& _func);

		void Draw() const;

		// BoundingSphereを初期化
		void UpdateBoundingData();
		/// <summary>
		/// コライダーの中心を設定する。
		/// 接続されているエンティティの座標のオフセットとなる。
		/// </summary>
		/// <param name="_center"></param>
		void SetCenter(const Vector3& _center);
		/// <summary>
		/// コライダー(AABB、OBB)の、一辺の半分の長さを設定する
		/// </summary>
		/// <param name="_extents"></param>
		void SetExtents(const Vector3& _extents);
		/// <summary>
		/// コライダー(球)の半径を返す
		/// </summary>
		/// <param name="_radius"></param>
		void SetRadius(float _radius);
		/// <summary>
		/// コライダーの中心(接続されているエンティティの座標のオフセット)を返す
		/// </summary>
		/// <returns></returns>
		Vector3 GetCenter() const;
		/// <summary>
		/// コライダー(AABB、OBB)の、一辺の半分の長さを返す
		/// </summary>
		/// <returns></returns>
		Vector3 GetExtents() const;
		/// <summary>
		/// コライダー(球)の半径を返す
		/// </summary>
		/// <returns></returns>
		float GetRadius() const;
		inline ColliderTag GetColliderTag() const
		{
			return colliderTag_;
		}

		void OnPostRestore() override;
		void Reset() override;
		void OnChangeEntityId() override;

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
		/// <summary>
		/// コライダー同士で押し出し合う
		/// </summary>
		/// <param name="_other"></param>
		void Push(const Collider& _other);
		/// <summary>
		/// 押し出しの判定をする。
		/// どちらかがisKinematicやisTriggerの場合は押し出しをしない
		/// </summary>
		/// <param name="_other"></param>
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

		[[MT_PROPERTY()]]
		Vector3 center_;
		[[MT_PROPERTY()]]
		float radius_;
		[[MT_PROPERTY()]]
		Vector3 extents_;

		// コライダー(球)の3Dモデルのハンドル。デバッグ表示用
		static FBXModelHandle hSphereModel_;
		// コライダー(AABB、OBB)の3Dモデルのハンドル。デバッグ表示用
		static FBXModelHandle hBoxModel_;
	};
} // namespace mtgb
