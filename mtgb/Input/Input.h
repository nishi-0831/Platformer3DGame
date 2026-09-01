#pragma once
#include "Core/ISystem.h"
#include "cmtgb.h"
#include "IncludingInput.h"
#include <wrl/client.h> // 追加
#include <functional>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <guiddef.h>
#include <string>
#include "Core/Time/Timer.h"
#include "InputConfig.h"
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")

typedef struct HWND__* HWND;
using Microsoft::WRL::ComPtr; // 追加

namespace mtgb
{
	class InputResource;
	class InputData;

	enum class DeviceType
	{
		UNKNOWN,
		GAME_PAD,
		FLIGHT_STICK
	};

	enum class ControllerType
	{
		UNKNOWN,
		DUAL_SHOCK,
		XBOX
	};

	struct JoystickContext
	{
		TimerHandle timerHandle;
		HRESULT lastResult;
		ComPtr<IDirectInputDevice8> device;
		DeviceType deviceType;
		JoystickContext();
		~JoystickContext();
		JoystickContext(ComPtr<IDirectInputDevice8> _device);
	};

	/// <summary>
	/// ジョイスティックの割り当て予約
	/// </summary>
	struct JoystickReservation
	{
		HWND hWnd;
		InputConfig config;
		DeviceType deviceType; // 割り当てて欲しいデバイスの種類
		std::function<void(ComPtr<IDirectInputDevice8>, GUID)> onAssign;
		~JoystickReservation();
	};

	struct GuidHash
	{
		size_t operator()(const GUID& _guid) const
		{
			size_t hash = std::hash<unsigned long>()(_guid.Data1);
			hash ^= (std::hash<unsigned short>()(_guid.Data2) << 1);
			hash ^= (std::hash<unsigned short>()(_guid.Data3) << 2);

			for (int i = 0; i < 8; i++)
			{
				hash ^= (std::hash<unsigned char>()(_guid.Data4[i]) << (3 + i));
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
		void UpdateKeyDevice();
		void UpdateMouseDevice();
		void UpdateJoystickDevice();

		void Release() override;

		/// <summary>
		/// マウスの座標データを更新する
		/// </summary>
		/// <param name="_x">座標 x</param>
		/// <param name="_y">座標 y</param>
		void UpdateMousePositionData(int32_t _x, int32_t _y);

		void CreateKeyDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppKeyDevice);
		void CreateMouseDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppMouseDevice);
		/// <summary>
		/// 入力状態を取得する対象のデバイスを切り替える
		/// </summary>
		/// <param name="_pJoystickDevice">切り替え対象のキーボードデバイス</param>
		void ChangeKeyDevice(ComPtr<IDirectInputDevice8> _pKeyDevice);
		/// <summary>
		/// 入力状態を取得するジョイスティックのGUIDを設定する
		/// </summary>
		/// <param name="_guid"></param>
		void SetJoystickGuid(GUID _guid);

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
		/// <param name="_pJoystickDevice">切り替え対象のジョイスティックデバイス</param>
		void ChangeJoystickDevice(ComPtr<IDirectInputDevice8> _pJoystickDevice);

		/// <summary>
		/// 接続されているジョイスティックの列挙、予約デバイスへの割り当てを行う
		/// </summary>
		void EnumJoystick();

		/// <summary>
		/// ジョイスティックが接続された場合に割り当てられるよう予約する
		/// 先着順で割り当てられます
		/// </summary>
		/// <param name="_pJoystickDevice">割り当て希望のデバイス</param>
		void RequestJoystickDevice(const JoystickReservation& _reservation);
		/// <summary>
		/// ジョイスティックが接続された場合に割り当てられるよう予約する
		/// 先着順で割り当てられます
		/// </summary>
		/// <param name="_pJoystickDevice">割り当て希望のデバイス</param>
		void RequestJoystickDevice(JoystickReservation&& _reservation);

		/// <summary>
		/// 接続されているジョイスティックを割り当て予約してるデバイスに割り当てる
		/// </summary>
		/// <param name="_pJoystickDevice"></param>
		void AssignJoystickToReservation(
			ComPtr<IDirectInputDevice8> _pJoystickDevice,
			size_t _reservationIndex,
			GUID _guid
		);

		/// <summary>
		/// 登録されたジョイスティックを解除する
		/// </summary>
		/// <param name="_guid">登録解除するGUID</param>
		/// <returns></returns>
		void UnregisterJoystickGuid(GUID _guid);

		/// <summary>
		/// 割り当てられたジョイスティックのGUIDを登録する
		/// </summary>
		/// <param name="_guid">登録するジョイスティックのGUID</param>
		/// <returns>登録済みの場合はfalseを返す</returns>
		bool RegisterJoystickGuid(GUID _guid);
		/// <summary>
		/// 割り当て予約がされていないか否か
		/// </summary>
		/// <returns>/returns>
		bool IsNotSubscribed();

		/// <summary>
		/// ベンダーIDからコントローラーの種類を判別
		/// </summary>
		/// <param name="_pInputDevice"></param>
		/// <returns></returns>
		static ControllerType GetControllerTypeByVendor(ComPtr<IDirectInputDevice8> _pInputDevice);
		/// <summary>
		/// デバイスの名前を取得
		/// </summary>
		/// <param name="_pInputDevice">デバイス</param>
		/// <returns>デバイス名</returns>
		std::string GetDeviceName(ComPtr<IDirectInputDevice8> _pInputDevice);
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
		/// デバイスのタイプを判別
		/// </summary>
		/// <param name="_pInputDevice">デバイス</param>
		/// <returns>デバイスタイプ</returns>
		static DeviceType GetDeviceType(ComPtr<IDirectInputDevice8> _pInputDevice);
		/// <summary>
		/// デバイスのタイプを判別
		/// </summary>
		/// <param name="_inst">デバイス</param>
		/// <returns>デバイスタイプ</returns>
		static DeviceType GetDeviceType(const DIDEVICEINSTANCE& _inst);

		/// <summary>
		/// <para> 予約の中から指定された種類のデバイスを要求しているものを探し、先着順で割り当てを行う </para>
		/// <para> 見つからない場合はどの種類でも構わないという予約に割り当てる　</para>
		/// </summary>
		/// <param name="_devType">要求するデバイスの種類</param>
		/// <returns></returns>
		int FindReservationIndexForDevice(DeviceType _devType) const;
		std::string_view GetJoystickStatusMessage(GUID _guid) const;

		bool IsJoystickConnected(GUID _guid) const;
		bool IsJoystickAssigned(GUID _guid) const;

	  private:
		TimerHandle enumTimerHandle_ { nullptr };

		void AcquireJoystick(ComPtr<IDirectInputDevice8> _pJoystickDevice);
		GUID GetDeviceGuid(ComPtr<IDirectInputDevice8> _pInputDevice);
		void SetProperty(ComPtr<IDirectInputDevice8> _pJoystickDevice, InputConfig _inputConfig);
		InputData* pInputData_;						  // 入力の状態
		ComPtr<IDirectInput8> pDirectInput_;		  // Direct Input 本体k
		ComPtr<IDirectInputDevice8> pKeyDevice_;	  // キーデバイス
		ComPtr<IDirectInputDevice8> pMouseDevice_;	  // マウスデバイス
		ComPtr<IDirectInputDevice8> pJoystickDevice_; // ジョイスティックデバイス

		std::vector<JoystickReservation> requestedJoystickDevices_; // 割り当て予約されたジョイスティックデバイス
		std::unordered_set<GUID, GuidHash> assignedJoystickGuids_;	// 既に割り当て済みのジョイスティック

		std::unordered_map<GUID, JoystickContext, GuidHash> joystickContext_;
		GUID currJoystickGuid_;
	};
} // namespace mtgb
