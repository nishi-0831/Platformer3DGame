#pragma once
#include <string>
#include "Vector3.h"
#include "Quaternion.h"
#include "GameObjectLayer.h"
#include "GameObjectTag.h"

namespace mtgb
{
	/// <summary>
	/// <para>�Q�[���I�u�W�F�N�g�𐶐�����Ƃ��̐ݒ�</para>
	/// <para>���̂܂܍\���̂��g�p�\�����AGameObjectBuilder��ʂ����Ƃ𐄏�</para>
	/// </summary>
	struct GAME_OBJECT_DESC
	{
		std::string name{};
		Vector3 position{ Vector3::Zero() };
		Vector3 scale{ Vector3::One() };
		Quaternion rotate{ Quaternion::Identity() };
		GameObjectLayerFlag layerFlag{ GameObjectLayerFlag::New().BeginEdit().OnAll().EndEdit() };
		GameObjectTag tag{ GameObjectTag::Untagged };
		bool isActive{ true };
		bool callUpdate{ true };
		bool callDraw{ true };
		// padding 10byte
	};

	// MEMO: �\���̂̂��̂܂܏������ł́A���Ԃ��Œ肳��Ă��邪�A
	//     : Builder���g�����ƂōD���ȃ^�C�~���O�ŔC�ӂ̒l���Z�b�g�ł���

	/// <summary>
	/// <para>GAME_OBJECT_DESC�̃r���_�[�N���X</para>
	/// <para>�����o�̃Z�b�^�֐��𗧂đ����ɌĂяo���A�ŏI�I��Build�֐���GAME_OBJECT_DESC���\�z</para>
	/// </summary>
	class GameObjectBuilder
	{
	public:
		/// <summary>
		/// <para>�ȉ��̂悤�Ɏg��</para>
		/// <para>GameObjectBuilder().SetName("Name").Set...().Build()</para>
		/// </summary>
		GameObjectBuilder()
		{}
		~GameObjectBuilder()
		{}

		GameObjectBuilder& SetName(const std::string& _name)              { desc_.name = _name;             return *this; }
		GameObjectBuilder& SetPosition(const Vector3& _position)          { desc_.position = _position;     return *this; }
		GameObjectBuilder& SetRotate(const Quaternion& _rotate)           { desc_.rotate = _rotate;         return *this; }
		GameObjectBuilder& SetScale(const Vector3& _scale)                { desc_.scale = _scale;           return *this; }
		GameObjectBuilder& SetIsActive(const bool _isActive)              { desc_.isActive = _isActive;     return *this; }
		GameObjectBuilder& SetCallUpdate(const bool _callUpdate)          { desc_.callUpdate = _callUpdate; return *this; }
		GameObjectBuilder& SetCallDraw(const bool _callDraw)              { desc_.callDraw = _callDraw;     return *this; }
		GameObjectBuilder& SetLayerFlag(const GameObjectLayerFlag& _flag) { desc_.layerFlag = _flag;        return *this; }
		GameObjectBuilder& SetTag(const GameObjectTag& _tag) { desc_.tag = _tag; return *this; }

		/// <summary>
		/// �ݒ���r���h����
		/// </summary>
		/// <returns>��������GAME_OBJECT_DESC�̃R���X�g�Q�ƃ|�C���^</returns>
		const GAME_OBJECT_DESC& Build() const { return desc_; }
	private:
		GAME_OBJECT_DESC desc_;
	};
}
