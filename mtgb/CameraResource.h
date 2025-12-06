#pragma once
#include "WindowContextResource.h"
#include "Handlers.h"
namespace mtgb
{
	/// <summary>
	/// ウィンドウごとに割り当てられたカメラ
	/// </summary>
	class CameraResource : public WindowContextResource
	{
	public:
		explicit CameraResource(WindowContext _windowContext);
		~CameraResource();
		
		void Release() override;
		/// <summary>
		/// カメラを切り替える
		/// </summary>
		void SetResource() override;

		/// <summary>
		/// カメラのハンドルをセットする
		/// </summary>
		/// <param name="_hCamera"></param>
		void SetHCamera(CameraHandleInScene _hCamera);
		/// <summary>
		///  割り当てられているカメラのハンドルを返す
		/// </summary>
		/// <returns></returns>
		CameraHandleInScene GetHCamera();
		/// <summary>
		/// <para> カメラのハンドルが未登録か否か </para>
		/// </summary>
		/// <returns>未登録の場合true、登録済みの場合false</returns>
		bool NotRegisterCamera();
	private:
		CameraHandleInScene hCamera_;

		// コピーコンストラクタとコピー代入演算子を削除
		CameraResource(const CameraResource&) = delete;
		CameraResource& operator=(const CameraResource&) = delete;

	};
}