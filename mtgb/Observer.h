#pragma once
#include <cmtgb.h>
#include <functional>
#include <list>

namespace mtgb
{
	/// <summary>
	/// �C�x���g���Ď��A�܂Ƃ߂Ď��s����N���X
	/// </summary>
	/// <typeparam name="...CallArgs"></typeparam>
	template<typename ...CallArgs>
	class Observer
	{
	public:
		using EventHandle = void*;
	public:
		Observer();
		virtual ~Observer();

	protected:
		void On(const std::function<void(CallArgs...)>& _callback);
		void Invoke(CallArgs... args);
		void Remove(const EventHandle hEvent);

	private:
		std::list<std::function<void(CallArgs...)>> functions_;
	};

	template<typename ...CallArgs>
	inline Observer<CallArgs...>::Observer()
	{
	}
	template<typename ...CallArgs>
	inline Observer<CallArgs...>::~Observer()
	{
	}
	template<typename ...CallArgs>
	inline void Observer<CallArgs...>::On(const std::function<void(CallArgs...)>& _callback)
	{
		functions_.push_back(_callback);
	}
	template<typename ...CallArgs>
	inline void Observer<CallArgs...>::Invoke(CallArgs ...args)
	{
		// ���ׂČĂяo��
		for (auto& func : functions_)
		{
			func();
		}
	}

	template<typename ...CallArgs>
	inline void Observer<CallArgs...>::Remove(const EventHandle _hEvent)
	{
		for (auto itr = functions_.begin(); itr != functions_.end(); itr++)
		{
			if (&(*itr) == _hEvent)
			{
				functions_.erase(itr);
				return;
			}
		}
	}
}
