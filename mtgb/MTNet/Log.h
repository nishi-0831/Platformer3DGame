#pragma once
#include <string>
#include <list>
#include <functional>
#include <mutex>

namespace mtnet
{
	constexpr size_t LOGS_COUNT_MAX{ 100 };  // �~�ς��郍�O�̍ő吔
	using OnLogFunc = std::function<void(const std::string&)>;

	/// <summary>
	/// �f�[�^�L�����A�N���X
	/// </summary>
	class Log
	{
	public:
		/// <summary>
		/// ���O����������
		/// </summary>
		/// <param name="_content">���e</param>
		static void Write(const std::string& _content);

		/// <summary>
		/// �o�C�i���z������O�ɏ�������
		/// </summary>
		/// <param name="_binArray">�o�C�i���z��̃|�C���^</param>
		/// <param name="_binArraySize">�o�C�i���z��̃T�C�Y</param>
		static void Write(const char* _binArray, const size_t& _binArraySize);

		/// <summary>
		/// ���O�ꗗ�𐶐�����
		/// </summary>
		/// <returns></returns>
		static std::string GenerateLogs();
		/// <summary>
		/// ���O�����s���ꂽ�Ƃ��̏���
		/// </summary>
		/// <param name="_callback">�R�[���o�b�N�֐�(������)</param>
		static void OnLog(const OnLogFunc& _callback);

	private:
		Log() = delete;
		~Log() = delete;

	private:
		static std::list<std::string> logs_;  // ���O���߂Ă���
		static OnLogFunc onLogFunc_;  // ���O�����s���ꂽ�Ƃ��̃R�[���o�b�N�֐�
		static std::mutex logWriteMutex_;  // ���O�������o���r������p
	};
}
