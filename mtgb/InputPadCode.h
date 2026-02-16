#pragma once

#include "IncludingInput.h"

/// <summary>
/// ゲームパッドのボタンコード
/// </summary>
enum struct XInputPadCode : WORD
{
	UP		= XINPUT_GAMEPAD_DPAD_UP,
	DOWN	= XINPUT_GAMEPAD_DPAD_DOWN,
	LEFT	= XINPUT_GAMEPAD_DPAD_LEFT,
	RIGHT	= XINPUT_GAMEPAD_DPAD_RIGHT,
	START	= XINPUT_GAMEPAD_START,
	BACK	= XINPUT_GAMEPAD_BACK, // いわゆるselectボタンのこと。
	L_STICK	= XINPUT_GAMEPAD_LEFT_THUMB,
	R_STICK	= XINPUT_GAMEPAD_RIGHT_THUMB,
	LBUTTON = XINPUT_GAMEPAD_LEFT_SHOULDER,
	RBUTTON = XINPUT_GAMEPAD_RIGHT_SHOULDER,
	A		= XINPUT_GAMEPAD_A,
	B		= XINPUT_GAMEPAD_B,
	X		= XINPUT_GAMEPAD_X,
	Y		= XINPUT_GAMEPAD_Y,
};

/// <summary>
/// ゲームパッドのDirectInputでのボタンコード
/// </summary>
enum struct PadCode : uint8_t
{
	B	   = 0,
	A	   = 1,
	X	   = 2,
	Y	   = 3,
	LB	   = 4,
	RB	   = 5,
	BACK   = 6,
	L_STICK = 6,
	R_STICK = 7,
	START  = 9,

	CROSS	 = 1,
	CIRCLE	 = 2,
	TRIANGLE = 3,
	L1		 = 4,
	R1		 = 5,
	L2		 = 6,
	R2		 = 7
};

// ×ボタン、dual shock 1

enum struct StickType : uint8_t
{
	LEFT,
	RIGHT
};

/// <summary>
/// <para> フライトスティックのボタンコード </para>
/// <para> どのインデックスがどのボタンかデバイスによって異なる可能性があるので注意 </para>
/// </summary>
enum struct FlightStickCode : uint8_t
{
	THUMB	= 0, // 親指
	TRIGGER = 1,
	BUTTON3 = 2,
	BUTTON4 = 3,
};

enum struct PadIDState
{
	INVALID,	// 無効
	ASSIGNED,	// 割り当て済み
	UNASSIGNED, // 空き
};