#pragma once
#include "IComponent.h"
#include "ColliderCP.h"
#include <set>
#include <DirectXCollision.h>
#include "Vector3.h"


namespace mtgb
{
	class ColliderCP;
	class Transform;

	class Collider : public IComponent<ColliderCP, Collider>
	{
		friend ColliderCP;

	public:

		enum ColliderType : uint8_t
		{
			TYPE_SPHERE,  // ��(���S����̈�苗��)
			TYPE_CAPSULE,  // �J�v�Z��(��������̈�苗��)
			TYPE_AABB,	  // �����s���E�{�b�N�X(�e���ɕ��s�ȕ�)
		};

		// �Փ˔�������邩�ۂ��̃^�O
		enum class ColliderTag
		{
			GAME_OBJECT, 
			STAGE,
			STAGE_BOUNDARY, // �X�e�[�W�̋��E�A�͈͊O�̃R���C�_�[���Ӗ�����^�O
		};

	public:
		using IComponent<ColliderCP, Collider>::IComponent;
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
		ColliderTag GetColliderTag() const { return colliderTag_; }
	public:
		ColliderType type_;  // �����蔻��̌`

		// �ÓI�ȁATransform�s�v�ȃR���C�_�[�p�̃t���O
		bool isStatic_;

		//union
		//{
		//	struct
		//	{
		//		Vector3 offset_;  // �I�t�Z�b�g
		//		float radius_ = 1.0f;  // ���a
		//	} sphere_;
		//	struct
		//	{
		//		Vector3 center_;  // AABB�̒��S
		//		Vector3 extents_; // AABB�̃T�C�Y�̔���
		//	} aabb_;
		//	struct
		//	{
		//		// �J�v�Z���̃f�[�^
		//	} capsule_;
		//};

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
		ColliderTag colliderTag_;
		static FBXModelHandle hSphereModel_;
		static FBXModelHandle hBoxModel_;
	};
}
