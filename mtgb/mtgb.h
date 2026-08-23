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

#include "Easing.h"
#include "Mathf.h"
#include "Detector/RectDetector.h"
#include "Components/Interpolator/Interpolator.h"

#pragma endregion

#pragma region Systems

#include "AssetsManager.h"
#include "Screen.h"
#include "ScoreManager.h"
#include "Core/Component/ComponentRegistry.h"
#include "Core/GameObject/GameObjectTypeRegistry.h"
#include "DirectX11Manager.h"
#include "Direct2D/Direct2D.h"
#include "Text/DirectWrite.h"
#include "Components/RigidBody/RigidBodyCP.h"
#include "Components/Transform/TransformCP.h"
#include "Components/TextRenderer/TextRendererCP.h"
#include "Components/ImageRenderer/ImageRendererCP.h"
#include "Components/Collider/ColliderCP.h"
#include "Components/MeshRenderer/MeshRendererCP.h"
#include "Components/Interpolator/InterpolatorCP.h"
#include "GameTime.h"
#include "Timer.h"
#include "Draw.h"
#include "SceneSystem.h"
#include "RenderSystem.h"
#include "EffekseerVFX.h"
#include "Editor/ImGuiEditorCamera.h"
#include "Editor/ImGuizmoManipulator.h"
#include "Editor/ImGuiEditor.h"
#include "Debug.h"
#include "Image/Image.h"
#include "Audio/Audio.h"
#include "ImGuiRenderer.h"
#include "Components/MeshRenderer/MeshRenderer.h"
#include "Components/TextRenderer/TextRenderer.h"
#include "Components/ImageRenderer/ImageRenderer.h"
#include "Text/TextCache.h"
#include "Model/Fbx.h"
#include "WindowContext/WindowContext.h"
#include "WindowContext/WindowContextResourceManager.h"
#include "WindowManager.h"
#include "SingleWindow.h"
#include "CameraSystem.h"
#include "ImGuiShowable.h"
#include "Editor/MTImGui.h"
#include "EventManager.h"
#include "Core/EntityManager.h"
#include "Core/GameObject/GameObjectGenerator.h"
#include "Core/Component/ComponentFactory.h"
#include "ShadowSettings.h"
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
#include "RectContainsInfo.h"
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
