#pragma once
#include <unordered_map>
#include <typeindex>
#include <optional>
#include <set>
#include <nlohmann/json.hpp>
#include "Core/ISystem.h"
#include "cmtgb.h"
#include "Utility/StringComparators.h"
namespace mtgb
{
	/// <summary>
	/// Componentの型情報が登録されるクラス。
	/// <para>ComponentからComponentPoolの索引、</para>
	/// <para>EntityIdからComponentの索引ができる</para>
	/// </summary>
	class ComponentRegistry : public ISystem
	{
	  public:
		void Initialize() override;
		void Update() override;

		ComponentRegistry();
		/// <summary>
		/// EntityIdに対応するComponentPool内のインデックスを登録する
		/// </summary>
		/// <param name="_entityId">EntityId</param>
		/// <param name="_typeIndex">Componentのtype_index</param>
		/// <param name="_componentIndex">ComponentPool内のインデックス</param>
		void RegisterComponentIndex(EntityId _entityId, std::type_index _typeIndex, size_t _componentIndex);

		/// <summary>
		/// ComponentPool内のインデックスを取得
		/// </summary>
		/// <param name="_entityId"></param>
		/// <param name="_componentTypeIndex"></param>
		/// <returns></returns>
		std::optional<size_t> GetComponentIndex(EntityId _entityId, std::type_index _componentTypeIndex);

		/// <summary>
		/// EntityIdが登録されているComponentの型情報を記録する
		/// </summary>
		/// <param name="_entityId"></param>
		/// <param name="_componentTypeIndex"></param>
		void RegisterComponent(EntityId _entityId, std::type_index _componentTypeIndex);

		/// <summary>
		/// Componentの型情報に対応する文字列を記録する
		/// </summary>
		/// <param name="_key"></param>
		/// <param name="_typeIndex"></param>
		void RegisterComponentKey(const std::string& _key, std::type_index _typeIndex);
		/// <summary>
		/// Component対応するComponentPool型情報を登録する
		/// </summary>
		/// <param name="_comp"></param>
		/// <param name="_pool"></param>
		void RegisterComponentPoolType(std::type_index _comp, std::type_index _pool);
		/// <summary>
		/// EntityIdに対応するComponentPool内のインデックスを登録解除
		/// </summary>
		/// <param name="_entityId"></param>
		/// <param name="_typeIndex"></param>
		void UnRegisterComponent(EntityId _entityId, std::type_index _typeIndex);
		/// <summary>
		/// EntityIdに対応するコンポーネントを全て登録解除する
		/// </summary>
		/// <param name="_entityId"></param>
		void ClearEntity(EntityId _entityId);
		/// <summary>
		/// EntityIdに対応するComponentPoolのインデックスを全て削除する
		/// </summary>
		/// <param name="_entityId"></param>
		void ClearComponentIndices(EntityId _entityId);
		/// <summary>
		/// Component対応するComponentPool型情報を返す
		/// </summary>
		/// <param name="_typeIndex">コンポーネントの型情報</param>
		/// <returns></returns>
		std::optional<std::type_index> GetComponentPoolType(std::type_index _componentTypeIndex);
		/// <summary>
		/// Component対応するComponentPool型情報を返す
		/// </summary>
		/// <param name="_typeIndex">コンポーネントの名前</param>
		/// <returns></returns>
		std::optional<std::type_index> GetComponentPoolType(std::string_view _componentName);

		/// <summary>
		/// EntityIdが登録されているComponentPoolの型情報の配列を返す
		/// </summary>
		/// <param name="_entityId"></param>
		/// <returns></returns>
		std::optional<std::vector<std::type_index>> GetComponentPoolTypes(EntityId _entityId);
		/// <summary>
		/// EntityIdが登録されているComponentの型情報を返す
		/// </summary>
		/// <param name="_entityId"></param>
		/// <returns></returns>
		std::optional<std::reference_wrapper<const std::set<std::type_index>>> GetComponentTypes(EntityId _entityId);
		/// <summary>
		/// EntityIdが登録されているComponentの型情報を返す
		/// </summary>
		/// <param name="_entityId"></param>
		/// <returns></returns>
		std::optional<std::set<std::type_index>> GetComponentTypes(const nlohmann::json& _json);

	  private:
		// Componentの型情報 → ComponentPoolの型情報（component type_index → pool type_index）
		std::unordered_map<std::type_index, std::type_index> componentTypeToPoolTypeMap_;
		// EntityId が持つ Component 型情報の集合
		std::unordered_map<EntityId, std::set<std::type_index>> entityComponents_;
		// 文字列（名前／キー） → Componentの型情報（string → component type_index）
		std::unordered_map<std::string, std::type_index, TransparentStringHash, TransparentStringEq>
			componentNameToTypeMap_;
		// キー : Componentの型情報, 値 : プール内のインデックス
		using ComponentIndexMap = std::unordered_map<std::type_index, size_t>;
		// キー : EntityId, 値 : ComponentIndexMap
		std::unordered_map<EntityId, ComponentIndexMap> componentIndices_;
	};
} // namespace mtgb