#pragma once
#include <mtgb.h>

class ScoreViewer : public mtgb::GameObject
{
public:
	ScoreViewer();
	ScoreViewer(RectF _rect, int _fontSize,TextAlignment _textAlignment);
	~ScoreViewer();

	void Update() override;
	void Draw() const override;

private:
	Transform* pTransform_;
	//TextRenderer* pTextRenderer_;
};
