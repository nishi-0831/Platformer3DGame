#pragma once
//#include "World.h"
#include <cmtgb.h>
#include <list>
#include "Handlers.h"
#include <string>
#include <vector>
#include <type_traits>
#include "GameObjectTag.h"
namespace mtgb
{
	using EntityId = int64_t;

	class GameObject;
	class Transform;

	//using GameScene = World;
	class GameScene
	{
		friend class SceneSystem;
		friend class RenderSystem;
		friend class Game;
	private:
		static GameScene* pInstance_;

	public:
		GameScene();
		virtual ~GameScene();

		template<class GameObjectT, typename ...Args>
		GameObjectT* Instantiate(Args... _args);
		void RegisterGameObject(GameObject* _pGameObject);

		/// <summary>
		/// �J�����I�u�W�F�N�g�Ƃ��ēo�^����
		/// </summary>
		/// <param name="_pGameObject">�o�^����Q�[���I�u�W�F�N�g�̃|�C���^</param>
		/// <returns>�o�^���ꂽ�J�����n���h��</returns>
		CameraHandleInScene RegisterCameraGameObject(GameObject* _pGameObject) const;
		
		GameObject* GetGameObject(std::string _name) const;
		void GetGameObjects(const std::string& _name, std::vector<GameObject*>* _pFoundGameObjects) const;
		void GetGameObjects(GameObjectTag _tag, std::vector<GameObject*>* _pFoundGameObjects) const;

		/// <summary>
		/// �e���v���[�g�p�����[�^�ŃI�u�W�F�N�g���擾
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns>���݂��Ă��Ȃ��AGameObject���p�����Ă��Ȃ��Ȃ�nullptr</returns>
		template<typename T>
		T* GetGameObject() const;

		template<typename T>
		void GetGameObjects(std::vector<T*>* _pFoundGameObjects) const;


		/// <summary>
		/// EntityId�ŃI�u�W�F�N�g���擾
		/// </summary>
		/// <param name="_entityId"></param>
		/// <returns>���݂��Ȃ��Ȃ�nullptr</returns>
		GameObject* GetGameObject(const EntityId _entityId) const;


	protected:
		virtual void Initialize();
		virtual void Update();
		virtual void Draw() const;
		virtual void End();

	private:
		std::list<GameObject*> pGameObjects_;  // �V�[���ɓo�ꂷ��Q�[���I�u�W�F�N�g
	};

	template<class GameObjectT, typename ...Args>
	inline GameObjectT* GameScene::Instantiate(Args... _args)
	{
		// ���N���X��GameObject�ł��邩
		static_assert(std::is_base_of<GameObject, GameObjectT>().value
			&& "GameObject�N���X���p�����Ă��Ȃ��N���X�̓C���X�^���X�ł��܂���B");

		GameObjectT* pInstance{ new GameObjectT{ _args... } };

		// TODO: �������A�������z��ɂ���
		pGameObjects_.push_back(pInstance);

		return pInstance;
	}
	template<typename T>
	inline T* GameScene::GetGameObject() const
	{
		// ���N���X��GameObject�ł��邩
		if (std::is_base_of<GameObject, std::remove_cvref_t<T>>().value == false)
		{
			return nullptr;
		}
	
		for (GameObject* obj : pGameObjects_)
		{
			T* instance = dynamic_cast<T*>(obj);

			if (instance)
			{
				return instance;
			}
		}
		return nullptr;
	}
	template<typename T>
	inline void GameScene::GetGameObjects(std::vector<T*>* _pFoundGameObjects) const
	{
		_pFoundGameObjects->clear();

		// ���N���X��GameObject�ł��邩
		if (std::is_base_of<GameObject, std::remove_cvref_t<T>>().value == false)
		{
			return;
		}

		for (GameObject* obj : pGameObjects_)
		{
			T* instance = dynamic_cast<T*>(obj);

			if (instance)
			{
				_pFoundGameObjects->push_back(instance);
			}
		}
	}
}
