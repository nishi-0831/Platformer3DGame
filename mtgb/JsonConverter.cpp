#include "JsonConverter.h"
#include "ReflectionInfo.h"
#include "JsonConverterImpl.h"

void JsonConverter::MergePatch(nlohmann::json& _target, nlohmann::json& _patch)
{
	if (_patch.is_object() == false)
	{
		// patchがオブジェクトでないなら空のオブジェクトにする
		_target = _patch;
		//_target.merge_patch
	}
}
