cmake_minimum_required(VERSION 3.17.0)

# 使用するOSのバージョン
set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)
# プロジェクト名 使用言語
project(Game LANGUAGES C CXX)
set(PROJECT_NAME Project1)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"
     OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/Release)

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

# compile_commands.jsonの生成を有効にする
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

################################################################################
# Global compiler options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    # ビルドモード共通のコンパイルオプション
    set(CMAKE_CXX_FLAGS "")
    # デバッグビルド時に追加されるコンパイルオプション
    set(CMAKE_CXX_FLAGS_DEBUG "")
    # リリースビルド時に追加されるコンパイルオプション
    set(CMAKE_CXX_FLAGS_RELEASE "")
endif()

################################################################################
# Global linker options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
endif()


################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
# ファイルのフィルタリング機能を有効にする
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################

################################################################################
# Source groups
# source_group("任意のフィルタ名" FILES <ソースコードのパス>)
################################################################################
set(no_group_source_files
    ".clang-format"
    ".pre-commit-config.yaml"
)
source_group("" FILES ${no_group_source_files})

set(01Game
    "mtgb/Box3D.cpp"
    "mtgb/Box3D.h"
    "Source/ActorManager.h"
    "Source/QuaternionCamera.cpp"
    "Source/QuaternionCamera.h"
    "Source/CircularSaw.h"
    "Source/FadeOutScreen.h"
    "Source/GameEvents.h"
    "Source/GameOverZone.h"
    "Source/Goal.cpp"
    "Source/Goal.h"
    "Source/HPViewer.h"
    "Source/IActor.h"
    "Source/MovingFloor.h"
    "Source/PatrolChargerEnemy.h"
    "Source/Player.h"
    "Source/RegisterGameObjectType.h"
    "Source/SampleGame.cpp"
    "Source/SampleGame.h"
    "Source/Scenes/SampleScene.cpp"
    "Source/Scenes/SampleScene.h"
    "Source/ScoreItem.h"
    "Source/SeaPlane.h"
    "Source/SkySphere.h"
    "Source/StageEditScene.h"
    "Source/StageID.h"
    "Source/StageManager.h"
    "Source/TheMain.cpp"
    "Source/TitleScene.cpp"
    "Source/TitleScene.h"
    "Source/Slider.cpp"
    "Source/Slider.h"
)
source_group("01Game" FILES ${01Game})

set(02Assets__Shader
    "Assets/Shader/2DCommon.hlsli"
    "Assets/Shader/3DCommon.hlsli"
)
source_group("02Assets\\Shader" FILES ${02Assets__Shader})

set(03mtgb
    "mtgb/ReflectionMacro.h"
    "mtgb/stdafx.cpp"
    "mtgb/stdafx.h"
)
source_group("03mtgb" FILES ${03mtgb})

set(03mtgb__Component
    "mtgb/Collider.cpp"
    "mtgb/Collider.h"
    "mtgb/ComponentConcept.h"
    "mtgb/IComponent.cpp"
    "mtgb/IComponent.h"
    "mtgb/IGameObjectCreator.h"
    "mtgb/ImageRenderer.cpp"
    "mtgb/ImageRenderer.h"
    "mtgb/Interpolator.cpp"
    "mtgb/IRenderable.h"
    "mtgb/ISerializableObject.h"
    "mtgb/MeshRenderer.cpp"
    "mtgb/RigidBody.cpp"
    "mtgb/RigidBody.h"
    "mtgb/TextRenderer.cpp"
    "mtgb/TextRenderer.h"
    "mtgb/Transform.cpp"
    "mtgb/Transform.h"
)
source_group("03mtgb\\Component" FILES ${03mtgb__Component})

set(03mtgb__Component__Data
)
source_group("03mtgb\\Component\\Data" FILES ${03mtgb__Component__Data})

set(03mtgb__Component__Mement
    "mtgb/IComponentMemento.h"
)
source_group("03mtgb\\Component\\Mement" FILES ${03mtgb__Component__Mement})


source_group("03mtgb\\Component\\StatefulComponent" FILES ${03mtgb__Component__StatefulComponent})

set(03mtgb__Core
    "mtgb/cmtgb.h"
    "mtgb/EntityManager.cpp"
    "mtgb/EntityManager.h"
    "mtgb/Game.cpp"
    "mtgb/Game.h"
    "mtgb/GameObject.cpp"
    "mtgb/GameObject.h"
    "mtgb/GameObjectBuilder.h"
    "mtgb/GameObjectLayer.h"
    "mtgb/GameObjectTag.h"
    "mtgb/GameScene.cpp"
    "mtgb/GameScene.h"
    "mtgb/Handlers.h"
    "mtgb/mtgb.cpp"
    "mtgb/mtgb.h"
)
source_group("03mtgb\\Core" FILES ${03mtgb__Core})

set(03mtgb__CoreType
    "mtgb/ColliderType.h"
    "mtgb/Color.cpp"
    "mtgb/Color.h"
    "mtgb/Command.h"
    "mtgb/Matrix4x4.h"
    "mtgb/Quaternion.cpp"
    "mtgb/Quaternion.h"
    "mtgb/RectF.h"
    "mtgb/RectInt.h"
    "mtgb/RectT.h"
    "mtgb/Vector2.cpp"
    "mtgb/Vector2.h"
    "mtgb/Vector2F.h"
    "mtgb/Vector2Int.cpp"
    "mtgb/Vector2Int.h"
    "mtgb/Vector2T.h"
    "mtgb/Vector3.cpp"
    "mtgb/Vector3.h"
    "mtgb/Vector4.cpp"
    "mtgb/Vector4.h"
)
source_group("03mtgb\\CoreType" FILES ${03mtgb__CoreType})

set(03mtgb__Includer
    "mtgb/IncludingInput.h"
    "mtgb/IncludingWindows.h"
)
source_group("03mtgb\\Includer" FILES ${03mtgb__Includer})

set(03mtgb__Library__MTBin
    "mtgb/MTBin/Common.h"
    "mtgb/MTBin/MemoryStream.cpp"
    "mtgb/MTBin/MemoryStream.h"
)
source_group("03mtgb\\Library\\MTBin" FILES ${03mtgb__Library__MTBin})

set(03mtgb__Library__MTStat
    "mtgb/MTStat/MTStat.h"
)
source_group("03mtgb\\Library\\MTStat" FILES ${03mtgb__Library__MTStat})

set(03mtgb__Library__Nlohmann-json
    "mtgb/JsonADLSerializers.h"
)
source_group("03mtgb\\Library\\Nlohmann-json" FILES ${03mtgb__Library__Nlohmann-json})

set(03mtgb__Shader
    "mtgb/FbxParts.cpp"
    "mtgb/FbxParts.h"
    "mtgb/Figure.cpp"
    "mtgb/Figure.h"
    "mtgb/IShader.cpp"
    "mtgb/IShader.h"
    "mtgb/PlaneUVScroll.h"
    "mtgb/Sprite.cpp"
    "mtgb/Sprite.h"
)
source_group("03mtgb\\Shader" FILES ${03mtgb__Shader})

set(03mtgb__System
    "mtgb/ComponentFactory.h"
    "mtgb/EventManager.cpp"
    "mtgb/EventManager.h"
    "mtgb/GameObjectFactory.h"
    "mtgb/GameObjectTypeRegistry.h"
    "mtgb/IGameObjectRegistrar.h"
    "mtgb/ISystem.cpp"
    "mtgb/ISystem.h"
    "mtgb/RegisterCommonGameObjectType.cpp"
    "mtgb/RegisterCommonGameObjectType.h"
)
source_group("03mtgb\\System" FILES ${03mtgb__System})

set(03mtgb__System__AssetsManager
    "mtgb/AssetsManager.cpp"
    "mtgb/AssetsManager.h"
)
source_group("03mtgb\\System\\AssetsManager" FILES ${03mtgb__System__AssetsManager})

set(03mtgb__System__Audio
    "mtgb/Audio.cpp"
    "mtgb/Audio.h"
    "mtgb/WaveData.cpp"
    "mtgb/WaveData.h"
)
source_group("03mtgb\\System\\Audio" FILES ${03mtgb__System__Audio})

set(03mtgb__System__Camera
    "mtgb/CameraSystem.cpp"
    "mtgb/WorldToScreenData.h"
)
source_group("03mtgb\\System\\Camera" FILES ${03mtgb__System__Camera})

set(03mtgb__System__Collider
    "mtgb/RectDetector.h"
)
source_group("03mtgb\\System\\Collider" FILES ${03mtgb__System__Collider})

set(03mtgb__System__Collider__ColliderInfo
    "mtgb/RectContainsInfo.h"
)
source_group("03mtgb\\System\\Collider\\ColliderInfo" FILES ${03mtgb__System__Collider__ColliderInfo})

set(03mtgb__System__ComponentPool
    "mtgb/ColliderCP.cpp"
    "mtgb/ComponentPool.cpp"
    "mtgb/ComponentPool.h"
    "mtgb/ComponentRegistry.h"
    "mtgb/IComponentPool.cpp"
    "mtgb/IComponentPool.h"
    "mtgb/ImageRendererCP.h"
    "mtgb/MeshRendererCP.h"
    "mtgb/RigidBodyCP.cpp"
    "mtgb/RigidBodyCP.h"
    "mtgb/TextRendererCP.cpp"
    "mtgb/TextRendererCP.h"
    "mtgb/TransformCP.cpp"
    "mtgb/TransformCP.h"
)
source_group("03mtgb\\System\\ComponentPool" FILES ${03mtgb__System__ComponentPool})

set(03mtgb__System__Debug
    "mtgb/Debug.cpp"
    "mtgb/Debug.h"
)
source_group("03mtgb\\System\\Debug" FILES ${03mtgb__System__Debug})

set(03mtgb__System__Debug__Command
    "mtgb/AddComponentCommand.cpp"
    "mtgb/AddComponentCommand.h"
    "mtgb/AddComponentCommandGeneric.h"
    "mtgb/CommandHistory.cpp"
    "mtgb/CommandHistoryManager.h"
    "mtgb/NamedCommandHistory.cpp"
    "mtgb/DeleteGameObjectCommand.cpp"
    "mtgb/DeleteGameObjectCommand.h"
    "mtgb/DuplicateGameObjectCommand.h"
    "mtgb/GameObjectCreateCommand.cpp"
    "mtgb/GameObjectCreateCommand.h"
    "mtgb/GroupCommand.h"
    "mtgb/GuizmoManipulatedEvent.cpp"
    "mtgb/GuizmoManipulatedEvent.h"
    "mtgb/ImGuiInputCommand.h"
    "mtgb/PropertyChangedEvent.cpp"
    "mtgb/PropertyChangedEvent.h"
    "mtgb/SelectionCommand.cpp"
    "mtgb/SelectionCommand.h"
)
source_group("03mtgb\\System\\Debug\\Command" FILES ${03mtgb__System__Debug__Command})

set(03mtgb__System__Debug__MTImGui
    "mtgb/GameObjectSelectionEvent.h"
    "mtgb/ImGuiUtil.h"
    "mtgb/JoystickProxy.cpp"
    "mtgb/JoystickProxy.h"
    "mtgb/MouseStateProxy.h"
    "mtgb/MTImGui.cpp"
    "mtgb/MTImGui.h"
)
source_group("03mtgb\\System\\Debug\\MTImGui" FILES ${03mtgb__System__Debug__MTImGui})

set(03mtgb__System__Debug__MTImGui__Editor
    "mtgb/CommandHistory.h"
    "mtgb/NamedCommandHistory.h"
    "mtgb/GameObjectGenerator.h"
    "mtgb/ImGuiEditor.cpp"
    "mtgb/ImGuiEditor.h"
    "mtgb/ImGuiEditorCamera.cpp"
    "mtgb/ImGuiEditorCamera.h"
    "mtgb/ImGuizmoManipulator.cpp"
    "mtgb/ImGuizmoManipulator.h"
)
source_group("03mtgb\\System\\Debug\\MTImGui\\Editor" FILES ${03mtgb__System__Debug__MTImGui__Editor})

set(03mtgb__System__Debug__MTImGui__PropertyInspector
    "mtgb/DefaultShow.h"
    "mtgb/DefaultShow.inl"
    "mtgb/ImGuiShowable.cpp"
    "mtgb/ImGuiShowable.h"
    "mtgb/ReflectionInfo.h"
    "mtgb/ShowAttributes.h"
    "mtgb/ShowFunc.h"
    "mtgb/ShowType.h"
    "mtgb/PropertyDisplayRegistry.cpp"
    "mtgb/PropertyDisplayRegistry.h"
    "mtgb/PropertyDisplayRegistryImpl.h"
)
source_group("03mtgb\\System\\Debug\\MTImGui\\PropertyInspector" FILES ${03mtgb__System__Debug__MTImGui__PropertyInspector})

set(03mtgb__System__Direct2D
    "mtgb/Direct2D/Direct2D.cpp"
    "mtgb/Direct2D/Direct2D.h"
    "mtgb/TextAlignment.h"
)
source_group("03mtgb\\System\\Direct2D" FILES ${03mtgb__System__Direct2D})

set(03mtgb__System__Direct2D__DirectWrite
    "mtgb/DirectWrite.cpp"
    "mtgb/DirectWrite.h"
)
source_group("03mtgb\\System\\Direct2D\\DirectWrite" FILES ${03mtgb__System__Direct2D__DirectWrite})

set(03mtgb__System__DirectX11
    "mtgb/DirectX11Draw.cpp"
    "mtgb/DirectX11Draw.h"
    "mtgb/DirectX11Manager.cpp"
    "mtgb/DirectX11Manager.h"
    "mtgb/Draw.cpp"
    "mtgb/Draw.h"
    "mtgb/HLSLInclude.cpp"
    "mtgb/HLSLInclude.h"
    "mtgb/ShaderType.h"
    "mtgb/Texture2D.cpp"
    "mtgb/Texture2D.h"
)
source_group("03mtgb\\System\\DirectX11" FILES ${03mtgb__System__DirectX11})

set(03mtgb__System__Draw
    "mtgb/MeshRenderer.h"
    "mtgb/RenderSystem.cpp"
    "mtgb/RenderSystem.h"
    "mtgb/UIDrawCommand.h"
)
source_group("03mtgb\\System\\Draw" FILES ${03mtgb__System__Draw})

set(03mtgb__System__Draw__FBX
    "mtgb/Fbx.cpp"
    "mtgb/Fbx.h"
    "mtgb/FbxAnimationClip.h"
    "mtgb/FbxAnimationController.h"
    "mtgb/FbxModel.cpp"
    "mtgb/FbxModel.h"
)
source_group("03mtgb\\System\\Draw\\FBX" FILES ${03mtgb__System__Draw__FBX})

set(03mtgb__System__Image
    "mtgb/Image.cpp"
    "mtgb/Image.h"
)
source_group("03mtgb\\System\\Image" FILES ${03mtgb__System__Image})

set(03mtgb__System__Input
    "mtgb/Axis.h"
    "mtgb/Input.cpp"
    "mtgb/Input.h"
    "mtgb/InputConfig.cpp"
    "mtgb/InputConfig.h"
    "mtgb/InputData.cpp"
    "mtgb/InputData.h"
    "mtgb/InputKeyCode.h"
    "mtgb/InputMouseCode.h"
)
source_group("03mtgb\\System\\Input" FILES ${03mtgb__System__Input})

set(03mtgb__System__Library
    "mtgb/ImGuiRenderer.cpp"
    "mtgb/ImGuiRenderer.h"
)
source_group("03mtgb\\System\\Library" FILES ${03mtgb__System__Library})

source_group("03mtgb\\System\\Model" FILES ${03mtgb__System__Model})

set(03mtgb__System__SceneSystem
    "mtgb/SceneSystem.cpp"
    "mtgb/SceneSystem.h"
)
source_group("03mtgb\\System\\SceneSystem" FILES ${03mtgb__System__SceneSystem})

set(03mtgb__System__Screen
    "mtgb/Screen.cpp"
    "mtgb/Screen.h"
)
source_group("03mtgb\\System\\Screen" FILES ${03mtgb__System__Screen})

set(03mtgb__System__Text
    "mtgb/TextCache.cpp"
    "mtgb/TextCache.h"
)
source_group("03mtgb\\System\\Text" FILES ${03mtgb__System__Text})

set(03mtgb__System__Time__GameTime
    "mtgb/GameTime.cpp"
    "mtgb/GameTime.h"
)
source_group("03mtgb\\System\\Time\\GameTime" FILES ${03mtgb__System__Time__GameTime})

set(03mtgb__System__Time__Timer
    "mtgb/Timer.cpp"
    "mtgb/Timer.h"
)
source_group("03mtgb\\System\\Time\\Timer" FILES ${03mtgb__System__Time__Timer})

set(03mtgb__System__Window
    "mtgb/SingleWindow.cpp"
    "mtgb/SingleWindow.h"
)
source_group("03mtgb\\System\\Window" FILES ${03mtgb__System__Window})

set(03mtgb__System__Window__DoubleWindow
    "mtgb/DoubleWindow.cpp"
    "mtgb/DoubleWindow.h"
)
source_group("03mtgb\\System\\Window\\DoubleWindow" FILES ${03mtgb__System__Window__DoubleWindow})

source_group("03mtgb\\System\\Window\\MainWindow" FILES ${03mtgb__System__Window__MainWindow})

source_group("03mtgb\\System\\Window\\SubWindow" FILES ${03mtgb__System__Window__SubWindow})

set(03mtgb__System__Window__WindowContextResourceManager
    "mtgb/CameraResource.cpp"
    "mtgb/CameraResource.h"
    "mtgb/Direct2DResource.cpp"
    "mtgb/Direct2DResource.h"
    "mtgb/Direct3DResource.cpp"
    "mtgb/Direct3DResource.h"
    "mtgb/DXGIResource.cpp"
    "mtgb/DXGIResource.h"
    "mtgb/InputResource.cpp"
    "mtgb/InputResource.h"
    "mtgb/ResourceCollection.cpp"
    "mtgb/ResourceCollection.h"
    "mtgb/WindowContextResource.cpp"
    "mtgb/WindowContextResource.h"
    "mtgb/WindowContextResourceManager.cpp"
    "mtgb/WindowContextResourceManager.h"
    "mtgb/WindowResource.cpp"
    "mtgb/WindowResource.h"
)
source_group("03mtgb\\System\\Window\\WindowContextResourceManager" FILES ${03mtgb__System__Window__WindowContextResourceManager})

set(03mtgb__System__Window__WindowManager
    "mtgb/WindowContext.cpp"
    "mtgb/WindowContext.h"
    "mtgb/WindowManager.cpp"
    "mtgb/WindowManager.h"
)
source_group("03mtgb\\System\\Window\\WindowManager" FILES ${03mtgb__System__Window__WindowManager})

set(03mtgb__X-Ignore
    "mtgb/Entity.cpp"
    "mtgb/Entity.h"
)
source_group("03mtgb\\X-Ignore" FILES ${03mtgb__X-Ignore})

set(04Library__ImGui
    "ThirdParty/ImGui/imconfig.h"
    "ThirdParty/ImGui/imgui.cpp"
    "ThirdParty/ImGui/imgui.h"
    "ThirdParty/ImGui/imgui_demo.cpp"
    "ThirdParty/ImGui/imgui_draw.cpp"
    "ThirdParty/ImGui/imgui_impl_dx11.cpp"
    "ThirdParty/ImGui/imgui_impl_dx11.h"
    "ThirdParty/ImGui/imgui_impl_win32.cpp"
    "ThirdParty/ImGui/imgui_impl_win32.h"
    "ThirdParty/ImGui/imgui_internal.h"
    "ThirdParty/ImGui/imgui_ja_gryph_ranges.cpp"
    "ThirdParty/ImGui/imgui_tables.cpp"
    "ThirdParty/ImGui/imgui_widgets.cpp"
    "ThirdParty/ImGui/imstb_rectpack.h"
    "ThirdParty/ImGui/imstb_textedit.h"
    "ThirdParty/ImGui/imstb_truetype.h"
)
source_group("04Library\\ImGui" FILES ${04Library__ImGui})

set(04Library__ImGui__ImGuizmo
    "ThirdParty/ImGui/GraphEditor.cpp"
    "ThirdParty/ImGui/GraphEditor.h"
    "ThirdParty/ImGui/ImCurveEdit.cpp"
    "ThirdParty/ImGui/ImCurveEdit.h"
    "ThirdParty/ImGui/ImGradient.cpp"
    "ThirdParty/ImGui/ImGradient.h"
    "ThirdParty/ImGui/ImGuizmo.cpp"
    "ThirdParty/ImGui/ImGuizmo.h"
    "ThirdParty/ImGui/ImSequencer.cpp"
    "ThirdParty/ImGui/ImSequencer.h"
    "ThirdParty/ImGui/ImZoomSlider.h"
)
source_group("04Library\\ImGui\\ImGuizmo" FILES ${04Library__ImGui__ImGuizmo})

set(Workflow
    ".github/workflows/DeployDoxygen.yml"
    ".github/workflows/ReleaseBuildAndPublish.yml"
)
source_group("Workflow" FILES ${Workflow})

set(Workflow__Utility
    "mtgb/Easing.h"
    "mtgb/Interpolator.h"
    "mtgb/JsonConverter.h"
    "mtgb/Mathf.h"
    "mtgb/MTAssert.h"
    "mtgb/MTStringUtility.cpp"
    "mtgb/MTStringUtility.h"
    "mtgb/ProfileUtlity.h"
    "mtgb/ReleaseUtility.h"
    "mtgb/StringComparators.h"
    "mtgb/WindowContextUtil.cpp"
    "mtgb/WindowContextUtil.h"
)
source_group("Workflow\\Utility" FILES ${Workflow__Utility})

set(Unfiltered
    "mtgb/CameraSystem.h"
    "mtgb/CircleDetector.cpp"
    "mtgb/CircleDetector.h"
    "mtgb/CommandHistoryManager.cpp"
    "mtgb/ComponentFactory.cpp"
    "mtgb/ComponentRegistry.cpp"
    "mtgb/DetectorConfigs.h"
    "mtgb/DuplicateGameObjectCommand.cpp"
    "mtgb/EffekseerVFX.cpp"
    "mtgb/EffekseerVFX.h"
    "mtgb/FbxAnimationClip.cpp"
    "mtgb/FbxAnimationController.cpp"
    "mtgb/GameObjectFactory.cpp"
    "mtgb/GameObjectGenerator.cpp"
    "mtgb/GameObjectLayer.cpp"
    "mtgb/GameObjectTypeRegistry.cpp"
    "mtgb/GroupCommand.cpp"
    "mtgb/ICamera.cpp"
    "mtgb/ICamera.h"
    "mtgb/IDetector.h"
    "mtgb/ImageRendererCP.cpp"
    "mtgb/ImGuiUtil.cpp"
    "mtgb/InputPadCode.h"
    "mtgb/InterpolatorCP.cpp"
    "mtgb/InterpolatorCP.h"
    "mtgb/JsonConverter.cpp"
    "mtgb/MeshRendererCP.cpp"
    "mtgb/MonitorInfo.cpp"
    "mtgb/MonitorInfo.h"
    "mtgb/MouseStateProxy.cpp"
    "mtgb/MTBit/BitFlag.h"
    "mtgb/PlaneUVScroll.cpp"
    "mtgb/QuatToEuler.cpp"
    "mtgb/QuatToEuler.h"
    "mtgb/RayDetector.cpp"
    "mtgb/RayDetector.h"
    "mtgb/RectDetector.cpp"
    "mtgb/ScoreManager.cpp"
    "mtgb/ScoreManager.h"
    "mtgb/ShowAttributes.cpp"
    "mtgb/UIDrawCommand.cpp"
    "Source/ActorManager.cpp"
    "Source/CircularSaw.cpp"
    "Source/FadeOutScreen.cpp"
    "Source/GameOverManager.cpp"
    "Source/GameOverManager.h"
    "Source/GameOverZone.cpp"
    "Source/HPViewer.cpp"
    "Source/IActor.cpp"
    "Source/MovingFloor.cpp"
    "Source/PatrolChargerEnemy.cpp"
    "Source/Player.cpp"
    "Source/RegisterGameObjectType.cpp"
    "Source/ResultScene.cpp"
    "Source/ResultScene.h"
    "Source/ScoreItem.cpp"
    "Source/SeaPlane.cpp"
    "Source/SkySphere.cpp"
    "Source/StageEditScene.cpp"
    "Source/StageManager.cpp"
    "Source/JumpController.cpp"
    "Source/JumpController.h"
    "Source/RespawnManager.cpp"
    "Source/RespawnManager.h"
    "Source/Button.cpp"
    "Source/Button.h"
    "Source/Panel.h"
    "Source/Panel.cpp"
    "Source/PanelManager.h"
    "Source/PanelManager.cpp"
)
source_group("Unfiltered" FILES ${Unfiltered})

set(ALL_FILES
    ${no_group_source_files}
    ${01Game}
    ${02Assets__Shader}
    ${03mtgb}
    ${03mtgb__Component}
    ${03mtgb__Component__Data}
    ${03mtgb__Component__Mement}
    ${03mtgb__Component__StatefulComponent}
    ${03mtgb__Core}
    ${03mtgb__CoreType}
    ${03mtgb__Includer}
    ${03mtgb__Library__MTBin}
    ${03mtgb__Library__MTNet}
    ${03mtgb__Library__MTStat}
    ${03mtgb__Library__Nlohmann-json}
    ${03mtgb__Shader}
    ${03mtgb__System}
    ${03mtgb__System__AssetsManager}
    ${03mtgb__System__Audio}
    ${03mtgb__System__Camera}
    ${03mtgb__System__Collider}
    ${03mtgb__System__Collider__ColliderInfo}
    ${03mtgb__System__ComponentPool}
    ${03mtgb__System__Debug}
    ${03mtgb__System__Debug__Command}
    ${03mtgb__System__Debug__MTImGui}
    ${03mtgb__System__Debug__MTImGui__Editor}
    ${03mtgb__System__Debug__MTImGui__PropertyInspector}
    ${03mtgb__System__Direct2D}
    ${03mtgb__System__Direct2D__DirectWrite}
    ${03mtgb__System__DirectX11}
    ${03mtgb__System__Draw}
    ${03mtgb__System__Draw__FBX}
    ${03mtgb__System__Draw__OBJ}
    ${03mtgb__System__Image}
    ${03mtgb__System__Input}
    ${03mtgb__System__Library}
    ${03mtgb__System__Model}
    ${03mtgb__System__SceneSystem}
    ${03mtgb__System__Screen}
    ${03mtgb__System__Text}
    ${03mtgb__System__Time__GameTime}
    ${03mtgb__System__Time__Timer}
    ${03mtgb__System__Window}
    ${03mtgb__System__Window__DoubleWindow}
    ${03mtgb__System__Window__MainWindow}
    ${03mtgb__System__Window__SubWindow}
    ${03mtgb__System__Window__WindowContextResourceManager}
    ${03mtgb__System__Window__WindowManager}
    ${03mtgb__X-Ignore}
    ${04Library__ImGui}
    ${04Library__ImGui__ImGuizmo}
    ${Workflow}
    ${Workflow__Utility}
    ${Unfiltered}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES} "mtgb/Intersection.h" "mtgb/Intersection.cpp" "mtgb/CallbackConcepts.h" "Source/UIComponent.h" "Source/UIComponent.cpp" "mtgb/SerializableGameObject.h" "mtgb/SerializableGameObject.cpp" "mtgb/ShadowSettings.h" "mtgb/ShadowSettings.cpp" "Source/RangedAttackEnemy.h" "Source/RangedAttackEnemy.cpp")

# プリコンパイル済みヘッダを設定する
target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/mtgb/stdafx.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE AirPlaneGame)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Release>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        # リリースビルド時のインクルードディレクトリ
        "$<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Program Files/Autodesk/FBX/FBX SDK/2020.3.7/include;$ENV{REFL_CPP}>"
        # どのビルド・モードでも共通
        "$ENV{FBX_SDK}/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty;"
        "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/EffekseeLib/install_msvc2022_x64/include/Effekseer;"
        "${CMAKE_CURRENT_SOURCE_DIR}/mtgb"
        "${CMAKE_CURRENT_SOURCE_DIR}/Generated"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        # デバッグビルド
        "$<$<CONFIG:Debug>:_DEBUG>"
        # リリースビルド
        "$<$<CONFIG:Release>:NDEBUG>"
        "_WINDOWS;"
        "_AMD64_;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:_DEBUG>"
        "$<$<CONFIG:Release>:NDEBUG>"
        "WIN32;"
        "_WINDOWS;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

set(MSVC_COMMON_OPTIONS
    # デバッグビルド時のオプション
    $<$<CONFIG:Debug>:/wd4005>
    # リリースビルド時のオプション
    $<$<CONFIG:Release>:/Oi;/Gy>
    # C++の標準規格に準拠したコンパイルを強制する
    /permissive-;
    # __cplusplusの値でC++のバージョンを判定している箇所が正しく機能するように
    /Zc:__cplusplus;
    /sdl;
    /W4;
    # 無名共用体/構造体を定義した際の警告を抑制
    /wd4201;
    # 認識されない属性を付与した際の警告を抑制
    /wd5030;
    # 警告をエラーとして扱う
    # /WX;
    /FI ${CMAKE_CURRENT_SOURCE_DIR}/mtgb/stdafx.h
    ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
    /utf-8;
    ${DEFAULT_CXX_EXCEPTION_HANDLING}
    /Od
)

set(CLANGCL_ADDITIONAL_OPTIONS
    $<$<CONFIG:Debug>:/Zi;-g;-O0;-fno-omit-frame-pointer>
    -fms-extensions 
    -fms-compatibility 
    -fms-compatibility-version=19.0
    -D_MSC_VER=1928 
    -target x86_64-pc-windows-msvc 
    -fdelayed-template-parsing 
    -fno-complete-member-pointers 
    -Wno-c++98-compat 
    -Wno-c++98-compat-pedantic 
    -Wno-reserved-id-macro
    -Wno-pragma-pack 
    -Wno-unknown-pragmas
    -Wno-unused-command-line-argument
)
################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER MATCHES ".*clang-cl.*")
        set_target_properties(${PROJECT_NAME} PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
        )
    else()
        target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            # 指定したC++標準バージョンを必須とする。コンパイラがサポートしていないならばビルド失敗
            CXX_STANDARD_REQUIRED ON
            # コンパイラ固有の拡張機能を無効にする
            CXX_EXTENSIONS OFF
    )
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${PROJECT_NAME} PRIVATE ${MSVC_COMMON_OPTIONS})
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER MATCHES ".*clang-cl.*")
        target_compile_options(${PROJECT_NAME} PRIVATE ${MSVC_COMMON_OPTIONS} ${CLANGCL_ADDITIONAL_OPTIONS})
    endif()

    # "/Y-" はプリコンパイル済みヘッダを無視するオプション
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    # プリコンパイル済みヘッダを無視するソース
    source_file_compile_options(ImGui/GraphEditor.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/ImCurveEdit.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/ImGradient.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/ImGuizmo.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/imgui_ja_gryph_ranges.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/ImSequencer.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/imgui.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/imgui_demo.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/imgui_draw.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/imgui_impl_dx11.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/imgui_impl_win32.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/imgui_tables.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "/Y-"
        )
    endif()
    source_file_compile_options(ImGui/imgui_widgets.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:NOREF;
                /OPT:NOICF
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()

################################################################################
# Pre build events (ビルド前イベント)
################################################################################

# コード生成ツールを実行するコマンド
set(GENERATE_SOURCE_CODE_COMMAND "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/Tools/ClangSourceGenerator/bin/ClangSourceGenerator.exe> " --config "Tools/ClangSourceGenerator" --projDir "${CMAKE_CURRENT_SOURCE_DIR}")
 add_custom_target(generate_source_code
    COMMAND ${GENERATE_SOURCE_CODE_COMMAND}
    USES_TERMINAL)
# コンパイル前に実行されるように依存させる
 add_dependencies(${PROJECT_NAME} generate_source_code)

 # キャッシュを削除し、再生成を行う
 add_custom_target(force_generate_source_code
    COMMAND ${GENERATE_SOURCE_CODE_COMMAND} --force
    USES_TERMINAL
 )

 if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
     set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
     set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
endif()

 add_custom_target(synchronize_assets
    COMMAND powershell -NoProfile -ExecutionPolicy Bypass -File "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/Tools/CopyAssets.ps1>"
            -AssetsDirectory "${CMAKE_CURRENT_SOURCE_DIR}" -OutputDirectory "${OUTPUT_DIRECTORY}"
    USES_TERMINAL
 )
 add_dependencies(${PROJECT_NAME} synchronize_assets)

 include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/RunClangTidy.cmake")
 include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/GeneratePch.cmake")

 if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Generated")
    file(GLOB_RECURSE GENERATED_SOURCE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Generated/*.gen.cpp")
    target_sources(${PROJECT_NAME}
        PRIVATE ${GENERATED_SOURCE})
    source_group("Generated" FILES ${GENERATED_SOURCE})
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/Generated"
    )
 endif()

################################################################################
# Dependencies (リンク先)
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PRIVATE
        # デバッグビルド
        "$<$<CONFIG:Debug>:"
            "$ENV{FBX_SDK}/lib/x64/debug;"
        ">"
        # リリースビルド
        "$<$<CONFIG:Release>:"
            "$ENV{FBX_SDK}/lib/x64/release;"
        ">"
        # どのビルドモードでも共通
        "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/EffekseeLib/install_msvc2022_x64/lib"
    )
endif()