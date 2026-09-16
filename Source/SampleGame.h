#pragma once
#include <mtgb.h>

class SampleGame : public mtgb::Game
{
  public:
	SampleGame();
	~SampleGame();

  private:
	void SetupSystems() override;

	inline std::string_view GetVersion() const override
	{
		return "0.0.1";
	};
	inline std::string_view GetTitle() const override
	{
		return "サンプルゲーム";
	};
};
