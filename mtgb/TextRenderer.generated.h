// TextRenderer.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
#include <string>
// ============================================================================
// TextRendererの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
struct TextRendererState
{
};

// クラスの前方宣言
	namespace mtgb
	{
		class TextRenderer;
	}



	

using TextRendererMemento = mtgb::ComponentMemento<mtgb::TextRenderer, TextRendererState>;

// ============================================================================
// TextRendererとTextRendererMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_TextRenderer() \
	public: \
	using Memento = TextRendererMemento; \
	TextRendererMemento* SaveToMemento() \
	{ \
	OnPreSave(); \
		TextRendererState state; \
		return new Memento(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const Memento& _memento) \
	{ \
		const TextRendererState& state = _memento.GetState(); \
		OnPostRestore(); \
	} \
	\
	friend struct TextRenderer_Register; \
	friend void to_json(nlohmann::json& _j,const TextRenderer& _target) \
	{ \
	} \
	friend void from_json(const nlohmann::json& _j, TextRenderer& _target) \
	{ \
		_target.OnPostRestore(); \
	} \
	static std::string TypeName(){ return "TextRenderer" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui() \
	{ \
		static bool registered = false; \
		if (registered) return; \
		registered = true; \
		\
		RegisterShowFuncHolder::Set<TextRenderer>([]( TextRenderer* _target, const char* _name) \
			{ \
			}); \
		MTImGui::Instance().RegisterComponentViewer<TextRenderer>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_TextRenderer()
#pragma warning(pop)