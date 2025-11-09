#pragma once
#include <mtgb.h>

class ScoreViewer : public mtgb::GameObject
{
public:
	ScoreViewer();
	~ScoreViewer();

	void Update() override;
	void Draw() const override;

private:
	Transform* pTransform_;
	
};
