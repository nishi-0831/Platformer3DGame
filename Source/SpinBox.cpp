#include "SpinBox.h"
#include <charconv>
mtgb::SpinBox::SpinBox(std::string_view _label, ValueType _valueType, bool _assignedValues)
	: min_ { 0 }
	, max_ { 0 }
	, incrementValue_ { 0 }
	, valueType_ { _valueType }
	, assignedValues_ { _assignedValues }
	, currValue_ {}
	, label_ { _label }
{
}

SpinBox mtgb::SpinBox::CreateNumberSpinBox(std::string_view _label, int _min, int _max, int _incrementValue)
{
	SpinBox spinBox(_label, ValueType::NUMBER, false);
	spinBox.min_			= _min;
	spinBox.max_			= _max;
	spinBox.incrementValue_ = _incrementValue;
	return spinBox;
}

SpinBox mtgb::SpinBox::CreateNumberSpinBox(std::string_view _label, const std::vector<int>& _values, int _defaultIndex)
{
	SpinBox spinBox(_label, ValueType::NUMBER, true);
	for (int value : _values)
	{
		spinBox.values_.push_back(std::to_string(value));
	}
	if (_defaultIndex >= 0 && _defaultIndex < spinBox.values_.size())
	{
		spinBox.currIdx_ = _defaultIndex;
	}
	else
	{
		massert(false && "_defaultIndex is invalid@SpinBox::CreateNumberSpinBox");
		spinBox.currIdx_ = 0;
	}
	spinBox.currValue_ = spinBox.values_[spinBox.currIdx_];
	return spinBox;
}

SpinBox mtgb::SpinBox::CreateStringSpinBox(
	std::string_view _label,
	const std::vector<std::string>& _strings,
	int _defaultIndex
)
{
	SpinBox spinBox(_label, ValueType::NUMBER, true);
	spinBox.values_ = _strings;
	if (_defaultIndex >= 0 && _defaultIndex < spinBox.values_.size())
	{
		spinBox.currIdx_ = _defaultIndex;
	}
	else
	{
		massert(false && "_defaultIndex is invalid@SpinBox::CreateStringSpinBox");
		spinBox.currIdx_ = 0;
	}
	spinBox.currValue_ = spinBox.values_[spinBox.currIdx_];
	return spinBox;
}

void mtgb::SpinBox::Increment()
{
	if (assignedValues_)
	{
		if (values_.empty())
		{
			massert(false && "values_ is empty @SpinBox::Increment");
			return;
		}
		currIdx_++;
		currIdx_   = std::clamp(currIdx_, 0, static_cast<int>(values_.size() - 1));
		currValue_ = values_[currIdx_];
	}
	else
	{
		int currNumber = GetNumber();
		int newNumber  = std::clamp(currNumber + incrementValue_, min_, max_);
		currValue_	   = std::to_string(newNumber);
	}
	callback_(*this);
}

void mtgb::SpinBox::Decrement()
{
	if (assignedValues_)
	{
		if (values_.empty())
		{
			massert(false && "values_ is empty @SpinBox::Decrement");
			return;
		}
		currIdx_--;
		currIdx_   = std::clamp(currIdx_, 0, static_cast<int>(values_.size() - 1));
		currValue_ = values_[currIdx_];
	}
	else
	{
		int currNumber = GetNumber();
		int newNumber  = std::clamp(currNumber - incrementValue_, min_, max_);
		currValue_	   = std::to_string(newNumber);
	}
	callback_(*this);
}

int mtgb::SpinBox::GetNumber()
{
	const std::string& str = currValue_;
	int value			   = 0;

	// 文字列を10進数整数値に変換
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);

	// パターンにマッチングする文字列が見つからなかった
	if (ec == std::errc::invalid_argument)
	{
		massert(false && "invalid_argument @SpinBox::GetNumber");
	}
	// 変換した結果の値が、from_charsに渡したvalueの型では表現できなかった場合
	if (ec == std::errc::result_out_of_range)
	{
		massert(false && "result_out_of_range @SpinBox::GetNumber");
	}

	return value;
}

std::string mtgb::SpinBox::GetString()
{
	return currValue_;
}

void mtgb::SpinBox::ShowImGui()
{
	///
	/// SpinBoxをImGuiで描画する
	///

	// ラベル描画
	ImGui::Text(label_.c_str());
	ImGui::SameLine();

	// 左ボタン描画
	ImGui::BeginDisabled(IsDisabledLeftButton());
	if (ImGui::ArrowButton("left", ImGuiDir_Left))
	{
		Decrement();
	}
	ImGui::EndDisabled();
	ImGui::SameLine();
	// 次の描画位置を記録
	float posStartX			= ImGui::GetCursorPosX() - ImGui::GetStyle().ItemSpacing.x;
	float posCenterX		= posStartX + WIDTH_LABEL / 2.0f;
	float centerAlignOffset = ImGui::CalcTextSize(currValue_.c_str()).x / 2.0f;
	// 現在の値を描画
	ImGui::SetCursorPosX(posCenterX - centerAlignOffset);
	ImGui::Text(currValue_.c_str());
	ImGui::SameLine();

	// 右ボタン描画
	ImGui::SetCursorPosX(posStartX + WIDTH_LABEL);
	ImGui::BeginDisabled(IsDisabledRightButton());
	if (ImGui::ArrowButton("right", ImGuiDir_Right))
	{
		Increment();
	}
	ImGui::EndDisabled();
}

void mtgb::SpinBox::SetNumber(int _number)
{
	std::string numberString = std::to_string(_number);

	auto itr = std::find(values_.begin(), values_.end(), numberString);
	if (itr != values_.end())
	{
		std::size_t index = std::distance(values_.begin(), itr);
		currIdx_		  = static_cast<int>(index);
		currValue_		  = values_[currIdx_];
		callback_(*this);
	}
}

void mtgb::SpinBox::SetString(std::string_view _string)
{
	auto itr = std::find(values_.begin(), values_.end(), _string);
	if (itr != values_.end())
	{
		std::size_t index = std::distance(values_.begin(), itr);
		currIdx_		  = static_cast<int>(index);
		currValue_		  = values_[currIdx_];
		callback_(*this);
	}
}

bool mtgb::SpinBox::IsDisabledLeftButton()
{
	if (assignedValues_)
	{
		if (currIdx_ == 0)
			return true;
		else
			return false;
	}
	else if (int value = GetNumber(); value <= min_)
		return true;

	return false;
}

bool mtgb::SpinBox::IsDisabledRightButton()
{
	if (assignedValues_)
	{
		if (currIdx_ == values_.size() - 1)
			return true;
		else
			return false;
	}
	else if (int value = GetNumber(); value >= max_)
		return true;

	return false;
}

mtgb::DictionarySpinBox::DictionarySpinBox(
	std::string_view _label,
	const std::vector<std::string>& _names,
	const std::vector<int>& _values,
	int _defaultIndex
)
	: spinBox_ { SpinBox::CreateStringSpinBox(_label, _names, _defaultIndex) }
{
	massert(_names.size() == _values.size());

	for (size_t i = 0; i < _names.size(); i++)
	{
		nameToValue_.insert(std::make_pair(_names[i], _values[i]));
	}
	spinBox_.SetOnValueChangedCallback(
		[this](SpinBox& _spinBox)
		{
			auto itr = nameToValue_.find(_spinBox.GetString());
			if (itr != nameToValue_.end())
			{
				currValue_ = itr->second;
			}
		}
	);
}

SpinBox& mtgb::DictionarySpinBox::GetSpinBox()
{
	return spinBox_;
}

int mtgb::DictionarySpinBox::GetCurrValue()
{
	return currValue_;
}
