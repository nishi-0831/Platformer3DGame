#pragma once
#include <refl.hpp>
#include "Vector3.h"
#include "Quaternion.h"
#include "JoystickProxy.h"
#include "MouseStateProxy.h"
#include "TransformData.h"
#include "TypeRegistry.h"
#include "MeshRendererData.h"
#include "ShowAttributes.h"
#include "ColliderData.h"
#include "RigidBodyData.h"
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


REGISTER_TYPE(TransformData, DisplayName{ "Transform" })
REGISTER_FIELD(parent)
REGISTER_FIELD(position)
REGISTER_FIELD(scale)
REGISTER_FIELD(rotate)
REGISTER_END

REGISTER_TYPE(MeshRendererData, DisplayName{ "MeshRenderer" })
REGISTER_FIELD(meshFileName)
REGISTER_FIELD(meshHandle)
REGISTER_FIELD(layer)
REGISTER_FIELD(shaderType)
REGISTER_END

REGISTER_TYPE(ColliderData, DisplayName{ "Collider" })
REGISTER_FIELD(colliderType)
REGISTER_FIELD(isStatic)
REGISTER_FIELD(colliderTag)
REGISTER_FIELD(center)
REGISTER_FIELD(radius)
REGISTER_FIELD(extents)
REGISTER_END

REGISTER_TYPE(RigidBodyData,DisplayName{"RigidBody"})
REGISTER_FIELD(useGravity)
REGISTER_FIELD(isKinematic)
REGISTER_END