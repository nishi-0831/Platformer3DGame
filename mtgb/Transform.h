#pragma once
#include "IComponent.h"
#include "Vector3.h"
#include "Vector3Ref.h"
#include "Quaternion.h"
#include "Matrix4x4.h"
#include "cmtgb.h"
#include "TransformCP.h"
#include "TransformCore.h"

namespace mtgb
{
	using EntityId = int64_t;

	class TransformCP;
	class TransformMemento;

	class Transform : public IComponent<mtgb::TransformCP, Transform>, public TransformCore
	{
		friend TransformCP;

	public:
		using IComponent<mtgb::TransformCP, Transform>::IComponent;
		
		//Transform();
		~Transform();
		inline Transform& operator=(const Transform& _other)
		{
			if (&_other == this)
			{
				return *this;
			}

			this->parent = _other.parent;
			this->position = _other.position;
			this->scale = _other.scale;
			this->rotate = _other.rotate;

			this->matrixTranslate_ = _other.matrixTranslate_;
			this->matrixRotate_ = _other.matrixRotate_;
			this->matrixScale_ = _other.matrixScale_;
			this->matrixWorld_ = _other.matrixWorld_;

//			massert(false && "Transform�̃R�s�[����");
			return *this;
		}

		/// <summary>
		/// �v�Z����
		/// </summary>
		void Compute();

		/// <summary>
		/// ���[���h�s��𐶐�����
		/// </summary>
		/// <param name="_pMatrix">�s��̃|�C���^�n��</param>
		void GenerateWorldMatrix(Matrix4x4* _pMatrix) const;
		/// <summary>
		/// ���[���h��]�s��𐶐�����
		/// </summary>
		/// <param name="_pMatrix">�s��̃|�C���^�n��</param>
		void GenerateWorldRotationMatrix(Matrix4x4* _pMatrix) const;
		/// <summary>
		/// �e�̍s��𐶐�����
		/// </summary>
		/// <param name="_pMatrix">�s��̃|�C���^�n��</param>
		void GenerateParentMatrix(Matrix4x4* _pMatrix) const;
		/// <summary>
		/// �e�̉�]�s��𐶐�����
		/// </summary>
		/// <param name="_pMatrix">�s��̃|�C���^�n��</param>
		void GenerateParentRotationMatrix(Matrix4x4* _pMatrix) const;

		/// <summary>
		/// �e��Transform���擾
		/// </summary>
		/// <returns>Transform�̃|�C���^</returns>
		Transform* GetParent() const;
		/// <summary>
		/// �e��Transform��ݒ�
		/// </summary>
		/// <param name="_entityId">�e�ɂ���G���e�B�e�B���ʎq</param>
		void SetParent(const EntityId _entityId) { parent = _entityId; }

		/// <summary>
		/// ��]����
		/// </summary>
		/// <param name="_rotate">�I�C���[�p(���W�A��)</param>
		void Rotation(const Vector3& _rotate);
		/// <summary>
		/// ���x�N�g���ŉ�]����
		/// </summary>
		/// <param name="_axis">���̃x�N�g��</param>
		/// <param name="_angle">��]�p�x(���W�A��)</param>
		void Rotation(const Vector3& _axis, const float _angle);

		/// <summary>
		/// ������̃��[���h�x�N�g��
		/// </summary>
		/// <returns>���[���h��]�s�񂪂�����ꂽ�x�N�g��</returns>
		Vector3 Up() const { return Vector3::Up() * matrixWorldRot_; }
		/// <summary>
		/// �������̃��[���h�x�N�g��
		/// </summary>
		/// <returns>���[���h��]�s�񂪂�����ꂽ�x�N�g��</returns>
		Vector3 Down() const { return Vector3::Down() * matrixWorldRot_; }
		/// <summary>
		/// �������̃��[���h�x�N�g��
		/// </summary>
		/// <returns>���[���h��]�s�񂪂�����ꂽ�x�N�g��</returns>
		Vector3 Left() const { return Vector3::Left() * matrixWorldRot_; }
		/// <summary>
		/// �E�����̃��[���h�x�N�g��
		/// </summary>
		/// <returns>���[���h��]�s�񂪂�����ꂽ�x�N�g��</returns>
		Vector3 Right() const { return Vector3::Right() * matrixWorldRot_; }
		/// <summary>
		/// ������̃��[���h�x�N�g��
		/// </summary>
		/// <returns>���[���h��]�s�񂪂�����ꂽ�x�N�g��</returns>
		Vector3 Back() const { return Vector3::Back() * matrixWorldRot_; }
		/// <summary>
		/// �O�����̃��[���h�x�N�g��
		/// </summary>
		/// <returns>���[���h��]�s�񂪂�����ꂽ�x�N�g��</returns>
		Vector3 Forward() const;

		/// <summary>
		/// ���[���h���W���擾
		/// </summary>
		/// <returns>���[���h���W�̃x�N�g��</returns>
		Vector3 GetWorldPosition() const { return Vector3::Zero() * matrixWorld_; }
		/// <summary>
		/// ���[���h��]���擾
		/// </summary>
		/// <returns>���[���h��]�̎l����</returns>
		Quaternion GetWorldRotate() const;

		TransformMemento* SaveToMemento() const;

		void RestoreFromMemento(const TransformMemento& _memento);
	private:
		/// <summary>
		/// �v�Z�p�������g�̃��[���h�s��𐶐�����
		/// </summary>
		/// <param name="_pMatrix">�s��̃|�C���^�n��</param>
		void GenerateWorldMatrixSelf(Matrix4x4* _pMatrix) const;
		/// <summary>
		/// �v�Z�p�������g�̃��[���h��]�s��𐶐�����
		/// </summary>
		/// <param name="_pMatrix">�s��̃|�C���^�n��</param>
		void GenerateWorldRotMatrixSelf(Matrix4x4* _pMatrix) const;

	public://private:
		Matrix4x4 matrixTranslate_{};         // �v�Z���ꂽ�ړ��s��
		Matrix4x4 matrixRotate_{};            // �v�Z���ꂽ��]�s��
		Matrix4x4 matrixScale_{};             // �v�Z���ꂽ�g�k�s��
		Matrix4x4 matrixWorld_{};             // �v�Z���ꂽ���[���h�s��
		Matrix4x4 matrixWorldRot_{};          // �v�Z���ꂽ���[���h��]�s��
	};
}
