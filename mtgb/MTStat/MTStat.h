#pragma once
#include <functional>
#include <map>
#include <concepts>
#include <type_traits>
#include <vector>

namespace mtstat
{
	template <typename T>
	concept EnumT = std::is_enum_v<T>;

	/// <summary>
	/// 特定のタイミング(start,update,end)で呼ばれるコールバックの型
	/// </summary>
	using Callback = std::function<void(void)>;

	template<typename Func>
	concept MTStatCallback = std::is_convertible_v<Func, Callback>;
	
	/// <summary>
	/// 遷移条件となるコールバックの型。trueを返すと遷移する
	/// </summary>
	using TransitionCallback = std::function<bool()>;

	template <typename Func>
	concept MTStatTransitionCallback = std::is_convertible_v<Func, TransitionCallback>;

	/// <summary>
	/// <para> ポリモルフィズムを抜きにしたステートクラス </para>
	/// <para> メソッドチェーンで状態を記述することができる</para>
	/// </summary>
	/// <typeparam name="StatEnumT">ステートに使用する列挙型</typeparam>
	template <EnumT StatEnumT> class MTStat
	{
	  public:

		MTStat()
			: stat_ {}
		{
		}
		~MTStat() {}

		template <MTStatCallback Func> MTStat& OnStart(StatEnumT _statEnum, Func&& _callback);
		template <MTStatCallback Func> MTStat& OnUpdate(StatEnumT _statEnum, Func&& _callback);
		template <MTStatCallback Func> MTStat& OnEnd(StatEnumT _statEnum, Func&& _callback);

		// どの状態でも呼ばれる共通関数
		template <MTStatCallback Func> MTStat& OnAnyStart(Func&& _callback);
		template <MTStatCallback Func> MTStat& OnAnyUpdate(Func&& _callback);
		template <MTStatCallback Func> MTStat& OnAnyEnd(Func&& _callback);

		/// <summary>
		/// <para> 指定した状態から別の状態への遷移条件を登録 </para>
		/// <para> 登録順で評価され、優先度は付けられない </para>
		/// </summary>
		/// <param name="_from">遷移元となる状態。この状態にいるときに遷移条件が評価される </param>
		/// <param name="_to">遷移先となる状態。条件が満たされたときにこの状態に遷移する</param>
		/// <param name="_callback">遷移条件を判定するコールバック。trueを返すと遷移する</param>
		/// <returns></returns>
		template<MTStatTransitionCallback Func>
		MTStat& RegisterTransition(StatEnumT _from, StatEnumT _to, Func&& _callback);

		/// <summary>
		/// <para> あらゆる状態から別の状態への遷移条件を登録 </para>
		/// <para> 登録順で評価され、優先度は付けられない </para>
		/// </summary>
		/// <param name="_to">遷移先となる状態。条件が満たされたときにこの状態に遷移する</param>
		/// <param name="_callback">遷移条件を判定するコールバック。trueを返すと遷移する</param>
		/// <returns></returns>
		template<MTStatTransitionCallback Func>
		MTStat& RegisterAnyTransition(StatEnumT _to, Func&& _callback);

		/// <summary>
		/// <para> 遷移条件を満たした状態の取得を試みる </para>
		/// <para> 条件を満たしている場合 trueが返ってきて、引数には遷移先の状態が格納される</para>
		/// </summary>
		/// <param name="_nextState"> 遷移可能な状態があればその値が格納される。戻り値がtrueの場合のみ有効 </param>
		/// <returns> 遷移可能な条件があればtrue、なければfalse </returns>
		bool TryGetNextState(StatEnumT& _nextState) const;
		void Update() const;
		void Change(StatEnumT _nextStat);

		StatEnumT Current() const
		{
			return stat_;
		}

		struct StateTransition
		{
			StatEnumT toState;
			TransitionCallback condition;
		};

	  private:
		StatEnumT stat_; // 現在のステート

		std::unordered_map<StatEnumT, Callback> updateFuncs_; // 登録されている更新関数
		std::unordered_map<StatEnumT, Callback> startFuncs_;  // 登録されている開始関数
		std::unordered_map<StatEnumT, Callback> endFuncs_;	   // 登録されている終了関数

		Callback anyUpdateFunc_;
		Callback anyStartFunc_;
		Callback anyEndFunc_;

		std::unordered_map<StatEnumT, std::vector<StateTransition>> transitionsMap_;
		std::vector<StateTransition> anyTransition_;
	};

	template <EnumT StatEnumT>
	template <MTStatTransitionCallback Func>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::RegisterTransition(
		StatEnumT _from,
		StatEnumT _to,
		Func&& _callback
	)
	{
		transitionsMap_[_from].emplace_back(_to, std::forward<Func>(_callback));
		return *this;
	}

	template <EnumT StatEnumT>
	template <MTStatTransitionCallback Func>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::RegisterAnyTransition(
		StatEnumT _to,
		Func&& _callback
	)
	{
		anyTransition_.emplace_back(_to, std::forward<Func>(_callback));
	}

	template <EnumT StatEnumT> inline bool MTStat<StatEnumT>::TryGetNextState(StatEnumT& _nextState) const
	{
		if (transitionsMap_.count(stat_))
		{
			const std::vector<StateTransition>& transitions = transitionsMap_.at(stat_);
			for (const StateTransition& transition : transitions)
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

	template <EnumT StatEnumT> inline void MTStat<StatEnumT>::Update() const
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

	template <EnumT StatEnumT> inline void MTStat<StatEnumT>::Change(const StatEnumT _nextStat)
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
	template <EnumT StatEnumT>
	template <MTStatCallback Func>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::OnStart(StatEnumT _statEnum, Func&& _callback)
	{
		startFuncs_.emplace( _statEnum, std::forward<Func>(_callback ));
		return *this;
	}
	template <EnumT StatEnumT>
	template <MTStatCallback Func>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::OnUpdate(StatEnumT _statEnum, Func&& _callback)
	{
		updateFuncs_.emplace(_statEnum, std::forward<Func>( _callback ));
		return *this;
	}
	template <EnumT StatEnumT>
	template <MTStatCallback Func>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::OnEnd(StatEnumT _statEnum, Func&& _callback)
	{
		endFuncs_.emplace( _statEnum, std::forward<Func>( _callback ));
		return *this;
	}
	template <EnumT StatEnumT>
	template <MTStatCallback Func>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::OnAnyStart(Func&& _callback)
	{
		anyStartFunc_ = std::forward<Func>( _callback);
		return *this;
	}
	template <EnumT StatEnumT>
	template <MTStatCallback Func>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::OnAnyUpdate(Func&& _callback)
	{
		anyUpdateFunc_ = std::forward<Func>( _callback);
		return *this;
	}
	template <EnumT StatEnumT>
	template <MTStatCallback Func>
	inline MTStat<StatEnumT>& MTStat<StatEnumT>::OnAnyEnd(Func&& _callback)
	{
		anyEndFunc_ = std::forward<Func>(_callback);
		return *this;
	}
} // namespace mtstat
