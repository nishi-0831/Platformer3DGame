#pragma once
#include "Entity.h"
#include <string>
#include "GameObjectBuilder.h"
#include <bitset>
#include <vector>
#include "Transform.h"
#include "IComponentMemento.h"
#include <nlohmann/json.hpp>
constexpr size_t COMPONENT_CAPACITY{ (8 * 8) - 4 };

namespace mtgb
{
	class SceneSystem;

	class GameObject : public Entity
	{
	public:
		GameObject(const GAME_OBJECT_DESC& _desc);
		GameObject();
		GameObject(const GameObject& _other);
		virtual ~GameObject();

		virtual inline void Initialize() {};
		virtual inline void Update() {};
		virtual inline void Draw() const {};
		virtual inline void End() {};
		virtual inline std::vector<IComponentMemento*> GetDefaultMementos(EntityId _entityId) const { return std::vector<IComponentMemento*>(); };

		nlohmann::json SerializeGameObject() const;
		/// <summary>
		/// ���O��ݒ肷��
		/// </summary>
		/// <param name="_name"></param>
		void SetName(const std::string _name) { name_ = _name; }

		/// <summary>
		/// �R���|�[�l���g���擾/�ǉ�
		/// </summary>
		/// <typeparam name="ComponentT">�R���|�[�l���g�̌^</typeparam>
		/// <returns>�R���|�[�l���g�̃|�C���^</returns>
		template<class ComponentT, typename ...Args>
		ComponentT* Component(Args&&... _args);

		/// <summary>
		/// �Q�[���I�u�W�F�N�g���C���X�^���X����
		/// </summary>
		/// <typeparam name="GameObjectT">�Q�[���I�u�W�F�N�g�̌^</typeparam>
		/// <typeparam name="...Args">�R���X�g���N�^�ϒ������^</typeparam>
		/// <param name="..._args">�R���X�g���N�^�ϒ�����</param>
		/// <returns>�C���X�^���X���ꂽ�Q�[���I�u�W�F�N�g�̃|�C���^</returns>
		template<class GameObjectT, typename ...Args>
		static GameObjectT* Instantiate(Args... _args);

		template<typename GameSceneT>
		GameSceneT& GetScene();

		template<typename T>
		static T* FindGameObject();

		template<typename T>
		static void FindGameObjects(std::vector<T*>* _pFoundGameObjects);

		static GameObject* FindGameObject(const std::string& _name);
		static void FindGameObjects(const std::string& _name, std::vector<GameObject*>* _pFoundGameObjects);
		static GameObject* FindGameObject(const EntityId _entityId);
		static void FindGameObjects(GameObjectTag _tag,std::vector<GameObject*>* _pFoundGameObjects);

		/// <summary>
		/// ���̃I�u�W�F�N�g���폜����
		/// </summary>
		inline void DestroyMe() { status_.toDestroy_ = TRUE; }
		/// <summary>
		/// ���̃I�u�W�F�N�g�͍폜�\�肩
		/// </summary>
		/// <returns>�폜�\�� true / false</returns>
		inline const bool IsToDestroy() const { return status_.toDestroy_; }
		std::string GetName() const { return name_; }
		/// <summary>
		/// ���C���[�t���O���擾
		/// </summary>
		/// <returns>���C���[�t���O���擾����</returns>
		GameObjectLayerFlag GetLayerFlag() const { return layerFlag_; }

		/// <summary>
		/// �^�O���擾
		/// </summary>
		/// <returns></returns>
		GameObjectTag GetTag() const { return tag_; }
	protected:

		std::string name_;

		struct Status
		{
			uint8_t isActive_ : 1;
			uint8_t callUpdate_ : 1;
			uint8_t callDraw_ : 1;
			uint8_t toDestroy_ : 1;  // �폜�\�肩
		} status_;

		GameObjectLayerFlag layerFlag_;  // ���C���[�̃t���O
		GameObjectTag tag_; // �Q�[���I�u�W�F�N�g�̃^�O

		std::bitset<COMPONENT_CAPACITY> componentsFlag_;  // �R���|�[�l���g�̃t���O
	};

	template<class ComponentT, typename ...Args>
	inline ComponentT* GameObject::Component(Args&&... _args)
	{
		return &ComponentT::Get(entityId_, std::forward<Args>(_args)...);
	}

	template<class GameObjectT, typename ...Args>
	inline GameObjectT* GameObject::Instantiate(Args ..._args)
	{
		return Game::System<SceneSystem>().GetActiveScene()
			->Instantiate<GameObjectT>(_args...);
	}
	template<typename GameSceneT>
	inline GameSceneT& GameObject::GetScene()
	{
		GameSceneT* pActiveScene{ static_cast<GameSceneT*>(Game::System<SceneSystem>().GetActiveScene()) };
		massert(pActiveScene != nullptr
			&& "�A�N�e�B�u�V�[���̎擾�Ɏ��s @GameObject::GetScene");

		return *pActiveScene;
	}
	template<typename T>
	inline T* GameObject::FindGameObject()
	{
		// template�L�[���[�h��t���Ȃ���<T>��<�����Ȃ�Ƌ�ʂ����Ȃ�?
		return mtgb::Game::System<SceneSystem>().GetActiveScene()->template GetGameObject<T>();
	}
	template<typename T>
	inline void GameObject::FindGameObjects(std::vector<T*>* _pFoundGameObjects)
	{
		// template�L�[���[�h��t���Ȃ���<T>��<�����Ȃ�Ƌ�ʂ����Ȃ�?
		mtgb::Game::System<SceneSystem>().GetActiveScene()->template GetGameObjects<T>(_pFoundGameObjects);
	}
}
