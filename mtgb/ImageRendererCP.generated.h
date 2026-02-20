// ImageRendererCP.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
#include <string>
// ============================================================================
// ImageRendererCPの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
struct ImageRendererCPState
{
};

// クラスの前方宣言
	namespace mtgb
	{
		class ImageRendererCP;
	}



	

using ImageRendererCPMemento = mtgb::ComponentMemento<mtgb::ImageRendererCP, ImageRendererCPState>;

// ============================================================================
// ImageRendererCPとImageRendererCPMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_ImageRendererCP() \
	public: \
	using Memento = ImageRendererCPMemento; \
	ImageRendererCPMemento* SaveToMemento() \
	{ \
	OnPreSave(); \
		ImageRendererCPState state; \
		return new Memento(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const Memento& _memento) \
	{ \
		const ImageRendererCPState& state = _memento.GetState(); \
		OnPostRestore(); \
	} \
	\
	friend struct ImageRendererCP_Register; \
	friend void to_json(nlohmann::json& _j,const ImageRendererCP& _target) \
	{ \
	} \
	friend void from_json(const nlohmann::json& _j, ImageRendererCP& _target) \
	{ \
		_target.OnPostRestore(); \
	} \
	static std::string TypeName(){ return "ImageRendererCP" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui() \
	{ \
		static bool registered = false; \
		if (registered) return; \
		registered = true; \
		\
		RegisterShowFuncHolder::Set<ImageRendererCP>([]( ImageRendererCP* _target, const char* _name) \
			{ \
			}); \
		MTImGui::Instance().RegisterComponentViewer<ImageRendererCP>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_ImageRendererCP()
#pragma warning(pop)