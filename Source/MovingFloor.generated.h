// MovingFloor.generated.h
#pragma once


// ============================================================================
// MovingFloor�̏�Ԃ�ۑ�����State�\���̂̒�`�AUndo/Redo�Ɏg��Memento��using�錾
// ============================================================================
#define MT_COMPONENT_MovingFloor() \
	struct MovingFloorState \
	{ \
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
		return new ComponentMemento<MovingFloor, MovingFloorState>(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const ComponentMemento<MovingFloor, MovingFloorState>& _memento) \
	{ \
		const MovingFloorState& state = _memento.GetState(); \
		OnPostRestore(); \
	} \
	\
	friend struct MovingFloor_Register; \
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(MovingFloor, )

// �}�N���㏑��
#define MT_COMPONENT() MT_COMPONENT_MovingFloor()
#define MT_GENERATED_BODY() MT_GENERATED_BODY_MovingFloor()