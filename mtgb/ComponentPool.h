#pragma once
#include <vector>
#include "cmtgb.h"
#include "IComponentPool.h"
#include "ISystem.h"
#include <nlohmann/json.hpp>
#include "ComponentRegistry.h"
#include "JsonConverter.h"
#include "AddComponentCommandGeneric.h"
#include "CommandHistoryManager.h"
#include "MTAssert.h"
namespace mtgb
{
	inline constexpr size_t COMPONENT_CAPACITY { 8192 };

	namespace detail
	{
		template <typename T>
		concept HasRegisterImGui = requires {
			{ T::RegisterImGui() };
		};
	} // namespace detail
	class Entity;

	template <typename ComponentT, typename DerivedT> class ComponentPool : public IComponentPool, public ISystem
	{
	  public:
		using Component = ComponentT;
		ComponentPool();
		virtual ~ComponentPool();

		void Initialize() override final;
		virtual void Start() {};
		virtual void Update() override {}

		void Release() override;
		/// <summary>
		/// JSONにシリアライズする
		/// </summary>
		/// <param name="_entityId">シリアライズするEntityのID</param>
		/// <returns>シリアライズしたJSON</returns>
		nlohmann::json Serialize(EntityId _entityId) override;
		/// <summary>
		/// メメントを作成する
		/// </summary>
		/// <param name="_entityId"></param>
		/// <returns></returns>
		IComponentMemento* SaveToMemento(EntityId _entityId) override;
		/// <summary>
		/// JSONをもとにデシリアライズする
		/// </summary>
		/// <param name="_entityId">JSONデータを反映するEntityのID</param>
		/// <param name="_json">デシリアライズに使うJSON</param>
		void Deserialize(EntityId _entityId, const nlohmann::json& _json) override;
		/// <summary>
		/// コンポーネントのデータをコピーする
		/// </summary>
		/// <param name="_dest">コピー先</param>
		/// <param name="_src">コピー元</param>
		void Copy(EntityId _dest, EntityId _src) override;
		ComponentT* Reuse(size_t _index, EntityId _entityId);
		/// <summary>
		/// コンポーネントを作成/取得する
		/// </summary>
		/// <param name="_entityId">エンティティId</param>
		/// <returns>コンポーネントの参照(確実に存在する)</returns>
		template <typename... Args> ComponentT& Get(EntityId _entityId, Args&&... _args);

		/// <summary>
		/// コンポーネント取得を試す。存在しない場合は失敗
		/// </summary>
		/// <param name="_pComponent">取得したコンポーネントを格納するポインタ</param>
		/// <param name="_entityId">取得を試すEntityのID</param>
		/// <returns>成功したらtrue/失敗したらfalse</returns>
		bool TryGet(ComponentT*& _pComponent, EntityId _entityId);
		/// <summary>
		/// コンポーネント取得を試す。存在しない場合は失敗
		/// </summary>
		/// <param name="_pComponent">取得したコンポーネントを格納ポインタ配列</param>
		/// <param name="_entityId">取得を試すEntityのID</param>
		/// <returns>成功したらtrue/失敗したらfalse</returns>
		bool TryGet(std::vector<ComponentT*>* _pComponents, EntityId _entityId);

		/// <summary>
		/// エンティティからコンポーネントを取り除く
		/// </summary>
		/// <param name="_entityId">エンティティId</param>
		void Remove(EntityId _entityId) override;

	  protected:
		std::vector<ComponentT> pool_; // コンポーネントそのものを格納するプール
		std::vector<EntityId> poolId_; // コンポーネントの登録エンティティId
	  private:
		/// <summary>
		/// エンティティを登録する
		/// </summary>
		/// <param name="_entityId"></param>
		void RegisterComponent(EntityId _entityId, size_t _poolIndex);
		/// <summary>
		/// ComponentにRegisterImGuiを持っている場合、それを呼ぶ
		/// </summary>
		void RegisterImGuiIfExists();
	};

	template <typename ComponentT, typename DerivedT>
	inline ComponentPool<ComponentT, DerivedT>::ComponentPool()
		: pool_ {}
		, poolId_ {}
	{
		pool_.reserve(COMPONENT_CAPACITY);
		poolId_.reserve(COMPONENT_CAPACITY);

		Game::System<ComponentRegistry>().RegisterComponentPoolType(typeid(ComponentT), typeid(DerivedT));
		Game::System<ComponentRegistry>().RegisterComponentKey(ComponentT::TypeName(), typeid(ComponentT));

		RegisterImGuiIfExists();
	}

	template <typename ComponentT, typename DerivedT> inline ComponentPool<ComponentT, DerivedT>::~ComponentPool() {}

	template <typename ComponentT, typename DerivedT> inline void ComponentPool<ComponentT, DerivedT>::Initialize()
	{
		RegisterCurrentScene(
			[&, this]
			{
				Release();
			}
		);
		Start();
	}

	template <typename ComponentT, typename DerivedT> inline void ComponentPool<ComponentT, DerivedT>::Release()
	{
		pool_.clear();
		poolId_.clear();
	}

	template <typename ComponentT, typename DerivedT>
	inline nlohmann::json ComponentPool<ComponentT, DerivedT>::Serialize(EntityId _entityId)
	{
		for (int i = 0; i < poolId_.size(); i++)
		{
			if (poolId_[i] == _entityId)
			{
				ComponentT& component = pool_[i];
				component.OnPreSave();

				nlohmann::json j;
				j[ComponentT::TypeName()] = JsonConverter::template Serialize<ComponentT>(component);
				return j;
			}
		}
		return nlohmann::json {};
	}

	template <typename ComponentT, typename DerivedT>
	inline IComponentMemento* ComponentPool<ComponentT, DerivedT>::SaveToMemento(EntityId _entityId)
	{
		for (int i = 0; i < poolId_.size(); i++)
		{
			if (poolId_[i] == _entityId)
			{
				return pool_[i].SaveToMemento();
			}
		}
		return nullptr;
	}

	template <typename ComponentT, typename DerivedT>
	inline void ComponentPool<ComponentT, DerivedT>::Deserialize(EntityId _entityId, const nlohmann::json& _json)
	{
		ComponentT& component = Get(_entityId);
		JsonConverter::template Deserialize<ComponentT>(component, _json.at(ComponentT::TypeName()));

		Command* cmd = new AddComponentCommandGeneric<ComponentT>(component);
		Game::System<CommandHistoryManager>().ExecuteCommand(cmd);
	}

	template <typename ComponentT, typename DerivedT>
	inline void ComponentPool<ComponentT, DerivedT>::Copy(EntityId _dest, EntityId _src)
	{
		if (_dest == _src)
			return;

		ComponentT& destCom = Get(_dest);
		ComponentT& srcCom	= Get(_src);

		destCom			  = srcCom;
		destCom.entityId_ = _dest;
		destCom.OnChangeEntityId();
		srcCom.entityId_ = _src;
		srcCom.OnChangeEntityId();
	}
	template <typename ComponentT, typename DerivedT>
	inline ComponentT* ComponentPool<ComponentT, DerivedT>::Reuse(size_t _index, EntityId _entityId)
	{
		if (poolId_.size() <= _index)
			return nullptr;
		if (poolId_[_index] != INVALID_ENTITY)
			return nullptr;

		poolId_[_index] = _entityId;

		// EntityIdに割り当てられたComponentとして登録
		Game::System<ComponentRegistry>().RegisterComponent(_entityId, std::type_index(typeid(ComponentT)));

		ComponentT* pComponent = &pool_[_index];
		pComponent->entityId_  = _entityId;
		pComponent->OnChangeEntityId();
		return pComponent;
	}
	template <typename ComponentT, typename DerivedT>
	template <typename... Args>
	inline ComponentT& ComponentPool<ComponentT, DerivedT>::Get(EntityId _entityId, Args&&... _args)
	{
		massert(_entityId != INVALID_ENTITY && "無効なEntityIdが渡されました");

		for (int i = 0; i < poolId_.size(); i++)
		{
			if (poolId_[i] == _entityId)
			{
				return pool_[i];
			}
		}

		// 再利用できる箇所を探す
		for (int i = 0; i < poolId_.size(); i++)
		{
			if (poolId_[i] == INVALID_ENTITY)
			{
				poolId_[i] = _entityId;
				pool_[i]   = ComponentT { _entityId, std::forward<Args>(_args)... };
				pool_[i].Initialize();
				RegisterComponent(_entityId, i);
				return pool_[i];
			}
		}

		massert(poolId_.size() + 1 <= COMPONENT_CAPACITY && "プールがキャパシティを超過しました");

		// 再利用できないなら新たに追加
		poolId_.push_back(_entityId);
		pool_.emplace_back(_entityId, std::forward<Args>(_args)...);
		// 追加したら初期化処理
		pool_.back().Initialize();
		RegisterComponent(_entityId, pool_.size() - 1);
		return pool_.back(); // 追加&&初期化したコンポーネントを返す
	}

	template <typename ComponentT, typename DerivedT>
	inline void ComponentPool<ComponentT, DerivedT>::RegisterImGuiIfExists()
	{
		if constexpr (detail::HasRegisterImGui<ComponentT>)
		{
			ComponentT::RegisterImGui();
		}
	}

	template <typename ComponentT, typename DerivedT>
	inline bool ComponentPool<ComponentT, DerivedT>::TryGet(ComponentT*& _pComponent, EntityId _entityId)
	{
		for (int i = 0; i < poolId_.size(); i++)
		{
			if (poolId_[i] == _entityId)
			{
				_pComponent = &pool_[i]; // Idが一致した添字のコンポーネントを返す
				return true;
			}
		}

		return false;
	}

	template <typename ComponentT, typename DerivedT>
	inline bool ComponentPool<ComponentT, DerivedT>::TryGet(std::vector<ComponentT*>* _pComponents, EntityId _entityId)
	{
		_pComponents->clear();
		for (int i = 0; i < poolId_.size(); i++)
		{
			if (poolId_[i] == _entityId)
			{
				_pComponents->push_back(&pool_[i]); // Idが一致した添字のコンポーネントを追加する
			}
		}

		// 何かしら追加されたなら見つかっている
		return _pComponents->size() >= 0;
	}

	template <typename ComponentT, typename DerivedT>
	inline void ComponentPool<ComponentT, DerivedT>::Remove(EntityId _entityId)
	{
		for (int i = 0; i < poolId_.size(); i++)
		{
			if (poolId_[i] == _entityId)
			{
				// 見つかったなら無効Idにする
				poolId_[i] = INVALID_ENTITY;

				// 登録解除
				Game::System<ComponentRegistry>().UnRegisterComponent(_entityId, std::type_index(typeid(ComponentT)));

				pool_[i].Reset();
				return;
			}
		}
	}
	template <typename ComponentT, typename DerivedT>
	inline void ComponentPool<ComponentT, DerivedT>::RegisterComponent(EntityId _entityId, size_t _poolIndex)
	{
		// インデックスを記録

		Game::System<ComponentRegistry>()
			.RegisterComponentIndex(_entityId, std::type_index(typeid(ComponentT)), _poolIndex);

		// EntityIdに割り当てられたComponentとして登録
		Game::System<ComponentRegistry>().RegisterComponent(_entityId, std::type_index(typeid(ComponentT)));
	}
} // namespace mtgb
