#pragma once
#include "cmtgb.h"
#include <functional>
#include <set>
#include <unordered_map>
#include <optional>
#include <typeindex>
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

		static void RegisterComponent(EntityId _entityId, const std::type_index& _typeIndex);
		static void UnRegisterComponent(EntityId _entityId, const std::type_index& _typeIndex);
		static void ClearEntity(EntityId _entityId);

		static std::optional<std::reference_wrapper<const std::set<std::type_index>>> GetComponentTypes(EntityId _entityId);

	protected:
		static std::unordered_map<EntityId, std::set<std::type_index>> entityComponents_;
		void RegisterCurrentScene(const std::function<void()>& _onMove);
	};
}
