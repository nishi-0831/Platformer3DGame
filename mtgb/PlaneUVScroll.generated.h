// PlaneUVScroll.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
#include <string>
// ============================================================================
// PlaneUVScrollの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
struct PlaneUVScrollState
{
};

// クラスの前方宣言
	namespace mtgb
	{
		class PlaneUVScroll;
	}



	

using PlaneUVScrollMemento = mtgb::ComponentMemento<mtgb::PlaneUVScroll, PlaneUVScrollState>;

// ============================================================================
// PlaneUVScrollとPlaneUVScrollMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_PlaneUVScroll() \
	public: \
	using Memento = PlaneUVScrollMemento; \
	PlaneUVScrollMemento* SaveToMemento() \
	{ \
	OnPreSave(); \
		PlaneUVScrollState state; \
		return new Memento(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const Memento& _memento) \
	{ \
		const PlaneUVScrollState& state = _memento.GetState(); \
		OnPostRestore(); \
	} \
	\
	friend struct PlaneUVScroll_Register; \
	friend void to_json(nlohmann::json& _j,const PlaneUVScroll& _target) \
	{ \
	} \
	friend void from_json(const nlohmann::json& _j, PlaneUVScroll& _target) \
	{ \
		_target.OnPostRestore(); \
	} \
	static std::string TypeName(){ return "PlaneUVScroll" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui() \
	{ \
		static bool registered = false; \
		if (registered) return; \
		registered = true; \
		\
		RegisterShowFuncHolder::Set<PlaneUVScroll>([]( PlaneUVScroll* _target, const char* _name) \
			{ \
			}); \
		MTImGui::Instance().RegisterComponentViewer<PlaneUVScroll>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_PlaneUVScroll()
#pragma warning(pop)