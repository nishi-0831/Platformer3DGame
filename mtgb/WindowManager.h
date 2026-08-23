#pragma once
#include <vector>
#include <map>
#include <string>
#include "ISystem.h"
#include "WindowContext.h"
#include "Vector2Int.h"
typedef struct HWND__* HWND;
typedef struct tagMSG MSG;

namespace mtgb
{
	class WindowResource;
	struct WindowConfig
	{
		std::string title;
		std::string className;
		int width;
		int height;
		int x;
		int y;
	};

	class WindowManager : public ISystem
	{
	  public:
		WindowManager();
		~WindowManager();
		/// <summary>
		/// ウィンドウを作成
		/// </summary>
		/// <param name="_windowResource"></param>
		/// <returns></returns>
		HWND CreateWindowContext(WindowResource* _windowResource);
		/// <summary>
		/// ウィンドウサイズを返す
		/// </summary>
		/// <param name="_context">ウィンドウの識別子</param>
		/// <returns></returns>
		Vector2Int GetWindowSize(WindowContext _context);

		void Initialize() override;
		void Update() override;
		void Release() override;

		/// <summary>
		/// WindowContextに対応するWindowConfigをセット
		/// </summary>
		/// <param name="windowContext">ウィンドウの識別子</param>
		/// <param name="config">ウィンドウ設定</param>
		void SetWindowConfig(WindowContext _windowContext, const WindowConfig& _config);

		/// <summary>
		/// WindowContextからWindowConfigを取得
		/// </summary>
		/// <param name="windowContext">ウィンドウの識別子</param>
		/// <returns>ウィンドウ設定</returns>
		static WindowConfig GetWindowConfig(WindowContext _windowContext);

		/// <summary>
		/// <para> フルスクリーン状態を切り替える </para>
		/// <para> 指定したWindowContextに割り当てられているモニターのサイズに合わせる </para>
		/// </summary>
		/// <param name="_ctx">切り替えるウィンドウ</param>
		static void ChangeFullScreenState(WindowContext _ctx);

		/// <summary>
		/// ウィンドウが一番近い場所にあるモニターでフルスクリーン状態を切り替える
		/// </summary>
		static void ChangeFullScreenStateNearestMonitor(WindowContext _ctx);

		/// <summary>
		/// <para> フルスクリーン状態を切り替える </para>
		/// </summary>
		/// <param name="_ctx">切り替えるウィンドウ</param>
		/// <param name="_rect"></param>
		static void ChangeFullScreenState(WindowContext _ctx, const RECT& _rect);

		static void ResizeWindow(WindowContext _windowContext, UINT _width, UINT _height);

	  private:
		static MSG* pPeekedMessage_;
		static std::map<WindowContext, WindowConfig> windowConfigMap_;
	};
} // namespace mtgb