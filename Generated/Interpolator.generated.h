// Interpolator.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JSON/JsonConverter.h"
#include <string>



// ============================================================================
// Interpolatorの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
struct InterpolatorState
{
		float dir_;
		float elapsed_;
		float duration_;
		mtgb::Vector3 startPos_;
		mtgb::Vector3 endPos_;
};

// クラスの前方宣言
	namespace mtgb
	{
		class Interpolator;
	}



	

using InterpolatorMemento = mtgb::ComponentMemento<mtgb::Interpolator, InterpolatorState>;

// ============================================================================
// InterpolatorとInterpolatorMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_Interpolator() \
	public: \
	using Memento = InterpolatorMemento; \
	InterpolatorMemento* SaveToMemento(); \
	\
	void RestoreFromMemento(const Memento& _memento); \
	\
	friend struct Interpolator_Register; \
	friend void to_json(nlohmann::json& _j,const Interpolator& _target); \
	friend void from_json(const nlohmann::json& _j, Interpolator& _target); \
	\
	static std::string TypeName(){ return "Interpolator" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui(); \

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_Interpolator()
#pragma warning(pop)