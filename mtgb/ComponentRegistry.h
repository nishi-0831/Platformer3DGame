#pragma once
#include "ISystem.h"
#include <unordered_map>
#include <typeindex>
#include <optional>
#include <set>
#include "nlohmann/json.hpp"
namespace mtgb
{
	class ComponentRegistry : public ISystem
	{
	public:

		void Initialize() override;
		void Update() override;

		ComponentRegistry();

		void RegisterComponentIndex(EntityId _entityId, const std::type_index& _typeIndex, size_t _componentIndex);

		/// <summary>
		/// コンポーネントプール内のインデックスを取得
		/// </summary>
		/// <param name="_entityId"></param>
		/// <param name="_componentTypeIndex"></param>
		/// <returns></returns>
		std::optional<size_t> GetComponentIndex(EntityId _entityId, const std::type_index& _componentTypeIndex);

		/// <summary>
		/// EntityIdが登録されているComponentの型情報を記録する
		/// </summary>
		/// <param name="_entityId"></param>
		/// <param name="_componentTypeIndex"></param>
		void RegisterComponent(EntityId _entityId, const std::type_index& _componentTypeIndex);

		/// <summary>
		/// Componentの型情報に対応する文字列を記録する
		/// </summary>
		/// <param name="_key"></param>
		/// <param name="_typeIndex"></param>
		void RegisterComponentKey(const std::string& _key, const std::type_index& _typeIndex);
		/// <summary>
		/// Componentに対応するComponentPoolの型情報を登録する
		/// </summary>
		/// <param name="_comp"></param>
		/// <param name="_pool"></param>
		void RegisterComponentPoolType(const std::type_index& _comp, const std::type_index& _pool);

		void UnRegisterComponent(EntityId _entityId, const std::type_index& _typeIndex);
		void ClearEntity(EntityId _entityId);

		void ClearComponentIndices(EntityId _entityId);
		/// <summary>
		/// Componentに対応するComponentPoolの型情報を返す
		/// </summary>
		/// <param name="_typeIndex">Componentの型情報</param>
		/// <returns></returns>
		std::optional<std::type_index> GetComponentPoolType(const std::type_index& _componentTypeIndex);

		std::optional<std::type_index> GetComponentPoolType(const std::string& _componentName);

		/// <summary>
		/// EntityIdが登録されているComponentの型情報を返す
		/// </summary>
		/// <param name="_entityId"></param>
		/// <returns></returns>
		std::optional<std::reference_wrapper<const std::set<std::type_index>>> GetComponentTypes(EntityId _entityId);
		std::optional<std::set<std::type_index>> GetComponentTypes(const nlohmann::json& _json);
	private:
		// Componentの型情報 → ComponentPoolの型情報（component type_index → pool type_index）
		std::unordered_map<std::type_index, std::type_index> componentTypeToPoolTypeMap_;
		// EntityId が持つ Component 型情報の集合
		std::unordered_map<EntityId, std::set<std::type_index>> entityComponents_;
		// 文字列（名前／キー） → Componentの型情報（string → component type_index）
		std::unordered_map<std::string,std::type_index> componentNameToTypeMap_;
		// キー : Componentの型情報, 値 : プール内のインデックス
		using ComponentIndexMap = std::unordered_map<std::type_index, size_t>;
		// キー : EntityId, 値 : ComponentIndexMap
		std::unordered_map<EntityId, ComponentIndexMap> componentIndices_;
	};
}