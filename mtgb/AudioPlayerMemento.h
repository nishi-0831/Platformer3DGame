#pragma once
#include "IComponentMemento.h"
#include "AudioPlayer.h"
#include "Handlers.h"

namespace mtgb
{
	class AudioPlayerMemento : public IComponentMemento
	{
		friend AudioPlayer;
	public:
		explicit AudioPlayerMemento(EntityId _entityId, AudioHandle _hAudio = INVALID_HANDLE)
			: entityId_{ _entityId }
			, hAudio_{ _hAudio }
		{
		}

		std::type_index GetComponentType() const override
		{
			return std::type_index(typeid(class AudioPlayer));
		}

		EntityId GetEntityId() const override { return entityId_; }

	private:
		EntityId entityId_;
		AudioHandle hAudio_;
	};
}
