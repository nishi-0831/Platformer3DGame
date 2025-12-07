// AudioPlayer.generated.cpp
# include "AudioPlayer.generated.h"
# include "AudioPlayer.h"
# include "TypeRegistry.h"

namespace mtgb
{
	// ImGui�\��������o�^
	struct AudioPlayer_Register
	{
		AudioPlayer_Register()
		{
			RegisterShowFuncHolder::Set<AudioPlayer>([]( AudioPlayer* _target, const char* _name)
				{
								TypeRegistry::Instance().CallFunc(&_target->hAudio, "hAudio");
				});
		}
	};

	static AudioPlayer_Register audioplayer_register;
}
