#pragma once
#include "Core/ISystem.h"
#include "cmtgb.h"
#include "IncludingInput.h"
#include <wrl/client.h>
#include <functional>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <guiddef.h>
#include <string>
#include "InputConfig.h"
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")

typedef struct HWND__* HWND;
using Microsoft::WRL::ComPtr;

namespace mtgb
{
	class InputData;

	/// <summary>
	/// コントローラーの種類
	/// </summary>
	enum class ControllerType
	{
		UNKNOWN,
		GAME_PAD,
		FLIGHT_STICK
	};

	/// <summary>
	/// ゲームパッドの種類
	/// </summary>
	enum class GamePadType
	{
		UNKNOWN,	// 不明
		DUAL_SHOCK, // デュアルショック
		XBOX		// Xboxコントローラー
	};

	/// <summary>
	/// 接続されたコントローラー
	/// </summary>
	struct ControllerContext
	{
		// 最後に取得を試みた際の結果
		HRESULT lastAcquireResult;
		// DirectInputのデバイス
		ComPtr<IDirectInputDevice8> device;
		// コントローラーの種類
		ControllerType controllerType;
		ControllerContext();
		~ControllerContext();
		ControllerContext(ComPtr<IDirectInputDevice8> _device);
	};

	/// <summary>
	/// コントローラーの割り当て予約
	/// </summary>
	struct ControllerReservation
	{
		// コントローラーが割り当てられるウィンドウのハンドル
		HWND hWnd;
		// コントローラーの入力設定
		InputConfig config;
		// 割り当てて欲しいコントローラーの種類
		ControllerType controllerType;
		// 割り当て時に呼ばれるコールバック
		std::function<void(ComPtr<IDirectInputDevice8>, GUID)> onAssign;
		ControllerReservation();
	};

	/// <summary>
	/// GUIDのハッシュ関数
	/// </summary>
	struct GuidHash
	{
		size_t operator()(const GUID& _guid) const
		{
			size_t hash = std::hash<unsigned long>()(_guid.Data1);
			hash ^= (std::hash<unsigned short>()(_guid.Data2) << 1);
			hash ^= (std::hash<unsigned short>()(_guid.Data3) << 2);

			// unsigned char Data4[8]
			for (int i = 0; i < 8; i++)
			{
				size_t seed = std::hash<unsigned char>()(_guid.Data4[i]);
				hash ^= seed << (3 + i);
			}
			return hash;
		}
	};

	class Input : public ISystem
	{
	  public:
		Input();
		~Input();

		void Initialize() override;
		void Update() override;
		/// <summary>
		/// キーボードの入力状態更新
		/// </summary>
		void UpdateKeyDevice();
		/// <summary>
		/// マウスの入力状態更新
		/// </summary>
		void UpdateMouseDevice();
		/// <summary>
		/// コントローラーの入力状態更新
		/// </summary>
		void UpdateControllerDevice();

		void Release() override;

		/// <summary>
		/// マウスの座標データを更新する
		/// </summary>
		/// <param name="_x">座標 x</param>
		/// <param name="_y">座標 y</param>
		void UpdateMousePositionData(int32_t _x, int32_t _y);

		/// <summary>
		/// 指定されたHWNDに対応するキーボードデバイス作成
		/// </summary>
		/// <param name="_hWnd"></param>
		/// <param name="_ppKeyDevice"></param>
		void CreateKeyDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppKeyDevice);
		/// <summary>
		/// 指定されたHWNDに対応するマウスデバイス作成
		/// </summary>
		/// <param name="_hWnd"></param>
		/// <param name="_ppMouseDevice"></param>
		void CreateMouseDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppMouseDevice);
		/// <summary>
		/// 入力状態を取得する対象のデバイスを切り替える
		/// </summary>
		/// <param name="_pJoystickDevice">切り替え対象のキーボードデバイス</param>
		void ChangeKeyDevice(ComPtr<IDirectInputDevice8> _pKeyDevice);
		/// <summary>
		/// 入力状態を取得するコントローラーのGUIDを設定する
		/// </summary>
		/// <param name="_guid"></param>
		void SetControllerGuid(GUID _guid);

		/// <summary>
		/// 入力状態を取得する対象のデバイスを切り替える
		/// </summary>
		/// <param name="_pJoystickDevice">切り替え対象のマウスデバイス</param>
		void ChangeMouseDevice(ComPtr<IDirectInputDevice8> _pMouseDevice);
		/// <summary>
		/// 入力状態を格納する対象を切り返る
		/// </summary>
		/// <param name="_pJoystickDevice">切り替え対象の入力状態を格納する物</param>
		void ChangeInputData(InputData* _pInputData);
		/// <summary>
		/// 入力状態を取得する対象のデバイスを切り替える
		/// </summary>
		/// <param name="_pJoystickDevice">切り替え対象のコントローラーデバイス</param>
		void ChangeControllerDevice(ComPtr<IDirectInputDevice8> _pJoystickDevice);

		/// <summary>
		/// 接続されているコントローラーの列挙、予約デバイスへの割り当てを行う
		/// </summary>
		void EnumController();

		/// <summary>
		/// コントローラーが接続された場合に割り当てられるよう予約する
		/// 先着順で割り当てられます
		/// </summary>
		/// <param name="_pJoystickDevice">割り当て希望のデバイス</param>
		void RequestControllerDevice(const ControllerReservation& _reservation);
		/// <summary>
		/// コントローラーが接続された場合に割り当てられるよう予約する
		/// 先着順で割り当てられます
		/// </summary>
		/// <param name="_pJoystickDevice">割り当て希望のデバイス</param>
		void RequestControllerDevice(ControllerReservation&& _reservation);

		/// <summary>
		/// 接続されているコントローラーを割り当て予約してるデバイスに割り当てる
		/// </summary>
		/// <param name="_pControllerDevice"></param>
		void AssignControllerToReservation(
			ComPtr<IDirectInputDevice8> _pControllerDevice,
			size_t _reservationIndex,
			GUID _guid
		);

		/// <summary>
		/// 登録されたコントローラーを解除する
		/// </summary>
		/// <param name="_guid">登録解除するGUID</param>
		/// <returns></returns>
		void UnregisterControllerGuid(GUID _guid);

		/// <summary>
		/// 割り当てられたコントローラーのGUIDを登録する
		/// </summary>
		/// <param name="_guid">登録するコントローラーのGUID</param>
		/// <returns>登録済みの場合はfalseを返す</returns>
		bool RegisterControllerGuid(GUID _guid);
		/// <summary>
		/// 割り当て予約がされていないか否か
		/// </summary>
		/// <returns>/returns>
		bool IsNotSubscribed();

		/// <summary>
		/// ベンダーIDからゲームパッドの種類を判別
		/// </summary>
		/// <param name="_pInputDevice"></param>
		/// <returns></returns>
		static GamePadType GetGamePadTypeByVendor(ComPtr<IDirectInputDevice8> _pInputDevice);
		/// <summary>
		/// デバイスの名前を取得
		/// </summary>
		/// <param name="_pInputDevice">デバイス</param>
		/// <returns>デバイス名</returns>
		std::string GetDeviceName(ComPtr<IDirectInputDevice8> _pInputDevice);
		/// <summary>
		/// デバイスの名前を取得
		/// </summary>
		/// <param name="_guid"></param>
		/// <returns></returns>
		std::string GetDeviceName(GUID _guid);

		/// <summary>
		/// デバイスの製品名を取得
		/// </summary>
		/// <param name="_pInputDevice">デバイス</param>
		/// <returns>デバイス名</returns>
		std::string GetDeviceProductName(ComPtr<IDirectInputDevice8> _pInputDevice);
		/// <summary>
		/// デバイスの製品名を取得
		/// </summary>
		/// <param name="_pInputDevice">デバイスのGUID</param>
		/// <returns>デバイス名</returns>
		std::string GetDeviceProductName(GUID _guid);
		/// <summary>
		/// HResultを文字列へ変換
		/// </summary>
		std::string_view ConvertHResultToMessage(HRESULT _hr) const;

		/// <summary>
		/// コントローラーの種類を判別
		/// </summary>
		/// <param name="_pInputDevice">コントローラーデバイス</param>
		/// <returns>コントローラーの種類</returns>
		static ControllerType GetControllerType(ComPtr<IDirectInputDevice8> _pControllerDevice);
		/// <summary>
		/// コントローラーの種類を判別
		/// </summary>
		/// <param name="_inst"></param>
		/// <returns>コントローラーの種類</returns>
		static ControllerType GetControllerType(const DIDEVICEINSTANCE& _inst);

		/// <summary>
		/// <para> 予約の中から指定された種類のデバイスを要求しているものを探し、先着順で割り当てを行う </para>
		/// <para> 見つからない場合はどの種類でも構わないという予約に割り当てる　</para>
		/// </summary>
		/// <param name="_devType">要求するデバイスの種類</param>
		/// <returns></returns>
		int FindReservationIndexForDevice(ControllerType _devType) const;
		std::string_view GetControllerStatusMessage(GUID _guid) const;

	  private:
		void AcquireController(ComPtr<IDirectInputDevice8> _pControllerDevice);
		GUID GetDeviceGuid(ComPtr<IDirectInputDevice8> _pInputDevice);
		void SetProperty(ComPtr<IDirectInputDevice8> _pJoystickDevice, InputConfig _inputConfig);
		InputData* pInputData_;							// 入力の状態
		ComPtr<IDirectInput8> pDirectInput_;			// Direct Input 本体
		ComPtr<IDirectInputDevice8> pKeyDevice_;		// キーボード
		ComPtr<IDirectInputDevice8> pMouseDevice_;		// マウス
		ComPtr<IDirectInputDevice8> pConrtollerDevice_; // コントローラー

		std::vector<ControllerReservation> requestedControllerDevices_; // 割り当て予約されたコントローラーデバイス
		std::unordered_set<GUID, GuidHash> assignedControllerGuids_;	// 既に割り当て済みのコントローラーのGUID
		// 接続されたコントローラー
		std::unordered_map<GUID, ControllerContext, GuidHash> controllerContext_;
		// 現在有効なコントローラーのGUID
		GUID currControllerGuid_;
	};
} // namespace mtgb
