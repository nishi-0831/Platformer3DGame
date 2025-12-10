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
namespace mtgb
{
	static constexpr size_t COMPONENT_CAPACITY{ 8192 };

	namespace detail
	{
		template<typename T,typename = void>
		struct has_register_imgui : std::false_type {};

		template<typename T>
		struct has_register_imgui<T,std::void_t<decltype(T::RegisterImGui())>>
			: std::true_type{ };

		template<typename T>
		inline constexpr bool has_register_imgui_v = has_register_imgui<T>::value;
	}
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
		using Component = ComponentT;
		ComponentPool();
		virtual ~ComponentPool();

		void Initialize() override;
		virtual void Start() {};
		virtual void Update() override {}

		void Release() override;
		nlohmann::json Serialize(EntityId _entityId) override;
		IComponentMemento* SaveToMemento(EntityId _entityId) override;
		void Deserialize(EntityId _entityId, const nlohmann::json& _json) override;
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
	private:
		template<typename T = ComponentT>
		std::enable_if_t<detail::has_register_imgui_v<T>> RegisterImGuiIfExists();
		template<typename T = ComponentT>
		std::enable_if_t<!detail::has_register_imgui_v<T>> RegisterImGuiIfExists();
	};

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline ComponentPool<ComponentT, DerivedT, IsSingleton>::ComponentPool() :
		pool_{},
		poolId_{}
	{
		pool_.reserve(COMPONENT_CAPACITY);
		poolId_.reserve(COMPONENT_CAPACITY);

		Game::System<ComponentRegistry>().RegisterComponentPoolType(typeid(ComponentT), typeid(DerivedT));
		Game::System<ComponentRegistry>().RegisterComponentKey(ComponentT::TypeName(), typeid(ComponentT));

		RegisterImGuiIfExists();
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
				ComponentT& component = pool_[i];
				component.OnPreSave();
				nlohmann::json j = JsonConverter::template Serialize<ComponentT>(component);
				return j;
			}
		}
		return nlohmann::json{};
	}

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline IComponentMemento* ComponentPool<ComponentT, DerivedT, IsSingleton>::SaveToMemento(EntityId _entityId)
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

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline void ComponentPool<ComponentT, DerivedT, IsSingleton>::Deserialize(EntityId _entityId, const nlohmann::json& _json)
	{
		ComponentT& component = Get(_entityId);
		JsonConverter::template Deserialize<ComponentT>(component, _json.at(ComponentT::TypeName()));
		
		Command* cmd = new AddComponentCommandGeneric<ComponentT>(component);
		Game::System<CommandHistoryManager>().ExecuteCommand(cmd);
	}

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline void ComponentPool<ComponentT, DerivedT, IsSingleton>::Copy(EntityId _dest, EntityId _src)
	{
		if (_dest == _src)
			return;

		ComponentT& destCom = Get(_dest);
		ComponentT& srcCom = Get(_src);
		
		destCom = srcCom;
	}

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	inline ComponentT* ComponentPool<ComponentT, DerivedT, IsSingleton>::Reuse(size_t _index, EntityId _entityId)
	{
		if (poolId_.size() <= _index)
			return nullptr;
		if (poolId_[_index] != INVALID_ENTITY)
			return nullptr;

		poolId_[_index] = _entityId;

		// EntityIdに割り当てられたComponentとして登録
		Game::System<ComponentRegistry>().RegisterComponent(_entityId, std::type_index(typeid(ComponentT)));

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
	template<typename T>
	inline std::enable_if_t<detail::has_register_imgui_v<T>> ComponentPool<ComponentT, DerivedT, IsSingleton>::RegisterImGuiIfExists()
	{
		ComponentT::RegisterImGui();
	}

	template<typename ComponentT, typename DerivedT, bool IsSingleton>
	template<typename T>
	inline std::enable_if_t<!detail::has_register_imgui_v<T>> ComponentPool<ComponentT, DerivedT, IsSingleton>::RegisterImGuiIfExists()
	{
		// 何も行わない
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
