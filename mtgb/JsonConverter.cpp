#include "JsonConverter.h"
#include "ReflectionInfo.h"
#include "JsonConverterImpl.h"

void JsonConverter::MergePatch(nlohmann::json& _target, nlohmann::json& _patch)
{
	if (_patch.is_object() == false)
	{
		// patch���I�u�W�F�N�g�łȂ��Ȃ��̃I�u�W�F�N�g�ɂ���
		_target = _patch;
		//_target.merge_patch
	}
}
