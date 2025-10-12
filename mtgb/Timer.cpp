#include "Timer.h"
#include "GameTime.h"


TimerHandle mtgb::Timer::AddAram(const float _time, const std::function<void()>& _callback)
{
	QUEUE_ELEMENT* pElement{ new QUEUE_ELEMENT{ _callback, _time } };
	Instance().EnqueueTimer(pElement);
	return reinterpret_cast<TimerHandle>(pElement);
}

TimerHandle mtgb::Timer::AddInterval(const float _time, const std::function<void()>& _callback, const bool _firstCall)
{
	QUEUE_ELEMENT* pElement{ new QUEUE_ELEMENT{ _callback, _firstCall ? 0 : _time } };
	Instance().pReenqueueElements_.insert({ pElement, _time });
	Instance().EnqueueTimer(pElement);
	return reinterpret_cast<TimerHandle>(pElement);
}

void mtgb::Timer::Remove(TimerHandle _hTimer)
{

	QUEUE_ELEMENT* pElement{ reinterpret_cast<QUEUE_ELEMENT*>(_hTimer) };
	Instance().toErase_.insert(pElement);  // �폜�\��ɒǉ�����

	//if (Instance().pReenqueueElements_.count(pElement) > 0)
	//{
	//	Instance().pReenqueueElements_.erase(pElement);
	//}
	//for (auto itr = Instance().pTimerQueue_.begin(); itr != Instance().pTimerQueue_.end();)
	//{
	//	if ((*itr) == pElement)
	//	{
	//		itr = Instance().pTimerQueue_.erase(itr);
	//	}
	//	else
	//	{
	//		itr++;
	//	}
	//}
}

void mtgb::Timer::Clear()
{
	for (auto& pTimer : Instance().pTimerQueue_)
	{
		delete pTimer;
	}

	 Instance().pTimerQueue_.clear();
}

void mtgb::Timer::Release()
{
	Instance().Clear();
	if (pInstance_ != nullptr)
	{
		delete pInstance_;
		pInstance_ = nullptr;
	}
}

void mtgb::Timer::Initialize()
{
}

void mtgb::Timer::Update()
{
	if (pTimerQueue_.size() <= 0)
	{
		return;  // �^�C�}�[�L���[����Ȃ��A
	}

	auto itr = pTimerQueue_.begin();
	while (true)
	{
		(*itr)->timeLeft -= Time::DeltaTimeF();

		float diff{ (*itr)->timeLeft };  // ����
		if (diff > 0)  // ������0���傫�� = �ҋ@��
		{
			break;  // �^�C�}�[�I�����Ă��Ȃ����ߗ��E
		}
		else  // ������0�ȉ� = �^�C�}�[�I��
		{
			(*itr)->on();  // ���s

			// �����g���܂킵����v�f�Ȃ�
			if (pReenqueueElements_.count(*itr) > 0)
			{
				QUEUE_ELEMENT* pElement = *itr;
				(*itr)->timeLeft = pReenqueueElements_[*itr];
				itr = pTimerQueue_.erase(pTimerQueue_.begin());  // ����
				EnqueueTimer(pElement);  // ���ԃZ�b�g���ăG���L���[
				break;
			}
			// �g���܂킳�Ȃ��Ȃ�
			{
				delete (*itr);  // �������
				itr = pTimerQueue_.erase(pTimerQueue_.begin());  // ����
			}
			if (itr == pTimerQueue_.end())
			{
				break;  // �㑱���Ȃ� = �L���[����Ȃ痣�E
			}
			(*itr)->timeLeft += diff;  // �������㑱�ɔ��f
			continue;  // �㑱���I�����Ă���\�������邽�ߌp��
		}
	}
	// �폜�\��̂������
	for (auto itr = pReenqueueElements_.begin(); itr != pReenqueueElements_.end();)
	{
		if (toErase_.count(itr->first))
		{
			for (auto queueItr = pTimerQueue_.begin(); queueItr != pTimerQueue_.end();)
			{
				if ((*queueItr) == itr->first)
				{
					queueItr = pTimerQueue_.erase(queueItr);
				}
				else
				{
					queueItr++;
				}
			}
			itr = pReenqueueElements_.erase(itr);
		}
		else
		{
			itr++;
		}
	}
	toErase_.clear();
}

mtgb::Timer::Timer()
{
	pInstance_ = this;
}

mtgb::Timer::~Timer()
{
	for (auto& pTimer : Instance().pTimerQueue_)
	{
		delete pTimer;
	}
}

void mtgb::Timer::EnqueueTimer(QUEUE_ELEMENT* _pElement)
{
	float lefter{ _pElement->timeLeft };  // ���Z�p
	float righter{ 0 };     // ���Z�p
	// �K�؂ȑ}���|�C���g��������
	for (auto itr = Instance().pTimerQueue_.begin();
		itr != Instance().pTimerQueue_.end();
		itr++)
	{
		if (lefter <= righter + (*itr)->timeLeft)
		{
			_pElement->timeLeft = lefter - righter;
			itr = Instance().pTimerQueue_.insert(itr, _pElement);
			itr++;
			(*itr)->timeLeft -= _pElement->timeLeft;
			return;
		}
		righter += (*itr)->timeLeft;
	}
	// ������Ȃ������疖�[�ɒǉ�
	Instance().pTimerQueue_.push_back(_pElement);
}

mtgb::Timer* mtgb::Timer::pInstance_{ nullptr };
