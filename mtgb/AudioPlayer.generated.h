// AudioPlayer.generated.h
#pragma once


// ============================================================================
// AudioPlayer�̏�Ԃ�ۑ�����State�\���̂̒�`�AUndo/Redo�Ɏg��Memento��using�錾
// ============================================================================
#define MT_COMPONENT_AudioPlayer() \
	struct AudioPlayerState \
	{ \
			AudioHandle hAudio; \
	}; \
	class AudioPlayer;\
	using AudioPlayerMemento = ComponentMemento<AudioPlayer, AudioPlayerState>;

// ============================================================================
// AudioPlayer��AudioPlayerMemento�̑��ݕϊ�����������
// ============================================================================
#define MT_GENERATED_BODY_AudioPlayer() \
	public: \
	IComponentMemento* SaveToMemento() \
	{ \
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
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(AudioPlayer, hAudio)

// �}�N���㏑��
#define MT_COMPONENT() MT_COMPONENT_AudioPlayer()
#define MT_GENERATED_BODY() MT_GENERATED_BODY_AudioPlayer()