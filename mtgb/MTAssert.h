#pragma once
#include <assert.h>
#include <system_error>
#include "IncludingWindows.h"
#include <string>
#include "Game.h"
#include "MTStringUtility.h"
#if _DEBUG

#define massert(expression)                                                                                  \
	if (!(expression))                                                                                       \
	{                                                                                                        \
		DWORD errorCode { GetLastError() };                                                                  \
		std::string text { #expression };                                                                    \
		text += "\r\n\"";                                                                                    \
		text += __FILE__;                                                                                    \
		text += "\"より、";                                                                                  \
		text += std::to_string(__LINE__);                                                                    \
		text += "行目のmassertが起動しました。\r\n";                                                         \
		text += "Windowsの最終エラーを取得しますか？(はい/いいえ)";                                          \
		std::string title { mtgb::Game::Title() };                                                           \
		title += "のアサーションエラー";                                                                     \
		std::wstring wText	= mtgb::UTF8ToWide(text);                                                        \
		std::wstring wTitle = mtgb::UTF8ToWide(title);                                                       \
		int result {                                                                                         \
			MessageBoxW(NULL, wText.c_str(), wTitle.c_str(), MB_YESNOCANCEL | MB_ICONSTOP | MB_SYSTEMMODAL)  \
		};                                                                                                   \
		if (result == IDYES)                                                                                 \
		{                                                                                                    \
                                                                                                             \
			LPVOID lpMsgBuf;                                                                                 \
			FormatMessageW(                                                                                  \
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, \
				NULL,                                                                                        \
				errorCode,                                                                                   \
				MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),                                                \
				(LPWSTR) & lpMsgBuf,                                                                         \
				0,                                                                                           \
				NULL                                                                                         \
			);                                                                                               \
			MessageBoxW(NULL, (LPCWSTR(lpMsgBuf)), wTitle.c_str(), MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);    \
		}                                                                                                    \
		assert(false && "assertion error");                                                                  \
	}

#else

#define massert(expression) (expression)

#endif
