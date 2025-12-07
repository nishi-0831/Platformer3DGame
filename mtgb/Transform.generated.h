// Transform.generated.h
#pragma once


// ============================================================================
// Transform�̏�Ԃ�ۑ�����State�\���̂̒�`�AUndo/Redo�Ɏg��Memento��using�錾
// ============================================================================
#define MT_COMPONENT_Transform() \
	struct TransformState \
	{ \
			EntityId parent; \
			Vector3 position; \
			Vector3 scale; \
			Quaternion rotate; \
	}; \
	class Transform;\
	using TransformMemento = ComponentMemento<Transform, TransformState>;

// ============================================================================
// Transform��TransformMemento�̑��ݕϊ�����������
// ============================================================================
#define MT_GENERATED_BODY_Transform() \
	public: \
	IComponentMemento* SaveToMemento() \
	{ \
		TransformState state; \
		state.parent = this->parent; \
		state.position = this->position; \
		state.scale = this->scale; \
		state.rotate = this->rotate; \
		return new ComponentMemento<Transform, TransformState>(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const ComponentMemento<Transform, TransformState>& _memento) \
	{ \
		const TransformState& state = _memento.GetState(); \
		this->parent = state.parent; \
		this->position = state.position; \
		this->scale = state.scale; \
		this->rotate = state.rotate; \
		OnPostRestore(); \
	} \
	\
	friend struct Transform_Register; \
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Transform, parent, position, scale, rotate)

// �}�N���㏑��
#define MT_COMPONENT() MT_COMPONENT_Transform()
#define MT_GENERATED_BODY() MT_GENERATED_BODY_Transform()