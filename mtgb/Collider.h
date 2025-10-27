#pragma once
#include "StatefulComponent.h"
#include "ColliderData.h"
#include "IComponentMemento.h"
#include <set>
#include <DirectXCollision.h>
#include "Vector3.h"
#include "ColliderType.h"
#include "ColliderCP.h"


namespace mtgb
{
	class ColliderCP;
	class Transform;
	class Collider : public ColliderData, public StatefulComponent<Collider, ColliderCP, ColliderData, ComponentMemento<Collider, ColliderData>>
	{

	public:
		using StatefulComponent<Collider, ColliderCP, ColliderData, ComponentMemento<Collider, ColliderData>>::StatefulComponent;
		friend ColliderCP;


		// �Փ˔�������邩�ۂ��̃^�O

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
		/// ���C�Ƃ̌�������
		/// </summary>
		/// <param name="_origin">���_</param>
		/// <param name="_dir">����(�����Ő��K�������)</param>
		/// <param name="dist">
		/// <para> ���C�̌��_����R���C�_�[�Ƃ̍ŏ��̌�_�܂ł̋������i�[�B</para>
		/// <para> ���C�̌��_�����̓����̏ꍇ�́A�����o��_�܂ł̋���</para>
		/// </param>
		/// <returns></returns>
		bool IsHit(const Vector3& _origin, const Vector3& _dir, float* dist);
		bool IsHit(const Vector3& _center, float _radius) const;
		void Draw() const;

		// BoundingSphere��������
		void UpdateBoundingData();
		
		void SetCenter(const Vector3& _center);
		void SetExtents(const Vector3& _extents);
		void SetRadius(float _radius);
		ColliderTag GetColliderTag() const { return colliderTag; }
		
	public:
		void OnPostRestore() override;

		std::set<Collider*> onColliders_;

		
	private:
		void UpdateBoundingSphere();
		void UpdateBoundingBox();
		union
		{
			DirectX::BoundingSphere computeSphere_;
			DirectX::BoundingBox computeBox_;
		};

		Transform* pTransform_;  // TODO: ��Ȃ�Transform
		//ColliderTag colliderTag;
		static FBXModelHandle hSphereModel_;
		static FBXModelHandle hBoxModel_;
	};

	using ColliderMemento = ComponentMemento<Collider, ColliderData>;
}
