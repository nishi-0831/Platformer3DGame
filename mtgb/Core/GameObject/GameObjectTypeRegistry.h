#pragma once
#include <typeindex>
#include <unordered_map>
#include <string>
#include <optional>
#include <span>
#include <tuple>
#include "Core/ISystem.h"
#include "StringComparators.h"
namespace mtgb
{
	/// <summary>
	/// 指定された名前のゲームオブジェクトが存在しなかった場合のデフォルト名
	/// </summary>
	static const char* DEFAULT_GAMEOBJECT_NAME { "Unknown" };
	/// <summary>
	/// ステージエディタ経由で生成するゲームオブジェクトのメタデータをまとめた構造体
	/// </summary>
	struct SpawnObjectDesc
	{
		// カテゴリー名。例えば"Enemy"なら、"Enemy/Hoge"
		std::string category;
		// エディタ上の表示名
		std::string displayName;
		// ツールチップに表示する説明文
		std::string tooltip;
		// ゲームオブジェクトのクラス名
		std::string typeName { DEFAULT_GAMEOBJECT_NAME };
		// 表示順番。降順
		int priority { 0 };
	};
	class GameObjectTypeRegistry : public ISystem
	{
	  public:
		void Initialize() override;
		void Update() override;

		/// <summary>
		/// 型を登録し、名前と紐付ける
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="_typeName"></param>
		template <typename T> void Register(const SpawnObjectDesc& _objDesc)
		{
			size_t size = spawnObjectDescs_.size();
			typeToDescIdx_.emplace(typeid(T), size);
			spawnObjectDescs_.push_back(_objDesc);
			nameToType_.emplace(std::string(_objDesc.typeName), typeid(T));

			// 降順で並べる
			priorityOrder_.push_back(std::make_pair(_objDesc.priority, size));
			std::sort(priorityOrder_.begin(), priorityOrder_.end(), std::greater<std::pair<int, size_t>>());
		}

		/// <summary>
		/// 型情報から型名を取得
		/// </summary>
		/// <param name="_typeIndex"></param>
		/// <returns></returns>
		std::string GetNameFromType(std::type_index _typeIndex) const;
		/// <summary>
		/// 型名からメタデータを取得
		/// </summary>
		/// <param name="_typeName"></param>
		/// <returns></returns>
		const SpawnObjectDesc& GetSpawnObjectDesc(std::string_view _typeName) const;
		/// <summary>
		/// 登録されている全ての型の名前を返す
		/// </summary>
		/// <returns></returns>
		std::vector<std::string> GetRegisteredNames() const;
		/// <summary>
		/// 登録されている全てのゲームオブジェクトのメタデータを返す
		/// </summary>
		/// <returns></returns>
		std::span<const SpawnObjectDesc> GetSpawnObjectDescs() const;
		/// <summary>
		/// ゲームオブジェクトの表示順番を返す。
		/// </summary>
		/// <returns>表示順番で並べられたインデックス配列</returns>
		std::vector<size_t> GetObjectPriorityOrder() const;
		/// <summary>
		/// 型名から型情報を取得
		/// </summary>
		/// <param name="_typeName"></param>
		/// <returns></returns>
		std::optional<std::type_index> GetTypeFromName(std::string_view _typeName) const
		{
			auto it = nameToType_.find(_typeName);
			if (it == nameToType_.end())
				return std::nullopt;
			return it->second;
		}

		/// <summary>
		/// 型が登録されているか確認
		/// </summary>
		/// <param name="_typeIndex"></param>
		/// <returns></returns>
		bool IsRegistered(std::type_index _typeIndex) const
		{
			return typeToDescIdx_.find(_typeIndex) != typeToDescIdx_.end();
		}
		/// <summary>
		/// 型が登録されているか確認
		/// </summary>
		/// <param name="_typeName"></param>
		/// <returns></returns>
		bool IsRegistered(std::string_view _typeName);

	  private:
		std::unordered_map<std::type_index, size_t> typeToDescIdx_;
		std::vector<SpawnObjectDesc> spawnObjectDescs_;
		std::unordered_map<std::string, std::type_index, TransparentStringHash, TransparentStringEq> nameToType_;
		/// <summary>
		/// ステージエディタで表示するゲームオブジェクトの順番
		/// </summary>
		std::vector<std::pair<int, size_t>> priorityOrder_;
	};
} // namespace mtgb
