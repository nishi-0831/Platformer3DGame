#pragma once
//#include "GameScene.h"
#include <cmtgb.h>
#include <string>
#include <functional>
#include <cstdint>
#include <map>
#include <list>
#include <cassert>
#include <typeinfo>
#include <string_view>
//#include <type_traits>
#include "ISystem.h"
#include "Vector2Int.h"
#include <typeindex>
#include "ReleaseUtility.h"
#include <nlohmann/json.hpp>
namespace mtgb
{
	using EntityId = int64_t;
	class IComponentPool;

	/// <summary>
	/// ���̃N���X���p�����A���Q�[���̃N���X������Ă�������
	/// </summary>
	class Game
	{
	protected:
		/// <summary>
		/// �V�X�e���̍X�V�^�C�~���O�^�C�v
		/// </summary>
		enum struct SystemUpdateType : int8_t
		{
			DontCallMe,  // �Ăяo���Ȃ��ŁI
			Cycle,  // ���T�C�N��
			Frame,  // ���t���[��
			Fixed,  // ���̊Ԋu
		};

		/// <summary>
		/// �V�X�e���o�^�̃R�[���o�b�N�֐�
		/// </summary>
		using RegisterSystem = std::function<void(
			std::type_index _systemTypeName,
			ISystem* _system,
			const bool _isComponentPool,
			const SystemUpdateType)>;

		/// <summary>
		/// �V�X�e���o�^�̊֐��z���_
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
		/// <summary>
		/// <para>�C�ӂ̍X�V���ԂŃV�X�e����ݒ肷��</para>
		/// <para>�g���V�X�e����K�v�Ȃ������̊֐�����new���Ă�������</para>
		/// </summary>
		virtual void SetupSystems(const RegisterSystemFuncHolder& _registerSystem) = 0;
		/// <summary>
		/// �Q�[���̃o�[�W������K���Ԃ��Ă�������
		/// </summary>
		/// <returns></returns>
		virtual std::string_view GetVersion() const = 0;
		/// <summary>
		/// �Q�[���̃^�C�g����K���Ԃ��Ă�������
		/// </summary>
		/// <returns></returns>
		virtual std::string_view GetTitle() const = 0;

		virtual Vector2Int GetScreenSize() const;

	private:
		std::map<std::type_index, ISystem*> pRegisterSystems_;  // �o�^�ς݂̃V�X�e��
		std::list<ISystem*> pCycleUpdateSystems_;  // ���T�C�N���X�V�����V�X�e��
		std::list<ISystem*> pFrameUpdateSystems_;  // ���t���[���X�V�����V�X�e��
		std::list<ISystem*> pFixedUpdateSystems_;  // �����ԂōX�V�����V�X�e��
		std::vector<IComponentPool*> pComponentPools_;    // �R���|�[�l���g�v�[���̃V�X�e��

	public:
		/// <summary>
		/// �Q�[�����N������
		/// </summary>
		/// <typeparam name="GameT"></typeparam>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="..._args"></param>
		template<typename GameT, typename ...Args>
		static void Run(Args... _args);

		/// <summary>
		/// �Q�[�����I������
		/// </summary>
		static void Exit();

		/// <summary>
		/// �V�X�e�����擾����
		/// </summary>
		/// <typeparam name="SystemT">�V�X�e���̌^</typeparam>
		/// <returns>�V�X�e���̎Q�ƃ|�C���^</returns>
		template<typename SystemT>
		static inline SystemT& System();

		/// <summary>
		/// �Q�[���̃o�[�W�������擾
		/// </summary>
		/// <returns>�o�[�W�������̕�����</returns>
		static inline std::string_view Version() { return pInstance_->GetVersion(); }
		/// <summary>
		/// �Q�[���^�C�g�����擾
		/// </summary>
		/// <returns>�Q�[���^�C�g���̕�����</returns>
		static inline std::string_view Title() { return pInstance_->GetTitle(); }

		/// <summary>
		/// �t���[���̍X�V������
		/// </summary>
		static void UpdateFrame();
		/// <summary>
		/// ����X�V������
		/// </summary>
		static void UpdateFixed();

		/// <summary>
		/// �G���e�B�e�B�ɑΉ�����R���|�[�l���g���폜����
		/// </summary>
		/// <param name="_entityId"></param>
		static void RemoveEntityAllComponent(const EntityId _entityId);

		static void RemoveEntityComponent(const std::type_index _typeIndex, EntityId _entityId);
		
		static nlohmann::json SerializeComponent(std::type_index _typeIndex, EntityId _entityId);
	private:
		/// <summary>
		/// �V�X�e���̏�����������
		/// </summary>
		static void InitializeSystems(const std::list<ISystem*>& _uninitialized);

		/// <summary>
		/// �V�X�e���̉������������
		/// </summary>
		static void ReleaseSystems(const std::list<ISystem*>& _runnings);

		/// <summary>
		/// �Q�[���T�C�N���̃��[�v���J�n
		/// </summary>
		static void RunLoopGameCycle();

		static Game* pInstance_;  // �B��̃Q�[���C���X�^���X
		static bool toExit_;  // �I���t���O
	};

	template<typename GameT, typename ...Args>
	inline void Game::Run(Args... _args)
	{
		// �Q�[�����C���X�^���X����
		pInstance_ = new GameT{ _args... };

		std::list<ISystem*> systems{};

		// �e�V�X�e���̓o�^
		pInstance_->SetupSystems(
		{
			[&](
				std::type_index _systemType,
				ISystem* _pSystem,
				const bool _isComponentPool,
				const SystemUpdateType _systemUpdateType)
			{
				assert(_pSystem != nullptr
					&& "ISystem�ւ̃A�b�v�L���X�g�Ɏ��s�A�p���֌W��public�ɂȂ��Ă��邩�m�F�B");

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

		// �e�V�X�e���̏�����
		pInstance_->InitializeSystems(systems);

		// �Q�[���T�C�N���J�n
		pInstance_->RunLoopGameCycle();

		// �������
		pInstance_->ReleaseSystems(systems);
		systems.clear();

		
		// �C���X�^���X�̉��
		delete pInstance_;
	}

	template<typename SystemT>
	inline SystemT& Game::System()
	{
		const char* t = typeid(SystemT).name();

		ISystem* pSystem{ pInstance_->pRegisterSystems_[typeid(SystemT)] };

		assert(pSystem != nullptr);

		// ���N���X��ISystem�ł��邩
		static_assert(std::is_base_of<ISystem, SystemT>().value
			&& "ISystem�N���X���p�����Ă��Ȃ��N���X�̃C���X�^���X�͎擾�ł��܂���B");
		return *(dynamic_cast<SystemT*>(pSystem));
	}
}
