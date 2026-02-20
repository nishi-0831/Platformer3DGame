// ImGuiRenderer.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
#include <string>
// ============================================================================
// ImGuiRendererの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
struct ImGuiRendererState
{
};

// クラスの前方宣言
	namespace mtgb
	{
		class ImGuiRenderer;
	}



	

using ImGuiRendererMemento = mtgb::ComponentMemento<mtgb::ImGuiRenderer, ImGuiRendererState>;

// ============================================================================
// ImGuiRendererとImGuiRendererMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_ImGuiRenderer() \
	public: \
	using Memento = ImGuiRendererMemento; \
	ImGuiRendererMemento* SaveToMemento() \
	{ \
	OnPreSave(); \
		ImGuiRendererState state; \
		return new Memento(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const Memento& _memento) \
	{ \
		const ImGuiRendererState& state = _memento.GetState(); \
		OnPostRestore(); \
	} \
	\
	friend struct ImGuiRenderer_Register; \
	friend void to_json(nlohmann::json& _j,const ImGuiRenderer& _target) \
	{ \
	} \
	friend void from_json(const nlohmann::json& _j, ImGuiRenderer& _target) \
	{ \
		_target.OnPostRestore(); \
	} \
	static std::string TypeName(){ return "ImGuiRenderer" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui() \
	{ \
		static bool registered = false; \
		if (registered) return; \
		registered = true; \
		\
		RegisterShowFuncHolder::Set<ImGuiRenderer>([]( ImGuiRenderer* _target, const char* _name) \
			{ \
			}); \
		MTImGui::Instance().RegisterComponentViewer<ImGuiRenderer>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_ImGuiRenderer()
#pragma warning(pop)