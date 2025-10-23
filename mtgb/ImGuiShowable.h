#pragma once

#include <string>

#include "ISystem.h"
#include <any>
#include <unordered_map>
#include <typeindex>
#include "WindowContext.h"
#include "ShowType.h"
#include "Entity.h"
#include <string_view>
namespace mtgb
{
	/// <summary>
	/// 継承してShowImGuiをオーバライドして使う
	/// これを使わなくてもImGui表示はできるので、別の箇所に分離したいときなどにどうぞ
	/// </summary>
	class ImGuiShowable
	{
		friend class MTImGui;
	public:
		ImGuiShowable(EntityId _entityId = INVALID_ENTITY);
		ImGuiShowable(ShowType _showType, EntityId _entityId = INVALID_ENTITY);
		ImGuiShowable(const std::string& _name,ShowType _showType, EntityId _entityId = INVALID_ENTITY);
		virtual ~ImGuiShowable();

		/// <summary>
		/// 指定した表示キューに積まれる
		/// </summary>
		virtual void ShowImGui();
	protected:
		EntityId entityId_; // Entityの場合はIdを渡すこと
		std::string displayName_; // 表示される際の名前
		ShowType show_; // 表示したいImGuiWindow
	};
	
}