// RigidBody.generated.h
#pragma once


// ============================================================================
// RigidBody�̏�Ԃ�ۑ�����State�\���̂̒�`�AUndo/Redo�Ɏg��Memento��using�錾
// ============================================================================
#define MT_COMPONENT_RigidBody() \
	struct RigidBodyState \
	{ \
			bool isNeedUpdate_; \
			Vector3 velocity_; \
			bool useGravity_; \
			bool isGround_; \
			bool isKinematic_; \
	}; \
	class RigidBody;\
	using RigidBodyMemento = ComponentMemento<RigidBody, RigidBodyState>;

// ============================================================================
// RigidBody��RigidBodyMemento�̑��ݕϊ�����������
// ============================================================================
#define MT_GENERATED_BODY_RigidBody() \
	public: \
	IComponentMemento* SaveToMemento() \
	{ \
		RigidBodyState state; \
		state.isNeedUpdate_ = this->isNeedUpdate_; \
		state.velocity_ = this->velocity_; \
		state.useGravity_ = this->useGravity_; \
		state.isGround_ = this->isGround_; \
		state.isKinematic_ = this->isKinematic_; \
		return new ComponentMemento<RigidBody, RigidBodyState>(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const ComponentMemento<RigidBody, RigidBodyState>& _memento) \
	{ \
		const RigidBodyState& state = _memento.GetState(); \
		this->isNeedUpdate_ = state.isNeedUpdate_; \
		this->velocity_ = state.velocity_; \
		this->useGravity_ = state.useGravity_; \
		this->isGround_ = state.isGround_; \
		this->isKinematic_ = state.isKinematic_; \
		OnPostRestore(); \
	} \
	\
	friend struct RigidBody_Register; \
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(RigidBody, isNeedUpdate_, velocity_, useGravity_, isGround_, isKinematic_)

// �}�N���㏑��
#define MT_COMPONENT() MT_COMPONENT_RigidBody()
#define MT_GENERATED_BODY() MT_GENERATED_BODY_RigidBody()