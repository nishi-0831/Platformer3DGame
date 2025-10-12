#pragma once
#include "ISystem.h"
#include "cmtgb.h"
#include "IncludingInput.h"
#include <wrl/client.h> // �ǉ�
#include <functional>
#include <vector>
#include <tuple>
#include <set>
#include <guiddef.h>
#include <map>
#include "Timer.h"
#include "InputConfig.h"
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")

typedef struct HWND__* HWND;
using Microsoft::WRL::ComPtr; // �ǉ�



namespace mtgb
{
	class InputResource;
	class InputData;

	enum class DeviceType
	{
		Unknown,
		GamePad,
		FlightStick
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
	/// �W���C�X�e�B�b�N�̊��蓖�ė\��
	/// </summary>
	struct JoystickReservation
	{
		HWND hWnd;
		InputConfig config;
		DeviceType deviceType; // ���蓖�Ăė~�����f�o�C�X�̎��
		std::function<void(ComPtr<IDirectInputDevice8>, GUID)> onAssign;
		~JoystickReservation();
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
		void UpdateGamePadDevice();

		void Release();

		/// <summary>
		/// �}�E�X�̍��W�f�[�^���X�V����
		/// </summary>
		/// <param name="_x">���W x</param>
		/// <param name="_y">���W y</param>
		void UpdateMousePositionData(const int32_t _x, const int32_t _y);

		void CreateKeyDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppKeyDevice);
		void CreateMouseDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppMouseDevice);
		/// <summary>
		/// ���͏�Ԃ��擾����Ώۂ̃f�o�C�X��؂�ւ���
		/// </summary>
		/// <param name="_pJoystickDevice">�؂�ւ��Ώۂ̃L�[�{�[�h�f�o�C�X</param>
		void ChangeKeyDevice(ComPtr<IDirectInputDevice8> _pKeyDevice);

		void SetJoystickGuid(GUID _guid);

		/// <summary>
		/// ���͏�Ԃ��擾����Ώۂ̃f�o�C�X��؂�ւ���
		/// </summary>
		/// <param name="_pJoystickDevice">�؂�ւ��Ώۂ̃}�E�X�f�o�C�X</param>
		void ChangeMouseDevice(ComPtr<IDirectInputDevice8> _pMouseDevice);
		/// <summary>
		/// ���͏�Ԃ��i�[����Ώۂ�؂�Ԃ�
		/// </summary>
		/// <param name="_pJoystickDevice">�؂�ւ��Ώۂ̓��͏�Ԃ��i�[���镨</param>
		void ChangeInputData(InputData* _pInputData);
		/// <summary>
		/// ���͏�Ԃ��擾����Ώۂ̃f�o�C�X��؂�ւ���
		/// </summary>
		/// <param name="_pJoystickDevice">�؂�ւ��Ώۂ̃W���C�X�e�B�b�N�f�o�C�X</param>
		void ChangeJoystickDevice(ComPtr<IDirectInputDevice8> _pJoystickDevice);

		/// <summary>
		/// �ڑ�����Ă���W���C�X�e�B�b�N�̗񋓁A�\��f�o�C�X�ւ̊��蓖�Ă��s��
		/// </summary>
		void EnumJoystick();

		/// <summary>
		/// �W���C�X�e�B�b�N���ڑ����ꂽ�ꍇ�Ɋ��蓖�Ă���悤�\�񂷂�
		/// �撅���Ŋ��蓖�Ă��܂�
		/// </summary>
		/// <param name="_pJoystickDevice">���蓖�Ċ�]�̃f�o�C�X</param>
		void RequestJoystickDevice(const JoystickReservation& _reservation);

		void RequestJoystickDevice(JoystickReservation&& _reservation);

		/// <summary>
		/// �ڑ�����Ă���W���C�X�e�B�b�N�����蓖�ė\�񂵂Ă�f�o�C�X�Ɋ��蓖�Ă�
		/// </summary>
		/// <param name="_pJoystickDevice"></param>
		void AssignJoystickToReservation(ComPtr<IDirectInputDevice8> _pJoystickDevice, size_t _reservationIndex, GUID _guid);

		/// <summary>
		/// �o�^���ꂽ�W���C�X�e�B�b�N����������
		/// </summary>
		/// <param name="_guid">�o�^��������GUID</param>
		/// <returns></returns>
		void UnregisterJoystickGuid(GUID _guid);

		/// <summary>
		/// ���蓖�Ă�ꂽ�W���C�X�e�B�b�N��GUID��o�^����
		/// </summary>
		/// <param name="_guid">�o�^����W���C�X�e�B�b�N��GUID</param>
		/// <returns>�o�^�ς݂̏ꍇ��false��Ԃ�</returns>
		bool RegisterJoystickGuid(GUID _guid);

		/// <summary>
		/// �擾���o��ݒ肷��
		/// </summary>
		void SetAcquireInterval(GUID _guid, ComPtr<IDirectInputDevice8> _device);
		/// <summary>
		/// ���蓖�ė\�񂪂���Ă��Ȃ����ۂ�
		/// </summary>
		/// <returns>/returns>
		bool IsNotSubscribed();

		/// <summary>
		/// �f�o�C�X�̖��O���擾
		/// </summary>
		/// <param name="_pInputDevice">�f�o�C�X</param>
		/// <returns>�f�o�C�X��</returns>
		std::string GetDeviceName(ComPtr<IDirectInputDevice8> _pInputDevice);
		std::string GetDeviceName(GUID _guid);

		/// <summary>
		/// �f�o�C�X�̐��i�����擾
		/// </summary>
		/// <param name="_pInputDevice">�f�o�C�X</param>
		/// <returns>�f�o�C�X��</returns>
		std::string GetDeviceProductName(ComPtr<IDirectInputDevice8> _pInputDevice);
		std::string GetDeviceProductName(GUID _guid);
		std::string ConvertHResultToMessage(HRESULT hr) const;

		/// <summary>
		/// �f�o�C�X�̃^�C�v�𔻕�
		/// </summary>
		/// <param name="_pInputDevice">�f�o�C�X</param>
		/// <returns>�f�o�C�X�^�C�v</returns>
		/// 
		static DeviceType GetDeviceType(ComPtr<IDirectInputDevice8> _pInputDevice);
		static DeviceType GetDeviceType(const DIDEVICEINSTANCE& _inst);
		HRESULT UpdateJoystickState(GUID guid);

		
		/// <summary>
		/// <para> �\��̒�����w�肳�ꂽ��ނ̃f�o�C�X��v�����Ă�����̂�T���A�撅���Ŋ��蓖�Ă��s�� </para>
		/// <para> ������Ȃ��ꍇ�͂ǂ̎�ނł��\��Ȃ��Ƃ����\��Ɋ��蓖�Ă�@</para>
		/// </summary>
		/// <param name="_devType">�v������f�o�C�X�̎��</param>
		/// <returns></returns>
		int FindReservationIndexForDevice(DeviceType _devType) const;
		const std::string GetJoystickStatusMessage(GUID guid) const;
		bool IsJoystickConnected(GUID guid) const;
		bool IsJoystickAssigned(GUID guid) const;

	private:

		void StartEnumTimer();
		void StopEnumTimer();
		void AutoEnum();
		// ����I�Ƀf�o�C�X�񋓂��^�C�}�[
		TimerHandle enumTimerHandle_{ nullptr };
		float enumInterval_{ 1.0f };

		/// <summary>
		/// �A�N�e�B�u�ȃR���g���[����ID�𒲂ׂ�
		/// </summary>
		void CheckValidPadID();

		void AcquireJoystick(ComPtr<IDirectInputDevice8> _pJoystickDevice);
		GUID GetDeviceGuid(ComPtr<IDirectInputDevice8> _pInputDevice);
		void SetProperty(ComPtr<IDirectInputDevice8> _pJoystickDevice, InputConfig _inputConfig);
		InputData* pInputData_;				 // ���͂̏��
		ComPtr<IDirectInput8> pDirectInput_;        // Direct Input �{��k
		ComPtr<IDirectInputDevice8> pKeyDevice_;    // �L�[�f�o�C�X
		ComPtr<IDirectInputDevice8> pMouseDevice_;  // �}�E�X�f�o�C�X
		ComPtr<IDirectInputDevice8> pJoystickDevice_;  // �W���C�X�e�B�b�N�f�o�C�X
		 
		std::vector<JoystickReservation> requestedJoystickDevices_;//���蓖�ė\�񂳂ꂽ�W���C�X�e�B�b�N�f�o�C�X
		std::set<GUID> assignedJoystickGuids_;//���Ɋ��蓖�čς݂̃W���C�X�e�B�b�N
		
		std::map<GUID, JoystickContext> joystickContext_;
		GUID currJoystickGuid_;
	};
	
}
