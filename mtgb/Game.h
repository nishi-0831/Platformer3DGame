#pragma once
//#include "GameScene.h"
#include "Entity.h"
#include <string>
#include <functional>
#include <cstdint>
#include <map>
#include <list>
#include <cassert>
#include <typeinfo>
#include <string_view>
#include <type_traits>
#include "ISystem.h"
#include "Vector2Int.h"
#include <typeindex>
#include "ReleaseUtility.h"
#include "IComponentMemento.h"
#include <nlohmann/json.hpp>
#include "IRenderable.h"
#include "ComponentFactory.h"
namespace mtgb
{
	//using EntityId = int64_t;
	class IComponentPool;

	/// <summary>
	/// このクラスを継承し、作るゲームのクラスを作ってください
	/// </summary>
	class Game
	{
	protected:
		/// <summary>
		/// システムの更新タイミングタイプ
		/// </summary>
		enum struct SystemUpdateType : int8_t
		{
			DontCallMe,  // 呼び出さないで！
			Cycle,  // 毎サイクル
			Frame,  // 毎フレーム
			Fixed,  // 一定の間隔
		};

		/// <summary>
		/// システム登録のコールバック関数
		/// </summary>
		using RegisterSystem = std::function<void(
			std::type_index _systemTypeName,
			ISystem* _system,
			const bool _isComponentPool,
			const SystemUpdateType)>;

		/// <summary>
		/// システム登録の関数ホルダ
		/// </summary>
		class RegisterSystemFuncHolder
		{
		public:
			RegisterSystemFuncHolder(RegisterSystem _function) :
				function_{ _function }
			{}

			template<typename SystemT>
			void Set(SystemUpdateType _type, const bool _isComponentCP = false) const
			{

				function_(
					typeid(SystemT),
					dynamic_cast<ISystem*>(new SystemT{}),
					_isComponentCP,
					_type);
			}

		private:
			RegisterSystem function_;
		};

	protected:
		Game();
		virtual ~Game();

	protected:
		template<typename T>
		void Set(SystemUpdateType _systemUpdateType);
		/// <summary>
		/// <para>任意の更新順番でシステムを設定する</para>
		/// <para>使うシステムを必要なだけこの関数内でnewしてください</para>
		/// </summary>
		virtual void SetupSystems(const RegisterSystemFuncHolder& _registerSystem) = 0;
		/// <summary>
		/// ゲームのバージョンを必ず返してください
		/// </summary>
		/// <returns></returns>
		virtual std::string_view GetVersion() const = 0;
		/// <summary>
		/// ゲームのタイトルを必ず返してください
		/// </summary>
		/// <returns></returns>
		virtual std::string_view GetTitle() const = 0;

		virtual Vector2Int GetScreenSize() const;

	private:
		std::map<std::type_index, ISystem*> pRegisterSystems_;  // 登録済みのシステム
		std::list<ISystem*> pCycleUpdateSystems_;  // 毎サイクル更新されるシステム
		std::list<ISystem*> pFrameUpdateSystems_;  // 毎フレーム更新されるシステム
		std::list<ISystem*> pFixedUpdateSystems_;  // 一定期間で更新されるシステム
		std::vector<IComponentPool*> pComponentPools_;    // コンポーネントプールのシステム
		std::vector<IRenderableCP*> pRenderablePools_; // 描画可能なコンポーネントプールのシステム
		std::vector<std::type_index> registerOrder_; // 登録順を保持する配列

		ComponentFactory componentFactory_;
	public:
		/// <summary>
		/// ゲームを起動する
		/// </summary>
		/// <typeparam name="GameT"></typeparam>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="..._args"></param>
		template<typename GameT, typename ...Args>
		static void Run(Args... _args);

		/// <summary>
		/// ゲームを終了する
		/// </summary>
		static void Exit();

		/// <summary>
		/// システムを取得する
		/// </summary>
		/// <typeparam name="SystemT">システムの型</typeparam>
		/// <returns>システムの参照ポインタ</returns>
		template<typename SystemT>
		static inline SystemT& System();

		static ComponentFactory& GetComponentFactory() { return pInstance_->componentFactory_; }
		/// <summary>
		/// ゲームのバージョンを取得
		/// </summary>
		/// <returns>バージョン情報の文字列</returns>
		static inline std::string_view Version() { return pInstance_->GetVersion(); }
		/// <summary>
		/// ゲームタイトルを取得
		/// </summary>
		/// <returns>ゲームタイトルの文字列</returns>
		static inline std::string_view Title() { return pInstance_->GetTitle(); }

		/// <summary>
		/// フレームの更新をする
		/// </summary>
		static void UpdateFrame();
		/// <summary>
		/// 定期更新をする
		/// </summary>
		static void UpdateFixed();

		/// <summary>
		/// コンポーネントプールのインターフェースを返す
		/// </summary>
		/// <param name="_typeIndex"></param>
		/// <returns></returns>
		static IComponentPool* GetCP(std::type_index _typeIndex);
		/// <summary>
		/// エンティティに対応する全てのコンポーネントを削除する
		/// </summary>
		/// <param name="_entityId"></param>
		static void RemoveEntityAllComponent(const EntityId _entityId);

		/// <summary>
		/// エンティティに対応するコンポーネントを削除する
		/// </summary>
		/// <param name="_typeIndex"></param>
		/// <param name="_entityId"></param>
		static void RemoveEntityComponent(const std::type_index _typeIndex, EntityId _entityId);
		
		/// <summary>
		/// エンティティに対応するコンポーネントをJSONにシリアライズして返す
		/// </summary>
		/// <param name="_typeIndex"> コンポーネントプールのtype_index </param>
		/// <param name="_entityId"></param>
		/// <returns></returns>
		static nlohmann::json SerializeComponent(std::type_index _typeIndex, EntityId _entityId);
		/// <summary>
		/// JSONからエンティティに対応するコンポーネントにデシリアライズして、メメントを返す
		/// </summary>
		/// <param name="_typeIndex">コンポーネントプールのtype_index</param>
		/// <param name="_entityId"></param>
		/// <param name="_json"></param>
		/// <returns></returns>
		static void DeserializeComponent(std::type_index _typeIndex, EntityId _entityId,const nlohmann::json& _json);
		static void DeserializeComponents(EntityId _entityId, const nlohmann::json& _json);

		/// <summary>
		/// 描画可能なコンポーネントプールを返す
		/// </summary>
		/// <returns></returns>
		static std::span<IRenderableCP*> GetRenderableCPs();
	private:
		/// <summary>
		/// システムの初期化をする
		/// </summary>
		static void InitializeSystems(const std::list<ISystem*>& _uninitialized);

		/// <summary>
		/// システムの解放処理をする
		/// </summary>
		static void ReleaseSystems(const std::list<ISystem*>& _runnings);

		/// <summary>
		/// ゲームサイクルのループを開始
		/// </summary>
		static void RunLoopGameCycle();

		static Game* pInstance_;  // 唯一のゲームインスタンス
		static bool toExit_;  // 終了フラグ
	};

	template<typename T>
	inline void Game::Set(SystemUpdateType _systemUpdateType)
	{
		static_assert(std::is_base_of_v<ISystem, T>);

		ISystem* pSystem = new T();
		pInstance_->registerOrder_.push_back(typeid(T));

		if constexpr (std::is_base_of_v<IComponentPool, T>)
		{
			pInstance_->pComponentPools_.push_back(dynamic_cast<IComponentPool*>(pSystem));
			pInstance_->componentFactory_.RegisterComponent<typename T::Component>();
		}
		if constexpr (std::is_base_of_v<IRenderableCP, T>)
		{
			pInstance_->pRenderablePools_.push_back(dynamic_cast<IRenderableCP*>(pSystem));
		}

		pInstance_->pRegisterSystems_.insert({ typeid(T),pSystem });
		switch (_systemUpdateType)
		{
		case SystemUpdateType::Cycle:
			pInstance_->pCycleUpdateSystems_.push_back(pSystem);
			break;
		case SystemUpdateType::Frame:
			pInstance_->pFrameUpdateSystems_.push_back(pSystem);
			break;
		case SystemUpdateType::Fixed:
			pInstance_->pFixedUpdateSystems_.push_back(pSystem);
			break;
		case SystemUpdateType::DontCallMe:
		default:
			break;
		}
	}

	template<typename GameT, typename ...Args>
	inline void Game::Run(Args... _args)
	{
		// ゲームをインスタンスする
		pInstance_ = new GameT{ _args... };

		std::list<ISystem*> systems{};

		// 各システムの登録
		pInstance_->SetupSystems(
		{
			[&](
				std::type_index _systemType,
				ISystem* _pSystem,
				const bool _isComponentPool,
				const SystemUpdateType _systemUpdateType)
			{
				assert(_pSystem != nullptr
					&& "ISystemへのアップキャストに失敗、継承関係がpublicになっているか確認。");

				systems.push_back(_pSystem);
				if (_isComponentPool)
				{
					pInstance_->pComponentPools_.push_back(dynamic_cast<IComponentPool*>(_pSystem));
				}
				pInstance_->pRegisterSystems_.insert({ _systemType , _pSystem });
				switch (_systemUpdateType)
				{
				case SystemUpdateType::Cycle:
					pInstance_->pCycleUpdateSystems_.push_back(_pSystem);
					break;
				case SystemUpdateType::Frame:
					pInstance_->pFrameUpdateSystems_.push_back(_pSystem);
					break;
				case SystemUpdateType::Fixed:
					pInstance_->pFixedUpdateSystems_.push_back(_pSystem);
					break;
				case SystemUpdateType::DontCallMe:
				default:
					break;
				}
			}
		});
		// 各システムの初期化
		pInstance_->InitializeSystems(systems);

		// ゲームサイクル開始
		pInstance_->RunLoopGameCycle();

		// 解放処理
		pInstance_->ReleaseSystems(systems);
		systems.clear();

		// インスタンスの解放
		delete pInstance_;
	}

	template<typename SystemT>
	inline SystemT& Game::System()
	{
		const char* t = typeid(SystemT).name();

		ISystem* pSystem{ pInstance_->pRegisterSystems_[typeid(SystemT)] };

		assert(pSystem != nullptr);

		// 基底クラスがISystemであるか
		static_assert(std::is_base_of<ISystem, SystemT>().value
			&& "ISystemクラスを継承していないクラスのインスタンスは取得できません。");
		return *(dynamic_cast<SystemT*>(pSystem));
	}
}
