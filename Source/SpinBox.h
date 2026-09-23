#include <string>
#include <vector>
#include <concepts>
#include <unordered_map>
namespace mtgb
{
	class SpinBox
	{
	  public:
		static SpinBox CreateNumberSpinBox(std::string_view _label, int _min, int _max, int _incrementValue);
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

	  private:
		enum class ValueType
		{
			NUMBER,
			STRING
		};
		SpinBox(std::string_view _label, ValueType _valueType, bool _assignedValues);
		bool IsDisabledLeftButton();
		bool IsDisabledRightButton();

		int min_;
		int max_;
		int incrementValue_;
		ValueType valueType_;
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
		int GetCurrValue();

	  private:
		std::unordered_map<std::string, int> nameToValue_;
		SpinBox spinBox_;
		int currValue_;
	};
} // namespace mtgb
