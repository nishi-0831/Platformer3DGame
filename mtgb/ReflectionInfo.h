#pragma once
#include <refl-cpp/refl.hpp>
#include "Vector3.h"
#include "Quaternion.h"
#include "JoystickProxy.h"
#include "MouseStateProxy.h"
#include "PropertyDisplayRegistry.h"
#include "ShowAttributes.h"
#include "Interpolator.h"
#include "Matrix4x4.h"
#include "RectF.h"
#include "UIDrawCommand.h"
// #include "TypeRegistryImpl.h"
#include "ShowFunc.h"
using namespace mtgb;
REGISTER_TYPE(Quaternion, ShowFunc(QuaternionShow {}))
REGISTER_MEMBER_FUNC(X, property("x"))
REGISTER_MEMBER_FUNC(Y, property("y"))
REGISTER_MEMBER_FUNC(Z, property("z"))
REGISTER_MEMBER_FUNC(W, property("w"))
REGISTER_END

REGISTER_TYPE(Vector3, ShowFunc(Vector3Show {}))
REGISTER_FIELD(x)
REGISTER_FIELD(y)
REGISTER_FIELD(z)
REGISTER_END

REGISTER_TYPE(Matrix4x4, ShowFunc(MatrixShow {}))
REGISTER_END

REGISTER_TYPE(JoystickProxy)
REGISTER_FIELD(lX)
REGISTER_FIELD(lY)
REGISTER_FIELD(lZ)
REGISTER_FIELD(lRx)
REGISTER_FIELD(lRy)
REGISTER_FIELD(lRz)
REGISTER_FIELD(rglSlider)
REGISTER_FIELD(rgdwPOV)
REGISTER_FIELD(rgbButtons)
REGISTER_FIELD(connectionStatus)
REGISTER_FIELD(assignmentStatus)
REGISTER_FIELD(lastErrorMessage)
REGISTER_FIELD(deviceName)
REGISTER_FIELD(deviceProductName)
REGISTER_END

REGISTER_TYPE(MouseStateProxy)
REGISTER_FIELD(lX)
REGISTER_FIELD(lY)
REGISTER_FIELD(lZ)
REGISTER_FIELD(rgbButtons)
REGISTER_END

REGISTER_TYPE(RectF)
REGISTER_FIELD(x)
REGISTER_FIELD(y)
REGISTER_FIELD(width)
REGISTER_FIELD(height)
REGISTER_END

REGISTER_TYPE(UIParams)
REGISTER_FIELD(depth)
REGISTER_FIELD(layerFlag)
REGISTER_END