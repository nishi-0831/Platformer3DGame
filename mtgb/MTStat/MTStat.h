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
	/// <para> ポリモルフィズムを抜きにしたステートクラス </para>
	/// <para> メソッドチェーンで状態を記述することができる</para>
	/// </summary>
	/// <typeparam name="StatEnumT">ステートに使用する列挙型</typeparam>
	template<EnumT StatEnumT>
	class MTStat
	{
	public:
		MTStat() : stat_{} {}
		~MTStat() {}

		MTStat& OnStart(const StatEnumT _statEnum, const std::function<void()>& _callback);
		MTStat& OnUpdate(const StatEnumT _statEnum, const std::function<void()>& _callback);
		MTStat& OnEnd(const StatEnumT _statEnum, const std::function<void()>& _callback);

		// どの状態でも呼ばれる共通関数
		MTStat& OnAnyStart(const std::function<void()>& _callback);
		MTStat& OnAnyUpdate(const std::function<void()>& _callback);
		MTStat& OnAnyEnd(const std::function<void()>& _callback);

		/// <summary>
		/// <para> 指定した状態から別の状態への遷移条件を登録 </para>
		/// <para> 登録順で評価され、優先度は付けられない </para>
		/// </summary>
		/// <param name="_from">遷移元となる状態。この状態にいるときに遷移条件が評価される </param>
		/// <param name="_to">遷移先となる状態。条件が満たされたときにこの状態に遷移する</param>
		/// <param name="_callback">遷移条件を判定するコールバック。trueを返すと遷移する</param>
		/// <returns></returns>
		MTStat& RegisterTransition(StatEnumT _from, StatEnumT _to,const std::function<bool()>& _callback);
		
		/// <summary>
		/// <para> あらゆる状態から別の状態への遷移条件を登録 </para>
		/// <para> 登録順で評価され、優先度は付けられない </para>
		/// </summary>
		/// <param name="_to">遷移先となる状態。条件が満たされたときにこの状態に遷移する</param>
		/// <param name="_callback">条件が満たされたときにこの状態に遷移する</param>
		/// <returns></returns>
		MTStat& RegisterAnyTransition(StatEnumT _to, const std::function<bool()>& _callback);
		
		/// <summary>
		/// <para> 遷移条件を満たした状態の取得を試みる </para>
		/// <para> 条件を満たしている場合 trueが返ってきて、引数には遷移先の状態が格納される</para>
		/// </summary>
		/// <param name="_nextState"> 遷移可能な状態があればその値が格納される。戻り値がtrueの場合のみ有効 </param>
		/// <returns> 遷移可能な条件があればtrue、なければfalse </returns>
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
		StatEnumT stat_;  // 現在のステート
		
		std::map<StatEnumT, std::function<void()>> updateFuncs_;  // 登録されている更新関数
		std::map<StatEnumT, std::function<void()>> startFuncs_;   // 登録されている開始関数
		std::map<StatEnumT, std::function<void()>> endFuncs_;     // 登録されている終了関数

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
		return *this;
	}

	template<EnumT StatEnumT>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::RegisterAnyTransition(StatEnumT _to, const std::function<bool()>& _callback)
	{
		anyTransition_.emplace_back(_to, _callback);
	}

	template<EnumT StatEnumT>
	inline bool MTStat<StatEnumT>::TryGetNextState(StatEnumT& _nextState)
	{
		if(transitionsMap_.count(stat_))
		{ 
			for (auto transition : transitionsMap_.at(stat_))
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
