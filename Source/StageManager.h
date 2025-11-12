#pragma once
#include <mtgb.h>
#include <map>
#include <optional>
#include "StageID.h"
#include <nlohmann/json.hpp>
class StageManger : public ISystem
{
public:
	void Initialize() override;
	void Update() override;
	std::optional<nlohmann::json> GetStageJson(StageID _stageID);
private:
	std::map<StageID, nlohmann::json> stageJsons_;
};
