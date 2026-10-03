#pragma once
/// <summary>
/// ゲームで使用するコントローラーの共通ボタン
/// </summary>
enum struct PadButton : uint8_t
{
	EAST,
	WEST,
	NORTH,
	SOUTH,
	L_STICK,
	R_STICK,
	START
};

/// <summary>
/// DualShockのDirectInputボタン配列のインデックス
/// </summary>
enum struct DualShockButtonIndex : uint8_t
{
	SQUARE		   = 0,
	CROSS		   = 1,
	CIRCLE		   = 2,
	TRIANGLE	   = 3,
	L1			   = 4,
	R1			   = 5,
	L2			   = 6,
	R2			   = 7,
	L_STICK_BUTTON = 10
};

/// <summary>
/// XboxコントローラーのDirectInputボタン配列のインデックス
/// </summary>
enum struct XboxButtonIndex : uint8_t
{
	A			   = 0,
	B			   = 1,
	X			   = 2,
	Y			   = 3,
	LB			   = 4,
	RB			   = 5,
	L_STICK_BUTTON = 8
};

enum struct StickType : uint8_t
{
	LEFT,
	RIGHT
};

/// <summary>
/// <para> フライトスティックのボタン </para>
/// <para> どのインデックスがどのボタンかデバイスによって異なる可能性があるので注意 </para>
/// </summary>
enum struct FlightStickButton : uint8_t
{
	THUMB	= 0, // 親指
	TRIGGER = 1,
	BUTTON3 = 2,
	BUTTON4 = 3,
};