#pragma once
#include "cmtgb.h"
#include <functional>
#include <set>
#include <unordered_map>
#include <optional>
#include <typeindex>
#include <nlohmann/json.hpp>
namespace mtgb
{
	using EntityId = int64_t;

	/// <summary>
	/// �R���|�[�l���g�v�[���̃C���^�t�F�[�X
	/// </summary>
	class IComponentPool
	{
	public:
		IComponentPool();
		virtual ~IComponentPool();

		virtual void Remove(const EntityId _entityId) = 0;
		virtual nlohmann::json Serialize(EntityId _entityId) = 0;
		static void RegisterComponentIndex(EntityId _entityId, const std::type_index& _typeIndex,size_t _componentIndex);

		/// <summary>
		/// �R���|�[�l���g�v�[�����̃C���f�b�N�X���擾
		/// </summary>
		/// <param name="_entityId"></param>
		/// <param name="_typeIndex"></param>
		/// <returns></returns>
		static std::optional<size_t> GetComponentIndex(EntityId _entityId, const std::type_index& _typeIndex);

		static void RegisterComponent(EntityId _entityId, const std::type_index& _typeIndex);
		static void RegisterComponentPoolType(const std::type_index& _comp, const std::type_index& _pool);
		static void UnRegisterComponent(EntityId _entityId, const std::type_index& _typeIndex);
		static void ClearEntity(EntityId _entityId);

		static void ClearComponentIndices(EntityId _entityId);
		/// <summary>
		/// Component�ɑΉ�����ComponentPool�̌^����Ԃ�
		/// </summary>
		/// <param name="_typeIndex">Component�̌^���</param>
		/// <returns></returns>
		static std::optional<std::type_index> GetComponentPoolType(const std::type_index& _typeIndex);

		/// <summary>
		/// EntityId���o�^����Ă���Component�̌^����Ԃ�
		/// </summary>
		/// <param name="_entityId"></param>
		/// <returns></returns>
		static std::optional<std::reference_wrapper<const std::set<std::type_index>>> GetComponentTypes(EntityId _entityId);

	protected:
		// Component��ComponentPool�ւ�type_index�̎���
		static std::unordered_map<std::type_index, std::type_index> componentPoolTypeMap_;
		static std::unordered_map<EntityId, std::set<std::type_index>> entityComponents_;

		// �L�[ : Component�̌^���, �l : �v�[�����̃C���f�b�N�X
		using ComponentIndexMap = std::unordered_map<std::type_index, size_t>;
		// �L�[ : EntityId, �l : ComponentIndexMap
		static std::unordered_map<EntityId, ComponentIndexMap> componentIndices_;
		void RegisterCurrentScene(const std::function<void()>& _onMove);
	};
}
