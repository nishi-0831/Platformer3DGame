#pragma once
#include <nlohmann/json.hpp>
#include <unordered_map>
class JsonConverter
{
public:
	template<typename T>
	static nlohmann::json Serialize(T& _value);

	static void MergePatch(nlohmann::json& _target, nlohmann::json& _patch);

private:


};


