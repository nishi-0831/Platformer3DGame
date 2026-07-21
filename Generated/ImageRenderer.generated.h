// ImageRenderer.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include <string>



// ============================================================================
// ImageRendererの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
struct ImageRendererState
{
		std::string imageFileName_;
		mtgb::RectF drawRect_;
		mtgb::UIParams uiParams_;
		mtgb::GameObjectLayerFlag layer_;
};

// クラスの前方宣言
	namespace mtgb
	{
		class ImageRenderer;
	}



	

using ImageRendererMemento = mtgb::ComponentMemento<mtgb::ImageRenderer, ImageRendererState>;

// ============================================================================
// ImageRendererとImageRendererMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_ImageRenderer() \
	public: \
	using Memento = ImageRendererMemento; \
	ImageRendererMemento* SaveToMemento(); \
	\
	void RestoreFromMemento(const Memento& _memento); \
	\
	friend struct ImageRenderer_Register; \
	friend void to_json(nlohmann::json& _j,const ImageRenderer& _target); \
	friend void from_json(const nlohmann::json& _j, ImageRenderer& _target); \
	\
	static std::string TypeName(){ return "ImageRenderer" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui(); \

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_ImageRenderer()
#pragma warning(pop)