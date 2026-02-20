// TextRendererCP.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
#include <string>
// ============================================================================
// TextRendererCPの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
struct TextRendererCPState
{
};

// クラスの前方宣言
	namespace mtgb
	{
		class TextRendererCP;
	}



	

using TextRendererCPMemento = mtgb::ComponentMemento<mtgb::TextRendererCP, TextRendererCPState>;

// ============================================================================
// TextRendererCPとTextRendererCPMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_TextRendererCP() \
	public: \
	using Memento = TextRendererCPMemento; \
	TextRendererCPMemento* SaveToMemento() \
	{ \
	OnPreSave(); \
		TextRendererCPState state; \
		return new Memento(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const Memento& _memento) \
	{ \
		const TextRendererCPState& state = _memento.GetState(); \
		OnPostRestore(); \
	} \
	\
	friend struct TextRendererCP_Register; \
	friend void to_json(nlohmann::json& _j,const TextRendererCP& _target) \
	{ \
	} \
	friend void from_json(const nlohmann::json& _j, TextRendererCP& _target) \
	{ \
		_target.OnPostRestore(); \
	} \
	static std::string TypeName(){ return "TextRendererCP" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui() \
	{ \
		static bool registered = false; \
		if (registered) return; \
		registered = true; \
		\
		RegisterShowFuncHolder::Set<TextRendererCP>([]( TextRendererCP* _target, const char* _name) \
			{ \
			}); \
		MTImGui::Instance().RegisterComponentViewer<TextRendererCP>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_TextRendererCP()
#pragma warning(pop)