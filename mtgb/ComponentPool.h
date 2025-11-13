#pragma once
#include <vector>
#include "cmtgb.h"
#include "IComponentPool.h"
#include "ISystem.h"
#include <nlohmann/json.hpp>
#include "ComponentRegistry.h"
namespace mtgb
{
	static constexpr size_t COMPONENT_CAPACITY{ 8192 };

	//class GameObject;
	class Entity;

	/// <summary>
	/// コンポーネントを連続した配置にするプール
	/// </summary>
	/// <typeparam name="ComponentT">コンポーネントの型</typeparam>
	/// <typeparam name="IsSingleton">１つしか格納できないか true / false</typeparam>
	template<typename ComponentT,typename DerivedT, bool IsSingleton = true>
	class ComponentPool : public IComponentPool, public ISystem
	{
	public:
		ComponentPool();
		virtual ~ComponentPool();

		void Initialize() override;
		virtual void Start() {};
		virtual void Update() override {}

		void Release() override;
		nlohmann::json Serialize(EntityId _entityId) override;
		IComponentMemento* Deserialize(EntityId _entityId, const nlohmann::json& _json) override;
		void Copy(EntityId _dest, EntityId _src) override;
		ComponentT* Reuse(size_t _index, EntityId _entityId);
		/// <summary>
		/// コンポーネントを作成/取得する
		/// </summary>
		/// <param name="_entityId">エンティティId</param>
		/// <returns>コンポーネントの参照ポインタ (確実に存在する)</returns>
		template<typename... Args>
		ComponentT& Get(EntityId _entityId, Args&&... _args);

		bool TryGet(ComponentT*& _pComponent, const EntityId _entityId);

		bool TryGet(std::vector<ComponentT*>* _pComponents, const EntityId _entityId);
		
		/*template<typename... Args>
		ComponentT& Add(EntityId _entityId, Args&&... _args) requires(!IsSingleton);*/

		/// <summary>
		/// エンティティが持っているコンポーネントを削除する
		/// </summary>
		/// <param name="_entityId">エンティティId</param>
		void Remove(const EntityId _entityId) override;

		/// <summary>
		/// エンティティに登録されたコンポーネントを解除する
		/// </summary>
		/// <param name="_pEntity">登録されているエンティティ</param>
		void UnRegister(EntityId _entityId);


	protected:
		std::vector<ComponentT> pool_;  // コンポーネントそのものを格納するプール
		std::vector<EntityId> poolId_;  // コンポーネントの登録エンティティId
	};

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline ComponentPool<ComponentT, DerivedT, IsSingleton>::ComponentPool() :
		pool_{},
		poolId_{}
	{
		pool_.reserve(COMPONENT_CAPACITY);
		poolId_.reserve(COMPONENT_CAPACITY);

		Game::System<ComponentRegistry>().RegisterComponentPoolType(typeid(ComponentT), typeid(DerivedT));
		Game::System<ComponentRegistry>().RegisterComponentKey(ComponentT::DisplayName(), typeid(ComponentT));
	}

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline ComponentPool< ComponentT,DerivedT, IsSingleton>::~ComponentPool()
	{

	}

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline void ComponentPool<ComponentT, DerivedT, IsSingleton>::Initialize()
	{
		RegisterCurrentScene([&, this] { Release(); });
		Start();
	}

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline void ComponentPool<ComponentT, DerivedT, IsSingleton>::Release()
	{
		pool_.clear();
		poolId_.clear();

		//pool_.reserve(COMPONENT_CAPACITY);
		//poolId_.reserve(COMPONENT_CAPACITY);
	}

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline  nlohmann::json ComponentPool<ComponentT, DerivedT, IsSingleton>::Serialize(EntityId _entityId)
	{
		for (int i = 0; i < poolId_.size(); i++)
		{
			if (poolId_[i] == _entityId)
			{
				return pool_[i].Serialize();
			}
		}
		return nlohmann::json{};
	}

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline IComponentMemento* ComponentPool<ComponentT, DerivedT, IsSingleton>::Deserialize(EntityId _entityId, const nlohmann::json& _json)
	{
		return ComponentT::Deserialize(_entityId, _json);
	}

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline void ComponentPool<ComponentT, DerivedT, IsSingleton>::Copy(EntityId _dest, EntityId _src)
	{
		ComponentT& destCom = Get(_dest);
		ComponentT& srcCom = Get(_src);
		
		destCom.CopyData(srcCom);
	}

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline ComponentT* ComponentPool<ComponentT, DerivedT, IsSingleton>::Reuse(size_t _index, EntityId _entityId)
	{
		if (poolId_.size() <= _index)
			return nullptr;
		if (poolId_[_index] != INVALID_ENTITY)
			return nullptr;

		poolId_[_index] = _entityId;

		ComponentT* pComponent = &pool_[_index];
		pComponent->entityId_ = _entityId;
		return pComponent;
	}

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	template<typename... Args>
	inline ComponentT& ComponentPool<ComponentT, DerivedT, IsSingleton>::Get(EntityId _entityId, Args&&... _args)
	{
		for (int i = 0; i < poolId_.size(); i++)
		{
			if (poolId_[i] == _entityId)
			{
				return pool_[i];
			}
		}

		// プールに存在しないなら新たに追加
		poolId_.push_back(_entityId);
		// NOTE: emplace_backで実体をそのまま追加
		pool_.emplace_back(_entityId, std::forward<Args>(_args)...); // 可変長引数でコンストラクタ呼び出し
		// 追加したら初期化処理
		pool_.back().Initialize();
		assert(poolId_.size() < COMPONENT_CAPACITY);

		// インデックスを記録
		size_t poolIndex = pool_.size() - 1;
		Game::System<ComponentRegistry>().RegisterComponentIndex(_entityId, std::type_index(typeid(ComponentT)), poolIndex);

		// EntityIdに割り当てられたComponentとして登録
		Game::System<ComponentRegistry>().RegisterComponent(_entityId, std::type_index(typeid(ComponentT)));

		return pool_.back(); // 追加&&初期化したコンポーネントを返す
	}

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline bool ComponentPool<ComponentT, DerivedT, IsSingleton>::TryGet(
		ComponentT*& _pComponent, const EntityId _entityId)
	{
		for (int i = 0; i < poolId_.size(); i++)
		{
			if (poolId_[i] == _entityId)
			{
				_pComponent = &pool_[i];  // Idが一致した添字のコンポーネントを返す
				return true;
			}
		}

		return false;
	}

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline bool ComponentPool<ComponentT, DerivedT, IsSingleton>::TryGet(
		std::vector<ComponentT*>* _pComponents, const EntityId _entityId) 
	{
		_pComponents->clear();
		for (int i = 0; i < poolId_.size(); i++)
		{
			if (poolId_[i] == _entityId)
			{
				_pComponents->push_back(&pool_[i]);  // Idが一致した添字のコンポーネントを追加する
			}
		}

		// 何かしら追加されたなら見つかっている
		return _pComponents->size() >= 0;
	}

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline void ComponentPool<ComponentT, DerivedT, IsSingleton>::Remove(const EntityId _entityId)
	{
		if constexpr (IsSingleton)
		{
			for (int i = 0; i < poolId_.size(); i++)
			{
				if (poolId_[i] == _entityId)
				{
					poolId_[i] = INVALID_ENTITY;

					// 登録解除
					Game::System<ComponentRegistry>().UnRegisterComponent(_entityId, std::type_index(typeid(ComponentT)));
					return;  // 見つかったなら無効Idにして回帰
				}
			}
		}
		else  // 複数ある可能性を考慮する
		{
			for (int i = 0; i < poolId_.size(); i++)
			{
				if (poolId_[i] == _entityId)
				{
					poolId_[i] = INVALID_ENTITY;
				}
			}
		}
	}

	

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline void ComponentPool<ComponentT, DerivedT, IsSingleton>::UnRegister(EntityId _entityId)
	{
		for (int i = 0; i < poolId_.size(); i++)
		{
			if (poolId_[i] == _entityId)
			{
				poolId_.erase(poolId_.begin() + i);
				pool_.erase(pool_.begin() + i);
				return;  // ポインタが一致したなら消す
			}
		}
	}
}
