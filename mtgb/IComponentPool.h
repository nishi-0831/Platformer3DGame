#pragma once
#include "cmtgb.h"
#include "IComponentMemento.h"
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
		virtual IComponentMemento* SaveToMemento(EntityId _entityId) = 0;
		virtual IComponentMemento* Deserialize(EntityId _entityId, const nlohmann::json& _json) = 0;
		virtual void Copy(EntityId _dest, EntityId _src) = 0;

	protected:

		// キー : Componentの型情報, 値 : プール内のインデックス
		using ComponentIndexMap = std::unordered_map<std::type_index, size_t>;
		// キー : EntityId, 値 : ComponentIndexMap
		void RegisterCurrentScene(const std::function<void()>& _onMove);
	};
}
