#pragma once
#include <concepts>
#include "Entity.h"
#include <type_traits>

namespace mtgb
{
	// ============================================================
	// �R���|�[�l���g�֘A�̃R���Z�v�g
	// ============================================================

	/// <summary>
	/// T �� Component �C���^�[�t�F�[�X���������Ă���
	/// �iGet(EntityId) �ÓI���\�b�h�����j
	/// </summary>
	/*template <typename T>
	concept ComponentT = requires(T _com, EntityId _id)
	{
		T::Get(_id);
	};*/

	/// <summary>
	/// T �͒P���ȃ��������C�A�E�g�̃R���|�[�l���g��ԍ\����
	/// </summary>
	template<typename T>
	concept ComponentDataT =
		std::is_standard_layout_v<T>;

	// ============================================================
	// �p���֌W�̃R���Z�v�g
	// ============================================================

	/// <summary>
	/// TDerived �� TData �����J�p�����Ă���
	/// �� static_cast<TData&>(derived) �����S
	/// 
	/// CRTP�Ȃǂ� TDerived���s���S�^�̏�Ԃŕ]�������ꍇ�����邽�߁A
	/// �|�C���^��static_cast�\���Ŕ��肷��
	/// </summary>
	template <typename TDerived, typename TData>
	concept DerivedFromDataT = 
		std::derived_from<TDerived, TData> || 
		requires(TDerived * _p) { static_cast<TData*>(_p); };

	/// <summary>
	/// TComponent �� TData �����J�p�����Ă���
	/// �� ComponentMemento<TComponent, TData> �̎g�p�����S
	/// </summary>
	template <typename TComponent, typename TData>
	concept ComponentFromDataT =
		std::derived_from<TComponent, TData>&&
		ComponentDataT<TData>;

	// ============================================================
	// Memento �֘A�̃R���Z�v�g
	// ============================================================

	class IComponentMemento;

	template <typename T>
	concept MementoT =
		std::derived_from<T, IComponentMemento>;

	/// <summary>
	/// T �� M ���������Ή����Ă���
	/// </summary>
	template <typename T, typename M>
	concept ComponentWithMementoT =
		// M �� IComponentMemento ���p�����Ă���imemento���̗v���j
		std::derived_from<M, IComponentMemento>;
		
}