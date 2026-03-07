#pragma once
#include <typeindex>
#include <unordered_map>
#include <string>
#include <optional>
#include "ISystem.h"
#include "StringComparators.h"
namespace mtgb
{
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
		template <typename T> void Register(std::string_view _typeName)
		{
			typeToName_.emplace(typeid(T), std::string(_typeName));
			nameToType_.emplace(std::string(_typeName), typeid(T));
		}

		/// <summary>
		/// 型情報から型名を取得
		/// </summary>
		/// <param name="_typeIndex"></param>
		/// <returns></returns>
		std::string GetNameFromType(std::type_index _typeIndex) const
		{
			auto it = typeToName_.find(_typeIndex);
			if (it == typeToName_.end())
				return "Unknown";
			return it->second;
		}

		/// <summary>
		/// 登録されている全ての型の名前を返す
		/// </summary>
		/// <returns></returns>
		std::vector<std::string> GetRegisteredNames() const;

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
			return typeToName_.find(_typeIndex) != typeToName_.end();
		}
		bool IsRegistered(std::string_view _typeName);

	  private:
		std::unordered_map<std::type_index, std::string> typeToName_;
		std::unordered_map<std::string, std::type_index, TransparentStringHash, TransparentStringEq> nameToType_;
	};
} // namespace mtgb
