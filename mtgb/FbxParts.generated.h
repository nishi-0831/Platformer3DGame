// FbxParts.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
#include <string>
// ============================================================================
// FbxPartsの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
struct FbxPartsState
{
};

// クラスの前方宣言
	namespace mtgb
	{
		class FbxParts;
	}



	

using FbxPartsMemento = mtgb::ComponentMemento<mtgb::FbxParts, FbxPartsState>;

// ============================================================================
// FbxPartsとFbxPartsMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_FbxParts() \
	public: \
	using Memento = FbxPartsMemento; \
	FbxPartsMemento* SaveToMemento() \
	{ \
	OnPreSave(); \
		FbxPartsState state; \
		return new Memento(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const Memento& _memento) \
	{ \
		const FbxPartsState& state = _memento.GetState(); \
		OnPostRestore(); \
	} \
	\
	friend struct FbxParts_Register; \
	friend void to_json(nlohmann::json& _j,const FbxParts& _target) \
	{ \
	} \
	friend void from_json(const nlohmann::json& _j, FbxParts& _target) \
	{ \
		_target.OnPostRestore(); \
	} \
	static std::string TypeName(){ return "FbxParts" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui() \
	{ \
		static bool registered = false; \
		if (registered) return; \
		registered = true; \
		\
		RegisterShowFuncHolder::Set<FbxParts>([]( FbxParts* _target, const char* _name) \
			{ \
			}); \
		MTImGui::Instance().RegisterComponentViewer<FbxParts>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_GENERATED_BODY() MT_GENERATED_BODY_FbxParts()
#pragma warning(pop)