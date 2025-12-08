// MovingFloor.generated.h
#pragma once


// ============================================================================
// MovingFloor�̏�Ԃ�ۑ�����State�\���̂̒�`�AUndo/Redo�Ɏg��Memento��using�錾
// ============================================================================
#define MT_COMPONENT_MovingFloor() \
	struct MovingFloorState \
	{ \
			float duration_; \
			Vector3 to_; \
			Vector3 from_; \
	}; \
	class MovingFloor;\
	using MovingFloorMemento = ComponentMemento<MovingFloor, MovingFloorState>;

// ============================================================================
// MovingFloor��MovingFloorMemento�̑��ݕϊ�����������
// ============================================================================
#define MT_GENERATED_BODY_MovingFloor() \
	public: \
	IComponentMemento* SaveToMemento() \
	{ \
		MovingFloorState state; \
		state.duration_ = this->duration_; \
		state.to_ = this->to_; \
		state.from_ = this->from_; \
		return new ComponentMemento<MovingFloor, MovingFloorState>(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const ComponentMemento<MovingFloor, MovingFloorState>& _memento) \
	{ \
		const MovingFloorState& state = _memento.GetState(); \
		this->duration_ = state.duration_; \
		this->to_ = state.to_; \
		this->from_ = state.from_; \
		OnPostRestore(); \
	} \
	\
	friend struct MovingFloor_Register; \
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(MovingFloor, duration_, to_, from_)

// �}�N���㏑��
#define MT_COMPONENT() MT_COMPONENT_MovingFloor()
#define MT_GENERATED_BODY() MT_GENERATED_BODY_MovingFloor()