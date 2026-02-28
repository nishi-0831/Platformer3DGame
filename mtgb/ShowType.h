#pragma once
/// <summary>
/// 表示するImGuiWindow
/// </summary>
enum class ShowType
{
	INSPECTOR,	// プロパティを表示
	SCENE_VIEW, // デバッグ用のゲーム画面
	SETTINGS,	// ゲームの設定をいじる画面
	EDITOR,
	NONE
};