#pragma once
#include <functional>
#include <type_traits>
#include "GameObject.h"
#include "IGameObjectCreator.h"
#include "IGameObjectRegistrar.h"
#include "GameObjectTypeRegistry.h"
#include "StringComparators.h"
#include <unordered_map>
#include <string_view>
#include <unordered_set>
namespace mtgb
{
	/// <summary>
	/// ゲームオブジェクトをその型名から生成するクラス
	/// </summary>
	class GameObjectFactory : public IGameObjectCreator, public IGameObjectRegistrar
	{
	  public:
		GameObjectFactory() = default;

		/// <summary>
		/// 型名からゲームオブジェクトを生成する
		/// </summary>
		/// <param name="_typeName">生成したいゲームオブジェクトの型名</param>
		/// <returns>生成したゲームオブジェクトのポインタ。型名からの生成に失敗した場合にはnullptr</returns>
		GameObject* Create(std::string_view _typeName) const override;
		/// <summary>
		/// ゲームオブジェクトの生成関数を登録する
		/// </summary>
		/// <param name="_typeName">ゲームオブジェクトの型名</param>
		/// <param name="_creator">生成関数</param>
		/// <param name="_isUnique">ゲームシーン上に一意に存在するべき否か。trueの場合、一個しか生成不可能</param>
		void RegisterFactory(std::string_view _typeName, const CreateFunc& _creator, bool _isUnique) override;

	  private:
		/// <summary>
		/// ゲームオブジェクトの型名:作成関数
		/// </summary>
		std::unordered_map<std::string, CreateFunc, TransparentStringHash, TransparentStringEq> creators_;
		/// <summary>
		/// 一つのシーンに一つしか存在しないゲームオブジェクトの型名:作成関数
		/// </summary>
		std::unordered_set<std::string, TransparentStringHash, TransparentStringEq> uniqueGameObjectNames_;
	};
} // namespace mtgb