#pragma once
#include "Entity.h"
#include <string>
#include "GameObjectBuilder.h"
#include <bitset>
#include <vector>
#include "Transform.h"
#include "IComponentMemento.h"
#include <nlohmann/json.hpp>

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
		virtual inline void Start() {};
		virtual inline void End() {};
		virtual void ShowImGui();

		nlohmann::json Serialize() const;
		void Deserialize(const nlohmann::json& _json);
		/// <summary>
		/// 名前を設定する
		/// </summary>
		/// <param name="_name"></param>
		void SetName(const std::string& _name)
		{
			name_ = _name;
		}

		/// <summary>
		/// コンポーネントを取得/追加
		/// </summary>
		/// <typeparam name="ComponentT">コンポーネントの型</typeparam>
		/// <returns>コンポーネントのポインタ</returns>
		template <class ComponentT, typename... Args> ComponentT* Component(Args&&... _args);

		/// <summary>
		/// ゲームオブジェクトをインスタンスする
		/// </summary>
		/// <typeparam name="GameObjectT">ゲームオブジェクトの型</typeparam>
		/// <typeparam name="...Args">コンストラクタ可変長引数型</typeparam>
		/// <param name="..._args">コンストラクタ可変長引数</param>
		/// <returns>インスタンスされたゲームオブジェクトのポインタ</returns>
		template <class GameObjectT, typename... Args> static GameObjectT* Instantiate(Args... _args);
		/// <summary>
		/// ゲームオブジェクトが所属するシーンを指定された型で取得する
		/// </summary>
		/// <typeparam name="GameSceneT"></typeparam>
		/// <returns></returns>
		template <typename GameSceneT> GameSceneT& GetScene();

		template <typename T> static T* FindGameObject();

		template <typename T> static void FindGameObjects(std::vector<T*>* _pFoundGameObjects);

		/// <summary>
		/// 指定されたタグに一致するゲームオブジェクトを返す
		/// </summary>
		/// <param name="_tag"></param>
		/// <returns>一番最初に一致したゲームオブジェクトを返す。存在しない場合はnullptr</returns>
		static GameObject* FindGameObject(GameObjectTag _tag);
		/// <summary>
		/// 指定されたタグに一致するゲームオブジェクトを取得
		/// </summary>
		/// <param name="_tag"></param>
		/// <param name="_pFoundGameObjects"></param>
		static void FindGameObjects(GameObjectTag _tag, std::vector<GameObject*>* _pFoundGameObjects);
		/// <summary>
		/// 指定された名前に一致するゲームオブジェクトを返す
		/// </summary>
		/// <param name="_name"></param>
		/// <returns>一番最初に一致したゲームオブジェクトを返す。存在しない場合はnullptr</returns>
		static GameObject* FindGameObject(std::string_view _name);
		/// <summary>
		/// 指定された名前に一致するゲームオブジェクトを取得
		/// </summary>
		/// <param name="_name"></param>
		static void FindGameObjects(std::string_view _name, std::vector<GameObject*>* _pFoundGameObjects);
		/// <summary>
		/// 指定されたEntityIdに一致するゲームオブジェクトを返す
		/// </summary>
		/// <param name="_entityId"></param>
		/// <returns></returns>
		static GameObject* FindGameObject(EntityId _entityId);

		/// <summary>
		/// このオブジェクトを削除する
		/// </summary>
		inline void DestroyMe()
		{
			status_.toDestroy_ = TRUE;
		}
		/// <summary>
		/// このオブジェクトは削除予定か
		/// </summary>
		/// <returns>削除予定 true / false</returns>
		inline bool IsToDestroy() const
		{
			return status_.toDestroy_;
		}

		std::string GetName() const
		{
			return name_;
		}
		/// <summary>
		/// 型名を返す
		/// </summary>
		/// <returns></returns>
		std::string GetClassTypeName() const;
		/// <summary>
		/// レイヤーフラグを取得
		/// </summary>
		/// <returns>レイヤーフラグを取得する</returns>
		GameObjectLayerFlag GetLayerFlag() const
		{
			return layerFlag_;
		}

		/// <summary>
		/// タグを取得
		/// </summary>
		/// <returns></returns>
		GameObjectTag GetTag() const
		{
			return tag_;
		}
		/// <summary>
		/// Start関数が呼ばれたか否か
		/// </summary>
		/// <returns></returns>
		inline bool IsNotCalledStart() const
		{
			return isNotCalledStart_;
		}
		/// <summary>
		/// Start関数が呼ばれたフラグをオンにする
		/// </summary>
		inline void MarkAsCalledStart()
		{
			isNotCalledStart_ = false;
		}
		struct Status
		{
			uint8_t isActive_ : 1;
			uint8_t callUpdate_ : 1;
			uint8_t callDraw_ : 1;
			uint8_t toDestroy_ : 1; // 削除予定か
		};

		bool isInspectable_; // ImGuiのインスペクターに表示可能か
	  protected:
		std::string name_;

		Status status_;
		GameObjectLayerFlag layerFlag_; // レイヤーのフラグ
		GameObjectTag tag_;				// ゲームオブジェクトのタグ
		bool isNotCalledStart_;			// Start関数が呼び出されたか
	};

	template <class ComponentT, typename... Args> inline ComponentT* GameObject::Component(Args&&... _args)
	{
		return &ComponentT::Get(entityId_, std::forward<Args>(_args)...);
	}

	template <class GameObjectT, typename... Args> inline GameObjectT* GameObject::Instantiate(Args... _args)
	{
		return Game::System<SceneSystem>().GetActiveScene()->Instantiate<GameObjectT>(_args...);
	}
	template <typename GameSceneT> inline GameSceneT& GameObject::GetScene()
	{
		GameSceneT* pActiveScene { static_cast<GameSceneT*>(Game::System<SceneSystem>().GetActiveScene()) };
		massert(pActiveScene != nullptr && "アクティブシーンの取得に失敗 @GameObject::GetScene");

		return *pActiveScene;
	}
	template <typename T> inline T* GameObject::FindGameObject()
	{
		// templateキーワードを付けないと<T>の<が小なりと区別がつかない?
		return mtgb::Game::System<SceneSystem>().GetActiveScene()->template GetGameObject<T>();
	}
	template <typename T> inline void GameObject::FindGameObjects(std::vector<T*>* _pFoundGameObjects)
	{
		// templateキーワードを付けないと<T>の<が小なりと区別がつかない?
		mtgb::Game::System<SceneSystem>().GetActiveScene()->template GetGameObjects<T>(_pFoundGameObjects);
	}
} // namespace mtgb
