// AudioPlayer.generated.h
#pragma once

#include <nlohmann/json.hpp>
#include "JsonConverter.h"
#include "MTImGui.h"
#include <string>
// ============================================================================
// AudioPlayerの状態を保存するState構造体の定義、Undo/Redoに使うMementoのusing宣言
// ============================================================================
#define MT_COMPONENT_AudioPlayer() \
	struct AudioPlayerState \
	{ \
			AudioHandle hAudio; \
	}; \
	class AudioPlayer;\
	using AudioPlayerMemento = ComponentMemento<AudioPlayer, AudioPlayerState>;

// ============================================================================
// AudioPlayerとAudioPlayerMementoの相互変換処理を実装
// ============================================================================
#define MT_GENERATED_BODY_AudioPlayer() \
	public: \
	using Memento = AudioPlayerMemento; \
	AudioPlayerMemento* SaveToMemento() \
	{ \
	OnPreSave(); \
		AudioPlayerState state; \
		state.hAudio = this->hAudio; \
		return new ComponentMemento<AudioPlayer, AudioPlayerState>(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const ComponentMemento<AudioPlayer, AudioPlayerState>& _memento) \
	{ \
		const AudioPlayerState& state = _memento.GetState(); \
		this->hAudio = state.hAudio; \
		OnPostRestore(); \
	} \
	\
	friend struct AudioPlayer_Register; \
	friend void to_json(nlohmann::json& _j,AudioPlayer& _target) \
	{ \
	_target.OnPreSave(); \
		_j["hAudio"] = JsonConverter::Serialize<AudioHandle>(_target.hAudio); \
	} \
	friend void from_json(const nlohmann::json& _j, AudioPlayer& _target) \
	{ \
		JsonConverter::Deserialize<AudioHandle>(_target.hAudio, _j.at("hAudio")); \
		_target.OnPostRestore(); \
	} \
	static std::string TypeName(){ return "AudioPlayer" ;} \
	/* ImGui表示処理の登録 */ \
	static void RegisterImGui() \
	{ \
		static bool registered = false; \
		if (registered) return; \
		registered = true; \
		\
		RegisterShowFuncHolder::Set<AudioPlayer>([]( AudioPlayer* _target, const char* _name) \
			{ \
				TypeRegistry::Instance().CallFunc(&_target->hAudio, "hAudio"); \
			}); \
		MTImGui::Instance().RegisterComponentViewer<AudioPlayer>(); \
	}

#pragma warning(push)
#pragma warning(disable:4005)
// マクロ上書き
#define MT_COMPONENT() MT_COMPONENT_AudioPlayer()
#define MT_GENERATED_BODY() MT_GENERATED_BODY_AudioPlayer()
#pragma warning(pop)