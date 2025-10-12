#pragma once
#include <assert.h>
#include "IncludingWindows.h"
#include <string>
#include "Game.h"

#if _DEBUG

#define massert(expression)                                                                                                                    \
	if (!(expression))                                                                                                                          \
	{                                                                                                                                          \
		std::string text{ #expression };                                                                                                       \
		text += "\r\n\"";                                                                                                                      \
		text += __FILE__;                                                                                                                      \
		text += "\"���A";                                                                                                                     \
		text += std::to_string(__LINE__);                                                                                                      \
		text += "�s�ڂ�massert���N�����܂����B\r\n";                                                                                                  \
		text += "�L�����Z�����Ȃ��ꍇ�A�X���[���܂��B\r\n";                                                                                                  \
		text += "Windows�̍ŏI�G���[���擾���܂����H(�͂�/������)";                                                                                                  \
		std::string title{ mtgb::Game::Title() };                                                                                              \
		title += "�̃A�T�[�V�����G���[";                                                                                                          \
		int result                                                                                                                             \
		{                                                                                                                                      \
			/*MessageBox(NULL, text.c_str(), title.c_str(), MB_RETRYCANCEL | MB_ICONSTOP | MB_SYSTEMMODAL)*/                                   \
			MessageBox(NULL, text.c_str(), title.c_str(), MB_YESNOCANCEL | MB_ICONSTOP | MB_SYSTEMMODAL)                                       \
		};                                                                                                                                     \
		if (result == IDYES)\
		{\
			DWORD errorCode{ GetLastError() };\
			MessageBox(NULL, (std::string{ "�ŏI�G���[�R�[�h:" } + std::to_string(errorCode)).c_str(), title.c_str(), MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);\
			throw "this massertion error";                                                                                                     \
		}\
		else if (result == IDNO)\
		{\
			throw "this massertion error";                                                                                                     \
		}\
		else\
		{\
			mtgb::Game::Exit();\
		}\
	}

#else

#define massert(expression) {}
	/*if (!!expression)       
	{                       
		mtgb::Game::Exit(); 
	}*/

#endif
