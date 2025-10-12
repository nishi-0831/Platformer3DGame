#pragma once

/// <summary>
/// ���[�g�������o�[
/// </summary>
class Bar
{
public:
	Bar();
	~Bar();

	/// <summary>
	/// ���݂̃��[�g���擾
	/// </summary>
	/// <returns>0.0f ~ 0.1f</returns>
	const float GetRate() const;

	/// <summary>
	/// �ő�l���Z�b�g����
	/// </summary>
	/// <param name="_maxValue">�ő�l</param>
	inline void SetMax(const float _maxValue) { maxValue_ = _maxValue; CalculateSafeValue(); }
	/// <summary>
	/// �ŏ��l���Z�b�g����
	/// </summary>
	/// <param name="_minValue">�ŏ��l</param>
	inline void SetMin(const float _minValue) { minValue_ = _minValue; CalculateSafeValue(); }
	/// <summary>
	/// ���݂̒l���Z�b�g����
	/// </summary>
	/// <param name="_currentValue"></param>
	inline void SetCurrent(const float _currentValue) { currentValue_ = _currentValue; CalculateSafeValue(); }

private:
	/// <summary>
	/// ���݂̒l��͈͓��ɂ���悤�v�Z����
	/// </summary>
	void CalculateSafeValue();

private:
	float currentValue_;  // ���݂̒l
	float maxValue_;  // �ő�l
	float minValue_;  // �ŏ��l
};
