#pragma once
#include <string>
#include <type_traits>
#include "Command.h"

namespace mtgb
{
	template<typename T>
	class ImGuiInputCommand : public Command
	{
	public:
		ImGuiInputCommand(
			T* _pTarget,
			const T& _oldValue,
			const T& _newValue,
			const std::string& _name)
			: pTarget_{ _pTarget }
			, oldValue_{ _oldValue }
			, newValue_{ _newValue }
			, name_{ _name }
		{
		}

		void Execute() override
		{
			*pTarget_ = newValue_;
		}

		void Undo() override
		{
			*pTarget_ = oldValue_;
		}

		std::string Name() const override
		{
			return "Input:" + name_;
		}
		EntityId GetCommandTargetEntityId() const override
		{
			if constexpr (std::is_base_of_v<Entity, T>)
			{
				return pTarget_->GetEntityId();
			}
			else
			{
				return INVALID_ENTITY;
			}
		}
	private:
		T* pTarget_;
		T oldValue_;
		T newValue_;
		std::string name_;
	};
}