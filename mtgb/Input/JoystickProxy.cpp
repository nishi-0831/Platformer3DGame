#include "JoystickProxy.h"
#include <iterator>
#include "Input.h"

mtgb::JoystickProxy::JoystickProxy(const DIJOYSTATE& _js)
	: lX(_js.lX)
	, lY(_js.lY)
	, lZ(_js.lZ)
	, lRx(_js.lRx)
	, lRy(_js.lRy)
	, lRz(_js.lRz)
{
	std::copy(std::begin(_js.rglSlider), std::end(_js.rglSlider), rglSlider);
	std::copy(std::begin(_js.rgdwPOV), std::end(_js.rgdwPOV), rgdwPOV);
	std::copy(std::begin(_js.rgbButtons), std::end(_js.rgbButtons), rgbButtons);
}

void mtgb::JoystickProxy::UpdateFromInput(GUID _guid)
{
	Input& input	  = Game::System<Input>();
	connectionStatus  = input.IsJoystickConnected(_guid) ? "接続中" : "切断";
	assignmentStatus  = input.IsJoystickAssigned(_guid) ? "割り当て済み" : "未割当";
	lastErrorMessage  = input.GetJoystickStatusMessage(_guid);
	deviceName		  = input.GetDeviceName(_guid);
	deviceProductName = input.GetDeviceProductName(_guid);
}

void mtgb::JoystickProxy::UpdateInputData(const DIJOYSTATE& _js)
{
	lX	= _js.lX;
	lY	= _js.lY;
	lZ	= _js.lZ;
	lRx = _js.lRx;
	lRy = _js.lRy;
	lRz = _js.lRz;
	std::copy(std::begin(_js.rglSlider), std::end(_js.rglSlider), rglSlider);
	std::copy(std::begin(_js.rgdwPOV), std::end(_js.rgdwPOV), rgdwPOV);
	std::copy(std::begin(_js.rgbButtons), std::end(_js.rgbButtons), rgbButtons);
}