#include "Bar.h"
#include "MTAssert.h"


Bar::Bar() :
	currentValue_{ 0.0f },
	maxValue_{ 0.0f },
	minValue_{ 0.0f }
{
}

Bar::~Bar()
{
}

const float Bar::GetRate() const
{
	// �͈͓��ł̑��ΐ� / �͈�
	return (currentValue_ - minValue_) / (maxValue_ - minValue_);
}

void Bar::CalculateSafeValue()
{
	// �����ŏ��l�ƍő�l���t�]���Ă���Ȃ甽�]����
	if (maxValue_ < minValue_)
	{
		float tmpMaxValue{ maxValue_ };
		maxValue_ = minValue_;
		minValue_ = tmpMaxValue;
	}

	// �������݂̒l���͈͊O�Ȃ�͈͓��ɔ[�߂�
	if (currentValue_ > maxValue_)
	{
		currentValue_ = maxValue_;
	}
	else if (currentValue_ < minValue_)
	{
		currentValue_ = minValue_;
	}
}
