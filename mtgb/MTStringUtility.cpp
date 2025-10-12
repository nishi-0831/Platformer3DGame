#include "MTStringUtility.h"
#include "IncludingWindows.h"
#include "MTAssert.h"

std::wstring mtgb::MultiToWide(const std::string& _string)
{
	return MultiToWide(_string.c_str());
}

std::wstring mtgb::MultiToWide(const char* _string)
{
	int wlen = MultiByteToWideChar(
		CP_ACP,//�ϊ��Ɏg�p����R�[�h�y�[�W���w��(�V�X�e���̊���R�[�h�y�[�W���g�p)
		0,//�ϊ��̎��
		_string,//�ϊ����镶����
		-1,//������̃T�C�Y�B-1�̏ꍇ�I�[��null�������܂ޕ�����S��
		NULL, //�ϊ����ꂽ��������󂯎��
		0);//��������󂯎��o�b�t�@�̃T�C�Y,-1�̏ꍇ������S��

	std::wstring wstr(wlen, L'\0');
	MultiByteToWideChar(
		CP_ACP,
		0,
		_string,
		-1,
		&wstr[0],
		wlen);

	return wstr;
}

std::string mtgb::WideToMulti(const std::wstring& _wstring)
{
	return WideToMulti(_wstring.c_str());
}

std::string mtgb::WideToMulti(const wchar_t* _wstring)
{
	int len = WideCharToMultiByte(
		CP_ACP,
		0,
		_wstring,
		-1,
		NULL,
		0,
		NULL,
		NULL);

	std::string str(len, '\0');

	WideCharToMultiByte(
		CP_ACP,
		0,
		_wstring,
		-1,
		&str[0],
		len,
		NULL,
		NULL);

	if (!str.empty() && str.back() == '\0')
	{
		str.pop_back();
	}

	return str;
}

std::wstring mtgb::UTF8ToWide(const std::string& _string)
{
	return UTF8ToWide(_string.c_str());
}

std::wstring mtgb::UTF8ToWide(const std::u8string& _string)
{
	if (_string.empty()) return std::wstring();

	// �K�v�ȃo�b�t�@�T�C�Y���擾
	int size = MultiByteToWideChar(
		CP_UTF8,  // �\�[�X�̃R�[�h�y�[�W (UTF-8)
		MB_ERR_INVALID_CHARS,
		reinterpret_cast<const char*>(_string.c_str()),
		static_cast<int>(_string.size()),
		nullptr,  // �o�͐�o�b�t�@�Ȃ�
		0         // �o�b�t�@�T�C�Y�擾
	);

	massert(size != 0 && "Invalid UTF-8 sequence");

	// �ϊ��p�o�b�t�@�m��
	std::wstring wstr(size, L'\0');
	int ret = MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		reinterpret_cast<const char*>(_string.c_str()),
		static_cast<int>(_string.size()),
		wstr.data(),
		size
	);

	massert(ret != 0 && "Invalid UTF-8 sequence");

	return wstr;
}

std::wstring mtgb::UTF8ToWide(const char* _string)
{
	int wlen = MultiByteToWideChar(
		CP_UTF8,
		0,
		_string,
		-1,
		NULL,
		0);

	std::wstring wstr(wlen, '\0');
	MultiByteToWideChar(
		CP_UTF8,
		0,
		_string,
		-1,
		&wstr[0],
		wlen);

	return wstr;
}

std::string mtgb::WideToUTF8(const std::wstring& _wstring)
{
	return WideToUTF8(_wstring.c_str());
}

std::string mtgb::WideToUTF8(const wchar_t* _wstring)
{
	//utf16��utf8
	int u8len = WideCharToMultiByte(
		CP_UTF8,
		0,
		_wstring,
		-1,
		NULL,
		0,
		NULL,
		NULL);

	std::string utf8str(u8len, '\0');

	WideCharToMultiByte(
		CP_UTF8,
		0,
		_wstring,//�ϊ����镶����
		-1,//������̃T�C�Y�B�I�[��null�����̏ꍇ-1
		&utf8str[0],//�ϊ����ꂽ��������󂯎��
		u8len, //��������󂯎��o�b�t�@�̃T�C�Y
		NULL, //�w�肵���R�[�h�y�[�W�ŕ\�����Ƃ��ł��Ȃ��ꍇ�Ɏg�p���镶���ւ̃|�C���^
		NULL);//�\�����Ƃ��ł����Ɋ���̕���(?)���g�p�������ǂ����������t���O�ւ̃|�C���^

	//������'\0'������
	if (!utf8str.empty() && utf8str.back() == '\0')
	{
		utf8str.pop_back();
	}

	return utf8str;
}

std::string mtgb::MultiToUTF8(const std::string& _string)
{
	//Shitf-JIS �� UTF-16 �� UTF-8
	return WideToUTF8(MultiToWide(_string));
}

std::string mtgb::MultiToUTF8(const char* _string)
{
	return WideToUTF8(MultiToWide(_string));
}

std::string mtgb::UTF8ToMulti(const std::string& _string)
{
	return WideToMulti(UTF8ToWide(_string));
}

std::string mtgb::UTF8ToMulti(const char* _string)
{
	return WideToMulti(UTF8ToWide(_string));
}
