#pragma once
#include <string>
#include <vector>
#include <concepts>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace mtgb
{
	class SpinBox
	{
	  public:
		static SpinBox CreateNumberSpinBox(
			std::string_view _label,
			int _min,
			int _max,
			int _incrementValue,
			int _defaultValue
		);
		static SpinBox CreateNumberSpinBox(std::string_view _label, const std::vector<int>& _values, int _defaultIndex);
		static SpinBox CreateStringSpinBox(
			std::string_view _label,
			const std::vector<std::string>& _strings,
			int _defaultIndex
		);
		void Increment();
		void Decrement();
		int GetNumber();
		std::string GetString();
		void ShowImGui();
		void SetNumber(int _number);
		void SetString(std::string_view _string);
		using OnChangedCallback = std::function<void(SpinBox&)>;

		template <typename Func>
			requires std::is_convertible_v<Func, OnChangedCallback>
		void SetOnValueChangedCallback(Func&& _func)
		{
			callback_ = std::forward<Func>(_func);
		}

		nlohmann::json Serialize() const;
		void Deserialize(const nlohmann::json& _json);

	  private:
		SpinBox(std::string_view _label, bool _assignedValues);
		bool IsDisabledLeftButton();
		bool IsDisabledRightButton();

		int min_;
		int max_;
		int incrementValue_;
		std::vector<std::string> values_;
		std::string currValue_;
		std::string label_;
		int currIdx_;
		bool assignedValues_;
		OnChangedCallback callback_;
		static constexpr float WIDTH_LABEL { 80.0f };
	};

	class DictionarySpinBox
	{
	  public:
		DictionarySpinBox(
			std::string_view _label,
			const std::vector<std::string>& _names,
			const std::vector<int>& _values,
			int _defaultIndex
		);
		SpinBox& GetSpinBox();
		int GetCurrValue() const;
		void Update();

		nlohmann::json SerializeCurrentSelection() const;
		void DeserializeCurrentSelection(const nlohmann::json& _json);

	  private:
		std::unordered_map<std::string, int> nameToValue_;
		SpinBox spinBox_;
		int currValue_;
		std::string currPresetName_;
	};
} // namespace mtgb
