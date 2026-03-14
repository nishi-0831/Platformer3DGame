#pragma once

#include <string>

#include "ShowType.h"
#include "Entity.h"

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
		// ImGuiShowableの寿命
		enum class Scope
		{
			// ゲームが終了する際に削除される
			GLOBAL,
			// シーンが遷移する際に削除される
			SCENE
		};

		ImGuiShowable(EntityId _entityId);
		ImGuiShowable(ShowType _showType, EntityId _entityId);
		ImGuiShowable(const std::string& _name, ShowType _showType, EntityId _entityId, Scope _scope);
		virtual ~ImGuiShowable();

		/// <summary>
		/// 指定した表示キューに積まれる
		/// </summary>
		virtual void ShowImGui();

	  protected:
		EntityId targetEntityId_; // Entityの場合はIdを渡すこと
		std::string displayName_; // 表示される際の名前
		ShowType show_;			  // 表示したいImGuiWindow
		Scope scope_;			  // ImGuiShowableの寿命
	};

} // namespace mtgb