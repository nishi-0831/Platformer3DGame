#pragma once
#include <functional>
#include <map>
#include <concepts>
#include <type_traits>
#include <vector>

namespace mtstat
{
	template<typename T>
	concept EnumT = std::is_enum_v<T>;
	
	/// <summary>
	/// �|�������t�B�Y���𔲂��ɂ����X�e�[�g�N���X
	/// </summary>
	/// <typeparam name="StatEnumT">�X�e�[�g�Ɏg�p����񋓌^</typeparam>
	template<EnumT StatEnumT>
	class MTStat
	{
	public:
		MTStat() : stat_{} {}
		~MTStat() {}

		MTStat& OnStart(const StatEnumT _statEnum, const std::function<void()>& _callback);
		MTStat& OnUpdate(const StatEnumT _statEnum, const std::function<void()>& _callback);
		MTStat& OnEnd(const StatEnumT _statEnum, const std::function<void()>& _callback);

		// �ǂ̏�Ԃł��Ă΂�鋤�ʊ֐�
		MTStat& OnAnyStart(const std::function<void()>& _callback);
		MTStat& OnAnyUpdate(const std::function<void()>& _callback);
		MTStat& OnAnyEnd(const std::function<void()>& _callback);

		/// <summary>
		/// <para> ���̏�Ԃ��J�ډ\�ƂȂ������o�^ </para>
		/// <para> �ǂ̏�ԂɑJ�ڂł��邩�͎w��ł��Ȃ� </para>
		/// <para> �o�^�ł�������͈�̂� </para>
		/// </summary>
		/// <param name="_statEnum">�J�ډ\�ƂȂ������o�^������</param>
		/// <param name="_callback">bool��Ԃ��R�[���o�b�N</param>
		/// <returns></returns>
		MTStat& RegisterTransition(StatEnumT _from, StatEnumT _to,const std::function<bool()>& _callback);
		
		MTStat& RegisterAnyTransition(StatEnumT _to, const std::function<bool()>& _callback);
		bool TryGetNextState(StatEnumT& _nextState);
		void Update() const;
		void Change(const StatEnumT _nextStat);

		const StatEnumT Current() const { return stat_; }

		struct StateTransition
		{
			StatEnumT toState;
			std::function<bool()> condition;
		};
	private:
		StatEnumT stat_;  // ���݂̃X�e�[�g
		
		std::map<StatEnumT, std::function<void()>> updateFuncs_;  // �o�^����Ă���X�V�֐�
		std::map<StatEnumT, std::function<void()>> startFuncs_;   // �o�^����Ă���J�n�֐�
		std::map<StatEnumT, std::function<void()>> endFuncs_;     // �o�^����Ă���I���֐�

		std::function<void()> anyUpdateFunc_;
		std::function<void()> anyStartFunc_;
		std::function<void()> anyEndFunc_;
		
		
		std::map<StatEnumT, std::vector<StateTransition>> transitionsMap_;
		std::vector<StateTransition> anyTransition_;
	};

	template<EnumT StatEnumT>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::OnStart(const StatEnumT _statEnum, const std::function<void()>& _callback)
	{
		startFuncs_.insert({ _statEnum, _callback });
		return *this;
	}

	template<EnumT StatEnumT>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::OnUpdate(const StatEnumT _statEnum, const std::function<void()>& _callback)
	{
		updateFuncs_.insert({ _statEnum, _callback });
		return *this;
	}

	template<EnumT StatEnumT>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::OnEnd(const StatEnumT _statEnum, const std::function<void()>& _callback)
	{
		endFuncs_.insert({ _statEnum, _callback });
		return *this;
	}

	template<EnumT StatEnumT>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::OnAnyStart(const std::function<void()>& _callback)
	{
		anyStartFunc_ = _callback;
		return *this;
	}

	template<EnumT StatEnumT>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::OnAnyUpdate(const std::function<void()>& _callback)
	{
		anyUpdateFunc_ = _callback;
		return *this;
	}

	template<EnumT StatEnumT>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::OnAnyEnd(const std::function<void()>& _callback)
	{
		anyEndFunc_ = _callback;
		return *this;
	}

	template<EnumT StatEnumT>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::RegisterTransition(StatEnumT _from, StatEnumT _to,const std::function<bool()>& _callback)
	{
		transitionsMap_[_from].emplace_back(_to,_callback);
	}

	template<EnumT StatEnumT>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::RegisterAnyTransition(StatEnumT _to, const std::function<bool()>& _callback)
	{
		anyTransition_.emplace_back(_to, _callback);
	}

	template<EnumT StatEnumT>
	inline bool MTStat<StatEnumT>::TryGetNextState(StatEnumT& _nextState)
	{
		for (auto transitions : transitionsMap_)
		{
			for (auto transition : transitions.second)
			{
				if (transition.condition())
				{
					_nextState = transition.toState;
					return true;
				}
			}
		}
		return false;
	}

	template<EnumT StatEnumT>
	inline void MTStat<StatEnumT>::Update() const
	{
		if (anyUpdateFunc_)
		{
			anyUpdateFunc_();
		}
		if (updateFuncs_.count(stat_))
		{
			updateFuncs_.at(stat_)();
		}
	}
	
	template<EnumT StatEnumT>
	inline void MTStat<StatEnumT>::Change(const StatEnumT _nextStat)
	{
		if (anyEndFunc_)
		{
			anyEndFunc_();
		}
		if (endFuncs_.count(stat_))
		{
			endFuncs_[stat_]();
		}

		stat_ = _nextStat;

		if (anyStartFunc_)
		{
			anyStartFunc_();
		}
		if (startFuncs_.count(_nextStat))
		{
			startFuncs_[_nextStat]();
		}
	}
}
