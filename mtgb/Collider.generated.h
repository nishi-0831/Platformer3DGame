// Collider.generated.h
#pragma once


// ============================================================================
// Collider�̏�Ԃ�ۑ�����State�\���̂̒�`�AUndo/Redo�Ɏg��Memento��using�錾
// ============================================================================
#define MT_COMPONENT_Collider() \
	struct ColliderState \
	{ \
			ColliderType colliderType_; \
			bool isStatic_; \
			ColliderTag colliderTag_; \
			Vector3 center_; \
			float radius_; \
			Vector3 extents_; \
	}; \
	class Collider;\
	using ColliderMemento = ComponentMemento<Collider, ColliderState>;

// ============================================================================
// Collider��ColliderMemento�̑��ݕϊ�����������
// ============================================================================
#define MT_GENERATED_BODY_Collider() \
	public: \
	IComponentMemento* SaveToMemento() \
	{ \
		ColliderState state; \
		state.colliderType_ = this->colliderType_; \
		state.isStatic_ = this->isStatic_; \
		state.colliderTag_ = this->colliderTag_; \
		state.center_ = this->center_; \
		state.radius_ = this->radius_; \
		state.extents_ = this->extents_; \
		return new ComponentMemento<Collider, ColliderState>(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const ComponentMemento<Collider, ColliderState>& _memento) \
	{ \
		const ColliderState& state = _memento.GetState(); \
		this->colliderType_ = state.colliderType_; \
		this->isStatic_ = state.isStatic_; \
		this->colliderTag_ = state.colliderTag_; \
		this->center_ = state.center_; \
		this->radius_ = state.radius_; \
		this->extents_ = state.extents_; \
		OnPostRestore(); \
	} \
	\
	friend struct Collider_Register; \
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Collider, colliderType_, isStatic_, colliderTag_, center_, radius_, extents_)

// �}�N���㏑��
#define MT_COMPONENT() MT_COMPONENT_Collider()
#define MT_GENERATED_BODY() MT_GENERATED_BODY_Collider()