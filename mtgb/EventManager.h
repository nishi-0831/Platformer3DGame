#pragma once

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <queue>
#include "ISystem.h"
namespace mtgb
{
	class EventBase
	{
	public:
		virtual ~EventBase() = default;
		virtual void UnsubscribeAll() = 0;
	};

	using EventHandlerId = std::size_t;
	/// <summary>
	/// <para>�C�x���g�p�̃e���v���[�g�N���X�B</para>
	/// <para>�C�ӂ̃C�x���g�f�[�^�^�ɑ΂��ăn���h���̓o�^�A�����A�ꊇ�����A�Ăяo�����s���B</para>
	/// <para>�n���h���� ID �ŊǗ����AInvoke ���s���ɕۗ����̉����v������������B</para>
	/// </summary>
	template<typename EventDataType>
	class Event : public EventBase
	{
	public:
		using EventHandler = std::function<void(const EventDataType&)>;

		/// <summary>
		/// �n���h����o�^���A�o�^�Ɋ��蓖�Ă�ꂽ ID ��Ԃ�
		/// </summary>
		EventHandlerId Subscribe(EventHandler _handler)
		{
			const EventHandlerId id = nextId++;
			handlers_.emplace(id, std::move(_handler));
			return id;
		}
		
		/// <summary>
		/// �w�肵���n���h�� ID ��ۗ������L���[�ɒǉ�����
		/// <para>���ۂ̉����͎���� Invoke �Ăяo�����ɍs��</para>
		/// </summary>
		void Unsubscribe(EventHandlerId _id)
		{
			pendingUnsubscribeIds.push(_id);
		}

		/// <summary>
		/// �o�^����Ă��邷�ׂẴn���h���𑦍��ɉ�������
		/// </summary>
		void UnsubscribeAll() override
		{
			handlers_.clear();
		}

		/// <summary>
		/// �ۗ����̉����v�����������A���̌シ�ׂẴn���h���ɑ΂��ăC�x���g�f�[�^��n���ČĂяo��
		/// </summary>
		void Invoke(const EventDataType& _data)
		{
			while (!pendingUnsubscribeIds.empty())
			{
				handlers_.erase(pendingUnsubscribeIds.front());
				pendingUnsubscribeIds.pop();
			}
			for (auto& handler : handlers_)
			{
				if (handler.second)
				{
					handler.second(_data);
				}
			}
		}
	private:
		std::unordered_map<EventHandlerId,EventHandler> handlers_;
		std::queue<EventHandlerId> pendingUnsubscribeIds;
		EventHandlerId nextId = 1;
	};

	class EventManager : public ISystem
	{
	public:
		~EventManager();
		void Initialize() override;
		void Update() override {}

		template<typename EventDataType>
		/// <summary>
		/// �w�肳�ꂽ�C�x���g�f�[�^�^�ɑΉ�����Event �I�u�W�F�N�g��Ԃ�
		/// </summary>
		Event<EventDataType>& GetEvent()
		{
			// �e���v���[�g�^ `EventDataType` �����ʂ��邽�߂� type_index ���쐬
			std::type_index typeIdx = std::type_index(typeid(EventDataType));

			auto it = events_.find(typeIdx);
			if (it == events_.end())
			{
				// ���݂��Ȃ��ꍇ�͐V�K�쐬���ă}�b�v�ɓo�^
				Event<EventDataType>* newEvent = new Event<EventDataType>();
				events_[typeIdx] = newEvent;
				return  static_cast<Event<EventDataType>&>(*events_[typeIdx]);
			}

			// �����̃C�x���g��Ԃ�
			return static_cast<Event<EventDataType>&>(*it->second);
		}

	private:
		std::unordered_map<std::type_index, EventBase*> events_;
	};
}