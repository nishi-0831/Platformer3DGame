// MeshRenderer.generated.h
#pragma once


// ============================================================================
// MeshRenderer�̏�Ԃ�ۑ�����State�\���̂̒�`�AUndo/Redo�Ɏg��Memento��using�錾
// ============================================================================
#define MT_COMPONENT_MeshRenderer() \
	struct MeshRendererState \
	{ \
			std::string meshFileName; \
			FBXModelHandle meshHandle; \
			GameObjectLayerFlag layer; \
			ShaderType shaderType; \
	}; \
	class MeshRenderer;\
	using MeshRendererMemento = ComponentMemento<MeshRenderer, MeshRendererState>;

// ============================================================================
// MeshRenderer��MeshRendererMemento�̑��ݕϊ�����������
// ============================================================================
#define MT_GENERATED_BODY_MeshRenderer() \
	public: \
	IComponentMemento* SaveToMemento() \
	{ \
		MeshRendererState state; \
		state.meshFileName = this->meshFileName; \
		state.meshHandle = this->meshHandle; \
		state.layer = this->layer; \
		state.shaderType = this->shaderType; \
		return new ComponentMemento<MeshRenderer, MeshRendererState>(GetEntityId(), state); \
	} \
	\
	void RestoreFromMemento(const ComponentMemento<MeshRenderer, MeshRendererState>& _memento) \
	{ \
		const MeshRendererState& state = _memento.GetState(); \
		this->meshFileName = state.meshFileName; \
		this->meshHandle = state.meshHandle; \
		this->layer = state.layer; \
		this->shaderType = state.shaderType; \
		OnPostRestore(); \
	} \
	\
	friend struct MeshRenderer_Register; \
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(MeshRenderer, meshFileName, meshHandle, layer, shaderType)

// �}�N���㏑��
#define MT_COMPONENT() MT_COMPONENT_MeshRenderer()
#define MT_GENERATED_BODY() MT_GENERATED_BODY_MeshRenderer()