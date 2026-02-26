#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "Handlers.h"
struct AnimationEvent
{
	std::string eventName;
	int startFrame;
	int endFrame;
	std::string boneName;
};

static void to_json(nlohmann::json& _json, const AnimationEvent& _evt)
{
	_json["name"] = _evt.eventName;
	_json["start"] = _evt.startFrame;
	_json["end"] = _evt.endFrame;
	_json["bone"] = _evt.boneName;
}

static void from_json(const nlohmann::json& _json, AnimationEvent& _evt)
{
	_evt.eventName = _json.at("name").get<std::string>();
	_evt.startFrame = _json.at("start").get<int>();
	_evt.endFrame = _json.at("end").get<int>();
	_evt.boneName	= _json.at("bone").get<std::string>();
}