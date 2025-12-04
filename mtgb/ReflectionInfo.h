#pragma once
#include <refl.hpp>
#include "Vector3.h"
#include "Quaternion.h"
#include "JoystickProxy.h"
#include "MouseStateProxy.h"
#include "TransformState.h"
#include "TypeRegistry.h"
#include "MeshRendererState.h"
#include "ShowAttributes.h"
#include "ColliderState.h"
#include "RigidBodyState.h"
#include "../Source/MovingFloorState.h"
#include "Interpolator.h"
#include "Matrix4x4.h"
//#include "TypeRegistryImpl.h"
#include <string>
using namespace mtgb;
REGISTER_TYPE(Quaternion, ShowFunc(QuaternionSHow{}))
REGISTER_MEMBER_FUNC(X,property("x"))
REGISTER_MEMBER_FUNC(Y,property("y"))
REGISTER_MEMBER_FUNC(Z,property("z"))
REGISTER_MEMBER_FUNC(W, property("w"))
REGISTER_END

REGISTER_TYPE(Vector3,ShowFunc(Vector3Show{}))
REGISTER_FIELD(x)
REGISTER_FIELD(y)
REGISTER_FIELD(z)
REGISTER_END

REGISTER_TYPE(Matrix4x4,ShowFunc(MatrixShow{}))
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

REGISTER_TYPE(Interpolator)
REGISTER_FIELD(dir)
REGISTER_FIELD(elapsed)
REGISTER_FIELD(duration)
REGISTER_FIELD(start)
REGISTER_FIELD(end)
REGISTER_END

REGISTER_TYPE(TransformState, DisplayName{ "Transform" })
REGISTER_FIELD(parent)
REGISTER_FIELD(position)
REGISTER_FIELD(scale)
REGISTER_FIELD(rotate)
REGISTER_END

REGISTER_TYPE(MeshRendererState, DisplayName{ "MeshRenderer" })
REGISTER_FIELD(meshFileName)
REGISTER_FIELD(meshHandle)
REGISTER_FIELD(layer)
REGISTER_FIELD(shaderType)
REGISTER_END

REGISTER_TYPE(ColliderState, DisplayName{ "Collider" })
REGISTER_FIELD(colliderType)
REGISTER_FIELD(isStatic)
REGISTER_FIELD(colliderTag)
REGISTER_FIELD(center)
REGISTER_FIELD(radius)
REGISTER_FIELD(extents)
REGISTER_END

REGISTER_TYPE(RigidBodyState,DisplayName{"RigidBody"})
REGISTER_FIELD(useGravity)
REGISTER_FIELD(isKinematic)
REGISTER_END

REGISTER_TYPE(MovingFloorState,DisplayName{"MovingFloor"})
REGISTER_FIELD(duration)
REGISTER_FIELD(to)
REGISTER_FIELD(from)
REGISTER_END