// Sprite.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
#include <string>
// ============================================================================
// Spriteの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
struct SpriteState
{
};

// クラスの前方宣言
	namespace mtgb
	{
		class Sprite;
	}



	

using SpriteMemento = mtgb::ComponentMemento<mtgb::Sprite, SpriteState>;

// ============================================================================
// SpriteとSpriteMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_Sprite() \
	public: \
	using Memento = SpriteMemento; \
	SpriteMemento* SaveToMemento() \
	{ \
	OnPreSave(); \
		SpriteState state; \
		return new Memento(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const Memento& _memento) \
	{ \
		const SpriteState& state = _memento.GetState(); \
		OnPostRestore(); \
	} \
	\
	friend struct Sprite_Register; \
	friend void to_json(nlohmann::json& _j,const Sprite& _target) \
	{ \
	} \
	friend void from_json(const nlohmann::json& _j, Sprite& _target) \
	{ \
		_target.OnPostRestore(); \
	} \
	static std::string TypeName(){ return "Sprite" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui() \
	{ \
		static bool registered = false; \
		if (registered) return; \
		registered = true; \
		\
		RegisterShowFuncHolder::Set<Sprite>([]( Sprite* _target, const char* _name) \
			{ \
			}); \
		MTImGui::Instance().RegisterComponentViewer<Sprite>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_Sprite()
#pragma warning(pop)