#pragma once

#include "Core/Game.h"
#include "cmtgb.h"

#pragma region Utilities

#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Vector2T.h"
#include "Math/Vector2F.h"
#include "Math/Vector2Int.h"

#include "Math/RectT.h"
#include "Math/RectInt.h"
#include "Math/RectF.h"
#include "WindowContext/WindowContextUtil.h"

#include "Math/Easing.h"
#include "Math/Mathf.h"
#include "Detector/RectDetector.h"
#include "Components/Interpolator/Interpolator.h"

#pragma endregion

#pragma region Systems

#include "AssetsManager.h"
#include "Screen.h"
#include "ScoreManager.h"
#include "Core/Component/ComponentRegistry.h"
#include "Core/GameObject/GameObjectTypeRegistry.h"
#include "Graphics/DirectX11Manager.h"
#include "Direct2D/Direct2D.h"
#include "Graphics/Text/DirectWrite.h"
#include "Components/RigidBody/RigidBodyCP.h"
#include "Components/Transform/TransformCP.h"
#include "Components/TextRenderer/TextRendererCP.h"
#include "Components/ImageRenderer/ImageRendererCP.h"
#include "Components/Collider/ColliderCP.h"
#include "Components/MeshRenderer/MeshRendererCP.h"
#include "Components/Interpolator/InterpolatorCP.h"
#include "Core/Time/GameTime.h"
#include "Core/Time/Timer.h"
#include "Graphics/Draw.h"
#include "Core/SceneSystem.h"
#include "Graphics/RenderSystem.h"
#include "Graphics/EffekseerVFX.h"
#include "Editor/ImGuiEditorCamera.h"
#include "Editor/ImGuizmoManipulator.h"
#include "Editor/ImGuiEditor.h"
#include "Debug.h"
#include "Graphics/Image/Image.h"
#include "Audio/Audio.h"
#include "Editor/ImGuiRenderer.h"
#include "Components/MeshRenderer/MeshRenderer.h"
#include "Components/TextRenderer/TextRenderer.h"
#include "Components/ImageRenderer/ImageRenderer.h"
#include "Graphics/Text/TextCache.h"
#include "Graphics/Model/Fbx.h"
#include "WindowContext/WindowContext.h"
#include "WindowContext/WindowContextResourceManager.h"
#include "WindowManager.h"
#include "SingleWindow.h"
#include "CameraSystem.h"
#include "Editor/ImGuiShowable.h"
#include "Editor/MTImGui.h"
#include "EventManager.h"
#include "Core/EntityManager.h"
#include "Core/GameObject/GameObjectGenerator.h"
#include "Core/Component/ComponentFactory.h"
#include "Graphics/ShadowSettings.h"
#pragma endregion

#include "Core/GameObject/GameObjectBuilder.h"
#include "Core/GameObject/GameObject.h"
#include "ICamera.h"

#pragma region Resource

#include "WindowContext/WindowContextResource.h"
#include "WindowContext/WindowResource.h"
#include "WindowContext/DXGIResource.h"
#include "WindowContext/Direct3DResource.h"
#include "WindowContext/Direct2DResource.h"
#include "WindowContext/InputResource.h"
#include "WindowContext/CameraResource.h"
#include "Input/Input.h"

#pragma endregion
#pragma region Data

#include "Input/InputData.h"
#include "Collision/RectContainsInfo.h"
//
#pragma endregion

#pragma region Unname

using mtgb::AudioHandle;
using mtgb::FBXModelHandle;
using mtgb::ImageHandle;
using mtgb::OBJModelHandle;

using mtgb::RectF;
using mtgb::RectInt;
using mtgb::Vector2;
using mtgb::Vector2F;
using mtgb::Vector2Int;
using mtgb::Vector3;

using mtgb::Collider;
using mtgb::GameObject;
using mtgb::RigidBody;
using mtgb::Transform;

#pragma endregion
