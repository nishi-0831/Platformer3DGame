#pragma once
#include <mtgb.h>
#include <functional>
#include <queue>

//�E�������Ԃ̕\��
//�E�������ԂɒB������m���}�m�F
//�E�I�[�o�[�V�[����(�N���Aor���s)
//�E�N���A�A�Q�[���I�[�o�[�̉��o(���̃e�L�X�g)

class TimeLimit : public mtgb::GameObject
{
public:
	TimeLimit();
	TimeLimit(float _time);
	~TimeLimit();

	void Update() override;
	void Draw() const override;
	/// <summary>
	/// �^�C�}�[�̃J�E���g�_�E�����n�߂�
	/// </summary>
	void StartTimer();

	/// <summary>
	/// �������ԂɒB�������ɌĂ΂��R�[���o�b�N���Z�b�g
	/// </summary>
	void RegisterOnEndTimerCallback(std::function<void()> _callback);

	/// <summary>
	/// �^�C�}�[�̃J�E���g�_�E�����ꎞ��~
	/// </summary>
	void PauseTimer();

	/// <summary>
	/// �^�C�}�[�̃J�E���g�_�E�����ĊJ
	/// </summary>
	void ResumeTimer();

	/// <summary>
	/// �c�莞�ԁA�t���O�A�R�[���o�b�N�����ׂă��Z�b�g
	/// </summary>
	void Reset();
	static constexpr float PLAY_SCENE_TIMER = 18.0f; // �v���C�V�[���̐�������(��)
private:
	std::queue<std::function<void()>> onEndTimerCallbacks_;
	float remainingTime_; // �c�莞��
	bool isStartTimer_; // �J�E���g�_�E�����n�܂�����
	bool isResumeTimer_; // �J�E���g�_�E�����ꎞ��~���Ă��邩
};