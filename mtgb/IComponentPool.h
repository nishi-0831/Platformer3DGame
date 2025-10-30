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
	/// コンポーネントプールのインタフェース
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
		/// コンポーネントプール内のインデックスを取得
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
		/// Componentに対応するComponentPoolの型情報を返す
		/// </summary>
		/// <param name="_typeIndex">Componentの型情報</param>
		/// <returns></returns>
		static std::optional<std::type_index> GetComponentPoolType(const std::type_index& _typeIndex);

		/// <summary>
		/// EntityIdが登録されているComponentの型情報を返す
		/// </summary>
		/// <param name="_entityId"></param>
		/// <returns></returns>
		static std::optional<std::reference_wrapper<const std::set<std::type_index>>> GetComponentTypes(EntityId _entityId);

	protected:
		// Component→ComponentPoolへのtype_indexの辞書
		static std::unordered_map<std::type_index, std::type_index> componentPoolTypeMap_;
		static std::unordered_map<EntityId, std::set<std::type_index>> entityComponents_;

		// キー : Componentの型情報, 値 : プール内のインデックス
		using ComponentIndexMap = std::unordered_map<std::type_index, size_t>;
		// キー : EntityId, 値 : ComponentIndexMap
		static std::unordered_map<EntityId, ComponentIndexMap> componentIndices_;
		void RegisterCurrentScene(const std::function<void()>& _onMove);
	};
}
