#include "MouseStateProxy.h"
#include <array>
#include <iterator>
mtgb::MouseStateProxy::MouseStateProxy(const _DIMOUSESTATE& _ms)
	: lX{_ms.lX}
	, lY{_ms.lY}
	, lZ{_ms.lZ}
{
	std::copy(std::begin(_ms.rgbButtons), std::end(_ms.rgbButtons), rgbButtons);
}

void mtgb::MouseStateProxy::UpdateInputData(const _DIMOUSESTATE& _ms)
{
	lX = {_ms.lX  };
	lY = { _ms.lY };
	lZ = { _ms.lZ };

	std::copy(std::begin(_ms.rgbButtons), std::end(_ms.rgbButtons), rgbButtons);

}
