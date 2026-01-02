// ImageRenderer.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
#include <string>
// ============================================================================
// ImageRendererの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
#define MT_COMPONENT_ImageRenderer()                                                  \
	struct ImageRendererState                                                         \
	{                                                                                 \
	};                                                                                \
	class ImageRenderer;                                                              \
	using ImageRendererMemento = ComponentMemento<ImageRenderer, ImageRendererState>;

// ============================================================================
// ImageRendererとImageRendererMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_ImageRenderer()                                                             \
  public:                                                                                             \
	using Memento = ImageRendererMemento;                                                             \
	ImageRendererMemento* SaveToMemento()                                                             \
	{                                                                                                 \
		OnPreSave();                                                                                  \
		ImageRendererState state;                                                                     \
		return new ComponentMemento<ImageRenderer, ImageRendererState>(GetEntityId(), state);         \
	}                                                                                                 \
                                                                                                      \
	void RestoreFromMemento(const ComponentMemento<ImageRenderer, ImageRendererState>& _memento)      \
	{                                                                                                 \
		const ImageRendererState& state = _memento.GetState();                                        \
		OnPostRestore();                                                                              \
	}                                                                                                 \
                                                                                                      \
	friend struct ImageRenderer_Register;                                                             \
	friend void to_json(nlohmann::json& _j, const ImageRenderer& _target)                             \
	{                                                                                                 \
	}                                                                                                 \
	friend void from_json(const nlohmann::json& _j, ImageRenderer& _target)                           \
	{                                                                                                 \
		_target.OnPostRestore();                                                                      \
	}                                                                                                 \
	static std::string TypeName()                                                                     \
	{                                                                                                 \
		return "ImageRenderer";                                                                       \
	}                                                                                                 \
	/* ImGui表示処理の登録 */                                                                         \
	static void RegisterImGui()                                                                       \
	{                                                                                                 \
		static bool registered = false;                                                               \
		if (registered)                                                                               \
			return;                                                                                   \
		registered = true;                                                                            \
                                                                                                      \
		RegisterShowFuncHolder::Set<ImageRenderer>([](ImageRenderer* _target, const char* _name) {}); \
		MTImGui::Instance().RegisterComponentViewer<ImageRenderer>();                                 \
	}

#pragma warning(push)
#pragma warning(disable : 4005)
// マクロ上書き
#define MT_COMPONENT() MT_COMPONENT_ImageRenderer()
#define MT_GENERATED_BODY() MT_GENERATED_BODY_ImageRenderer()
#pragma warning(pop)